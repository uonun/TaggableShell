::Uninstall first
taskkill /f /im Explorer.exe
regsvr32 /s /u bin\TaggableShellEx.Handlers.x64.dll
RD /S /Q bin
Explorer.exe