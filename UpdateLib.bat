

xcopy	/y		.\Engine\Bin\Engine.lib					.\EngineSDK\Lib\
xcopy	/y		.\Engine\Bin\Engine.dll					.\Client\Bin\
xcopy	/y		.\Engine\Bin\ShaderFiles\*.*			.\Client\Bin\ShaderFiles\
xcopy	/y/s	.\Engine\Public\*.*						.\EngineSDK\Inc\
xcopy	/y		.\Engine\Bin\Engine.dll					.\MapTool\Bin\
xcopy	/y		.\Engine\Bin\ShaderFiles\*.*				.\MapTool\
xcopy	/y		.\Engine\Bin\ShaderFiles\*.*				.\Bin\ShaderFiles\