$ErrorActionPreference = "Stop"
& (Join-Path $PSScriptRoot "check_ui0_dependency_status.ps1") -FailWhenStale
if ($LASTEXITCODE -ne 0) {
  Write-Host "guardrail: readerview0 dependency pin is not current"
  exit $LASTEXITCODE
}
Write-Host "guardrail: readerview0 UI0 API 89 dependency is current"
exit 0

