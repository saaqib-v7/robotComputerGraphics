param (
    [string]$vcpkg_root = "C:\vcpkg"
)

$vcpkg_githash = "6784dd45fa9a97ce6232f57d755e029b4db3eb81"

# Check if the directory exists
if (Test-Path -Path $vcpkg_root) {
    # Directory exists, delete it recursively
    Remove-Item -Path $vcpkg_root -Recurse -Force
}

# Create the directory again
New-Item -Path $vcpkg_root -ItemType Directory -Force

$vcpkg_root = Resolve-Path -Path $vcpkg_root

Set-Location -Path (Split-Path -Path $vcpkg_root -Parent)

git clone https://github.com/microsoft/vcpkg.git (Split-Path -Path $vcpkg_root -Leaf)
Set-Location -Path $vcpkg_root

git checkout $vcpkg_githash

.\bootstrap-vcpkg.bat

.\vcpkg.exe install SDL2 SDL2-image[libjpeg-turbo] imgui[sdl2-binding,opengl3-binding] glew glm --triplet x64-windows 

# Set the environment variable for the system
# [System.Environment]::SetEnvironmentVariable("VCPKG_ROOT", $vcpkg_root, [System.EnvironmentVariableTarget]::Machine)