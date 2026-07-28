# readerview0

`readerview0` is an experimental C package for reusable reader application
chrome and interaction.

It composes shared controls and layout from UI0 while leaving document parsing,
pagination, persistence, rendering, accessibility adapters, and native
application hosting to its consumers. It is currently used by
[8vo](https://github.com/devze-ro/8vo) and re10.

Readerview0 is under active development and is part of the same coding-agent
experiment as 8vo. Its APIs are not stable and there is no packaged release.

## Build and test

The package source-consumes the exact UI0 and Ground0 revisions recorded under
`vendor/ui0_dependency/`.

```powershell
git clone https://github.com/devze-ro/readerview0.git
git clone https://github.com/devze-ro/ui0.git
git clone https://github.com/devze-ro/ground0.git
$ui0Commit = Get-Content .\readerview0\vendor\ui0_dependency\COMMIT
$ground0Commit = Get-Content .\readerview0\vendor\ui0_dependency\SOURCE_CLOSURE_GROUND0_COMMIT
git -C .\ui0 checkout $ui0Commit.Trim()
git -C .\ground0 checkout $ground0Commit.Trim()
Set-Location .\readerview0
build\win32_build.bat
```

Building requires PowerShell 5.1 or newer and Visual Studio 2022 or Build Tools
2022 with the C++ workload and a Windows SDK. The build runs the strict
dependency guard, architecture audit, and package tests.

## Documentation

- [Architecture and ownership](docs/architecture.md)
- [Historical engineering records](docs/slices/README.md)

## License

First-party source code and documentation are licensed under the
[Mozilla Public License 2.0](LICENSE). Source-consumed dependencies and
third-party materials retain their original licenses.
