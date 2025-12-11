@echo off
 
echo =============================================================
echo =============================================================

xcopy "Lib\CallHiMesDll.dll" "..\..\..\..\..\..\Bin\App\SpatLib\" /S /E /Y /I /Q
xcopy "Lib\Config.ini" "..\..\..\..\..\..\Bin\Customized\Hisense\" /S /E /Y /I /Q
xcopy "Lib\IntelligentPrecise.PhoneTest.Interface.dll" "..\..\..\..\..\..\Bin\Customized\Hisense\" /S /E /Y /I /Q
xcopy "Lib\IntelligentPrecise.DBManager.dll" "..\..\..\..\..\..\Bin\Customized\Hisense\" /S /E /Y /I /Q
xcopy "Lib\IntelligentPrecise.Loger.dll" "..\..\..\..\..\..\Bin\Customized\Hisense\" /S /E /Y /I /Q
xcopy "Lib\log4net.dll" "..\..\..\..\..\..\Bin\Customized\Hisense\" /S /E /Y /I /Q
pause


