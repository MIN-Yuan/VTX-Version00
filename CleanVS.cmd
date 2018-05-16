@echo ==================================
@echo SET VisualStudio PATH
@echo ==================================
call "%VS110COMNTOOLS%\vsvars32.bat

@echo ==================================
@echo Clean VisualStudio Solution
@echo ==================================
devenv VTX.sln /Clean Debug
devenv VTX.sln /Clean Release
del *.sdf
rmdir ipch /s /q

PAUSE