rmdir /S /Q build
mkdir build

cd build

cmake -G "Visual Studio 16 2019" -DCMAKE_SYSTEM_VERSION=10.0.15063.0 ..
cmake --build . --config Release
cd ..

%VK_SDK_PATH%\Bin\glslangValidator.exe -V src\DLLToInject\GameOverlay\vulkan\src\shader.vert -o python\game_overlay_sdk\lib\vert.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V src\DLLToInject\GameOverlay\vulkan\src\shader.frag -o python\game_overlay_sdk\lib\frag.spv
%VK_SDK_PATH%\Bin\glslangValidator.exe -V src\DLLToInject\GameOverlay\vulkan\src\shader.comp -o python\game_overlay_sdk\lib\comp.spv

echo F | xcopy /Y src\DLLToInject\GameOverlay\vulkan\src\shader.vert python\game_overlay_sdk\lib\shader.vert
echo F | xcopy /Y src\DLLToInject\GameOverlay\vulkan\src\shader.frag python\game_overlay_sdk\lib\shader.frag
echo F | xcopy /Y src\DLLToInject\GameOverlay\vulkan\src\shader.comp python\game_overlay_sdk\lib\shader.comp

echo F | xcopy /Y src\DLLToInject\GameOverlay\vulkan\VK_LAYER_OCAT_overlay64.json python\game_overlay_sdk\lib\VK_LAYER_OCAT_overlay64.json

cmd \k