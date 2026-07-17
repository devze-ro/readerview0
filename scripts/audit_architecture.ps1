param([string]$RepoRoot = (Split-Path -Parent $PSScriptRoot))

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path -LiteralPath $RepoRoot).Path
$Code = Join-Path $RepoRoot "code"
$Failures = [System.Collections.Generic.List[string]]::new()
$Required = @(
  "code\readerview0_version.h",
  "code\readerview0.h",
  "code\readerview0.c",
  "code\readerview0_sources.manifest",
  "code\reader_view\reader_view.h",
  "code\reader_view\reader_view.c",
  "code\reader_view\reader_view_debug.h",
  "code\reader_view\reader_view_debug.c"
)
foreach ($relative in $Required) {
  if (!(Test-Path -LiteralPath (Join-Path $RepoRoot $relative) -PathType Leaf)) {
    $Failures.Add("missing required package file: $relative")
  }
}

$Unity = Join-Path $Code "readerview0.c"
if (Test-Path -LiteralPath $Unity) {
  $UnityText = [System.IO.File]::ReadAllText($Unity)
  $Count = ([regex]::Matches($UnityText, 'reader_view/reader_view\.c')).Count
  if ($Count -ne 1) { $Failures.Add("readerview0.c must include reader_view.c exactly once") }
  $DebugCount = ([regex]::Matches($UnityText, 'reader_view/reader_view_debug\.c')).Count
  if ($DebugCount -ne 1) { $Failures.Add("readerview0.c must include reader_view_debug.c exactly once") }
  if ($UnityText -match 'ui0\.c') { $Failures.Add("readerview0.c must not embed UI0") }
}

$Header = Join-Path $Code "readerview0.h"
if (Test-Path -LiteralPath $Header) {
  $HeaderText = [System.IO.File]::ReadAllText($Header)
  foreach ($requiredText in @('#include "ui0.h"', 'UI0_API_VERSION != READERVIEW0_UI0_REQUIRED_API_VERSION')) {
    if (!$HeaderText.Contains($requiredText)) { $Failures.Add("readerview0.h missing: $requiredText") }
  }
}

$ReaderViewHeader = Join-Path $Code "reader_view\reader_view.h"
if (Test-Path -LiteralPath $ReaderViewHeader) {
  $ReaderViewHeaderText = [System.IO.File]::ReadAllText($ReaderViewHeader)
  foreach ($requiredText in @(
    'READER_VIEW_REFERENCE_TOP_CHROME_HEIGHT',
    'READER_VIEW_REFERENCE_FOOTER_HEIGHT',
    'ReaderViewText chrome_title',
    'UI0Rect page_surface_rect',
    'UI0Rect content_rect',
    'ReaderViewContentGeometryStyle',
    'ReaderViewContentGeometry',
    'reader_view_default_content_geometry_style',
    'reader_view_resolve_content_geometry'
  )) {
    if (!$ReaderViewHeaderText.Contains($requiredText)) { $Failures.Add("reader_view.h missing API 3 surface: $requiredText") }
  }
}

$Manifest = Join-Path $Code "readerview0_sources.manifest"
if (Test-Path -LiteralPath $Manifest) {
  $ManifestText = [System.IO.File]::ReadAllText($Manifest)
  foreach ($requiredText in @("source=reader_view/reader_view.c", "diagnostic_source=reader_view/reader_view_debug.c", "external_source=ui0/code/ui0.c")) {
    if (!$ManifestText.Contains($requiredText)) { $Failures.Add("source manifest missing: $requiredText") }
  }
}

$Forbidden = @(
  @{ Pattern = '\b(?:reader0|re10|lectern0|zero_foundation)\b'; Message = "forbidden repository dependency" },
  @{ Pattern = '#\s*include\s*[<\"](?:windows|windowsx|commctrl|d2d1|dwrite|wincodec|sqlite3)\.h'; Message = "forbidden platform or database include" },
  @{ Pattern = '\b(?:malloc|calloc|realloc|free|HeapAlloc|CreateWindow|sqlite3_)\s*\('; Message = "forbidden allocation/platform/persistence call" },
  @{ Pattern = '\b(?:callback|provider_table|vtable|event_bus|dependency_injection)\b'; Message = "forbidden indirection framework" }
)
Get-ChildItem -LiteralPath (Join-Path $Code "reader_view") -File -Recurse | Where-Object {
  $_.Extension -eq ".c" -or $_.Extension -eq ".h"
} | ForEach-Object {
  $path = $_.FullName
  $relative = $path.Substring($RepoRoot.Length).TrimStart('\')
  $line = 0
  Get-Content -LiteralPath $path | ForEach-Object {
    $line += 1
    foreach ($rule in $Forbidden) {
      if ($_ -match $rule.Pattern) { $Failures.Add("${relative}:${line}: $($rule.Message)") }
    }
  }
}

if ($Failures.Count -gt 0) {
  $Failures | ForEach-Object { Write-Error $_ }
  exit 1
}
Write-Host "readerview0 architecture audit: pass"
Write-Host "dependency direction: application -> readerview0 API 3 -> UI0 API 91"
Write-Host "document, persistence, rendering, native accessibility, and product policy: absent"
exit 0
