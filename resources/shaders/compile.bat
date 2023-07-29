@echo off
setlocal enabledelayedexpansion

set "shader_dir=%~dp0"
set "glslangValidator=D:\VulkanSDK\Bin\glslc.exe"

for %%f in ("*.vert" "*.frag" "*.geom") do (
    echo Compiling: %%~nxf
    "%glslangValidator%" %%f -o %%~dpnf.spv
)

echo All shaders compiled.
pause