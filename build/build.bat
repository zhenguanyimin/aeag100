@echo off
cd ..
SET VERSION=%VERSION:"=%
SET SRC_WORKSPACE=%cd:\=/%
SET VITIS_HOME=%VITIS_HOME:\=/%
SET QEMUDATA=%VITIS_HOME:\=/%/data/emulation/platforms/zynq/sw/a9_standalone/qemu

REM ɾ��releaseĿ¼�������ļ�����������
if exist build\release rd /S /Q build\release
   mkdir build\release

REM ��ȡvitis workspace��
for /f "tokens=1,2 delims==" %%i in (build/conf.properties) do (
	SET VITIS_WORKSPACE=%%j	
)
REM ��ȡ�汾��
for /f "tokens=2,3" %%i in (aeag100_app/src/cfg/aeag_config.h) do (
	if "%%i" == "EMBED_SOFTWARE_PS_VERSION_STR" (
		SET VERSION=%%j
	) 
)

REM ��ȡXSA�ļ���
for /f "delims=:" %%a in ('dir /b hw_spec\*.xsa ^| findstr /n .*') do SET count=%%a
	if %count% equ 1 (
		for /f "delims=" %%i in ('dir /s /b hw_spec\*.xsa') do SET XSA_FILE=%%~nxi
	) else (
		color cf & echo ��⵽���xsa�ļ���ϵͳֻ����һ��xsa�ļ���
		pause
		exit 1
	)

@echo ********����aeag100_app_system.bif�ļ�********
echo //arch = zynq; split = false; format = BIN>build/release/aeag100_app_system.bif
echo the_ROM_image:  >>build/release/aeag100_app_system.bif
echo {>>build/release/aeag100_app_system.bif
echo [bootloader]%SRC_WORKSPACE%/aeag100_plt/zynq_fsbl/fsbl.elf>>build/release/aeag100_app_system.bif
echo %SRC_WORKSPACE%/hw_spec/aeag_top.bit>>build/release/aeag100_app_system.bif
echo %SRC_WORKSPACE%/aeag100_app/Debug/aeag100_app.elf>>build/release/aeag100_app_system.bif
echo }>>build/release/aeag100_app_system.bif

@echo ********�޸�platform.spr********
python build/build_aeag100.py %SRC_WORKSPACE%\aeag100_plt\platform.spr %QEMUDATA% %SRC_WORKSPACE%/hw_spec/%XSA_FILE% "<platformDir>/hw"/%XSA_FILE%

@echo ********���ڱ�����,���Եȼ�����********
call xsct build/aeag100.tcl %VITIS_WORKSPACE% %SRC_WORKSPACE%/hw_spec/%XSA_FILE%

if not exist aeag100_app/Debug/aeag100_app.elf (
   color cf & echo ********������������鿴����********
   pause
   exit 1
)else (
	call build/build_aeag100.bat 
)
 
if not exist build/release/*.bin (
   echo ********BIN�ļ�����ʧ��********
   exit 1
)else (
   @echo ******** BIN�ļ����ɳɹ� *******
   @echo ********��������********
   cp flash_prog/fsbl_autel.elf build/release/fsbl_autel.elf
   cp hw_spec/aeag_top.bit build/release/aeag_top.bit
   cp aeag100_app/Debug/aeag100_app.elf build/release/aeag100_app.elf
   cp aeag100_plt/zynq_fsbl/fsbl.elf  build/release/fsbl.elf
)

@echo *******��ԭplatform.spr�ļ�*******
if exist %cd%\aeag100_plt\platform.spr_bak (
   del /q %cd%\aeag100_plt\platform.spr
   ren %cd%\aeag100_plt\platform.spr_bak platform.spr
)
pause

