set project_name=simple_transmitter

rmdir /s /q ipch

del /f /q Debug\*.*
del /f /q Release\*.*
del /f /q x64\Debug\*.*
del /f /q x64\Release\*.*

del /f /q %project_name%.sdf
del /f /q %project_name%\Debug\*.*
del /f /q %project_name%\Release\*.*
del /f /q %project_name%\x64\Debug\*.*
del /f /q %project_name%\x64\Release\*.*

del /f /q /s %project_name%.vcxproj.user

ping /n 3 127.0.1 >nul

