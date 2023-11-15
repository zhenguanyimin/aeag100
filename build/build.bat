@echo off
cd ..
SET VERSION=%VERSION:"=%
SET SRC_WORKSPACE=%cd:\=/%
SET VITIS_HOME=%VITIS_HOME:\=/%
SET QEMUDATA=%VITIS_HOME:\=/%/data/emulation/platforms/zynq/sw/a9_standalone/qemu

REM 删除release目录下所有文件并重新生成
if exist build\release rd /S /Q build\release
   mkdir build\release

REM 获取vitis workspace号
for /f "tokens=1,2 delims==" %%i in (build/conf.properties) do (
	SET VITIS_WORKSPACE=%%j	
)
REM 获取版本号
for /f "tokens=2,3" %%i in (aeag100_app/src/cfg/aeag_config.h) do (
	if "%%i" == "EMBED_SOFTWARE_PS_VERSION_STR" (
		SET VERSION=%%j
	) 
)

REM 获取XSA文件名
for /f "delims=:" %%a in ('dir /b hw_spec\*.xsa ^| findstr /n .*') do SET count=%%a
	if %count% equ 1 (
		for /f "delims=" %%i in ('dir /s /b hw_spec\*.xsa') do SET XSA_FILE=%%~nxi
	) else (
		color cf & echo 检测到多个xsa文件，系统只允许一个xsa文件。
		pause
		exit 1
	)

@echo ********创建aeag100_app_system.bif文件********
echo //arch = zynq; split = false; format = BIN>build/release/aeag100_app_system.bif
echo the_ROM_image:  >>build/release/aeag100_app_system.bif
echo {>>build/release/aeag100_app_system.bif
echo [bootloader]%SRC_WORKSPACE%/aeag100_plt/zynq_fsbl/fsbl.elf>>build/release/aeag100_app_system.bif
echo %SRC_WORKSPACE%/hw_spec/aeag_top.bit>>build/release/aeag100_app_system.bif
echo %SRC_WORKSPACE%/aeag100_app/Debug/aeag100_app.elf>>build/release/aeag100_app_system.bif
echo }>>build/release/aeag100_app_system.bif

@echo ********修改platform.spr********
python build/build_aeag100.py %SRC_WORKSPACE%\aeag100_plt\platform.spr %QEMUDATA% %SRC_WORKSPACE%/hw_spec/%XSA_FILE% "<platformDir>/hw"/%XSA_FILE%

@echo ********正在编译中,请稍等几分钟********
call xsct build/aeag100.tcl %VITIS_WORKSPACE% %SRC_WORKSPACE%/hw_spec/%XSA_FILE%

if not exist aeag100_app/Debug/aeag100_app.elf (
   color cf & echo ********程序编译出错，请查看程序********
   pause
   exit 1
)else (
	call build/build_aeag100.bat 
)
 
if not exist build/release/*.bin (
   echo ********BIN文件生成失败********
   exit 1
)else (
   @echo ******** BIN文件生成成功 *******
   @echo ********拷贝物料********
   cp flash_prog/fsbl_autel.elf build/release/fsbl_autel.elf
   cp hw_spec/aeag_top.bit build/release/aeag_top.bit
   cp aeag100_app/Debug/aeag100_app.elf build/release/aeag100_app.elf
   cp aeag100_plt/zynq_fsbl/fsbl.elf  build/release/fsbl.elf
)

@echo *******还原platform.spr文件*******
if exist %cd%\aeag100_plt\platform.spr_bak (
   del /q %cd%\aeag100_plt\platform.spr
   ren %cd%\aeag100_plt\platform.spr_bak platform.spr
)
pause

