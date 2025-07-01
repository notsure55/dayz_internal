# if build directory doesnt exist
if (-not (Test-Path build))
{
    New-Item -ItemType Directory -Path build | Out-Null
}

Set-Location build

if (-not (Test-Path CMakeCache.txt))
{
    # build cmake files
    cmake -G "Visual Studio 17 2022" -A x64 ..
}

# build project
cmake --build . --config Release
