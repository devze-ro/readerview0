param([switch]$FailWhenStale)

$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Metadata = Join-Path $Root "vendor\ui0_dependency"
if ($env:READERVIEW0_UI0_DIR) {
  $Ui0 = $env:READERVIEW0_UI0_DIR
} else {
  $Ui0 = Join-Path $Root "..\ui0"
}
if ($env:READERVIEW0_ZERO_FOUNDATION_DIR) {
  $Foundation = $env:READERVIEW0_ZERO_FOUNDATION_DIR
} elseif ($env:UI0_ZERO_FOUNDATION_DIR) {
  $Foundation = $env:UI0_ZERO_FOUNDATION_DIR
} else {
  $Foundation = Join-Path $Ui0 "..\zero_foundation"
}

foreach ($path in @($Ui0, $Foundation, $Metadata)) {
  if (!(Test-Path -LiteralPath $path -PathType Container)) {
    Write-Error "missing dependency path: $path"
  }
}

$RequiredCommit = (Get-Content -Raw (Join-Path $Metadata "COMMIT")).Trim()
$RequiredApi = (Get-Content -Raw (Join-Path $Metadata "API_VERSION")).Trim()
$RequiredVersion = (Get-Content -Raw (Join-Path $Metadata "VERSION")).Trim()
$RequiredFoundationCommit = (Get-Content -Raw (Join-Path $Metadata "SOURCE_CLOSURE_ZERO_FOUNDATION_COMMIT")).Trim()
$RequiredFoundationVersion = (Get-Content -Raw (Join-Path $Metadata "SOURCE_CLOSURE_ZERO_FOUNDATION_VERSION")).Trim()
$CurrentCommit = (git -C $Ui0 rev-parse HEAD).Trim()
$CurrentFoundationCommit = (git -C $Foundation rev-parse HEAD).Trim()
$CurrentVersion = (Get-Content -Raw (Join-Path $Ui0 "VERSION")).Trim()
$CurrentFoundationVersion = (Get-Content -Raw (Join-Path $Foundation "VERSION")).Trim()
$VersionHeader = [System.IO.File]::ReadAllText((Join-Path $Ui0 "code\ui0_version.h"))
$ApiMatch = [regex]::Match($VersionHeader, 'UI0_API_VERSION\s+(\d+)')
$CurrentApi = if ($ApiMatch.Success) { $ApiMatch.Groups[1].Value } else { "missing" }
$DirtyUi0 = @(git -C $Ui0 status --porcelain)
$DirtyFoundation = @(git -C $Foundation status --porcelain)

Write-Host "UI0 required/current: $RequiredCommit / $CurrentCommit"
Write-Host "UI0 API required/current: $RequiredApi / $CurrentApi"
Write-Host "UI0 version required/current: $RequiredVersion / $CurrentVersion"
Write-Host "source closure foundation required/current: $RequiredFoundationCommit / $CurrentFoundationCommit"

$ExitCode = 0
if ($RequiredCommit -ne $CurrentCommit) { $ExitCode = 2 }
if ($RequiredApi -ne $CurrentApi -and $ExitCode -eq 0) { $ExitCode = 3 }
if ($RequiredVersion -ne $CurrentVersion -and $ExitCode -eq 0) { $ExitCode = 4 }
if ($RequiredFoundationCommit -ne $CurrentFoundationCommit -and $ExitCode -eq 0) { $ExitCode = 5 }
if ($RequiredFoundationVersion -ne $CurrentFoundationVersion -and $ExitCode -eq 0) { $ExitCode = 6 }
if (($DirtyUi0.Count -gt 0 -or $DirtyFoundation.Count -gt 0) -and $ExitCode -eq 0) { $ExitCode = 7 }

if ($ExitCode -eq 0) {
  Write-Host "readerview0 dependency status: current and clean"
  exit 0
}
if ($DirtyUi0.Count -gt 0) { Write-Host "UI0 is dirty"; $DirtyUi0 | Select-Object -First 40 }
if ($DirtyFoundation.Count -gt 0) { Write-Host "zero_foundation is dirty"; $DirtyFoundation | Select-Object -First 40 }
Write-Host "readerview0 dependency status: stale, incompatible, or dirty"
if ($FailWhenStale) { exit $ExitCode }
exit 0

