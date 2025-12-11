@echo off
 
echo =============================================================
echo =============================================================

xcopy "ConfigTinno\SystemConfig.ini" "..\..\..\..\..\..\Bin\Setting\" /S /E /Y /I /Q

xcopy "ConfigTinno\MES.InterfaceOpen.dll" "..\..\..\..\..\..\Bin\" /S /E /Y /I /Q
xcopy "ConfigTinno\TINNO_MES_Interface.dll" "..\..\..\..\..\..\Bin\" /S /E /Y /I /Q

xcopy "ConfigTinno\MES.ini" "..\..\..\..\..\..\Bin\Config\" /S /E /Y /I /Q
xcopy "ConfigTinno\TinnoMes.ini" "..\..\..\..\..\..\Bin\Config\" /S /E /Y /I /Q
xcopy "ConfigTinno\TinnoMesLogin.exe" "..\..\..\..\..\..\Bin\Config\" /S /E /Y /I /Q
xcopy "ConfigTinno\TinnoMesMsg.dat" "..\..\..\..\..\..\Bin\Config\" /S /E /Y /I /Q
pause


