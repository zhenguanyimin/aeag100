# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: F:\Project_mission\Detection_algorithm\code_1\pro_4_BB2_T2\project_02_F_AD9361_D_scan_1\project_02_F_AD9361_D_scan\aeag100_app_system\_ide\scripts\debugger_aeag100_app-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source F:\Project_mission\Detection_algorithm\code_1\pro_4_BB2_T2\project_02_F_AD9361_D_scan_1\project_02_F_AD9361_D_scan\aeag100_app_system\_ide\scripts\debugger_aeag100_app-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent JTAG-SMT2 210251A08870" && level==0 && jtag_device_ctx=="jsn-JTAG-SMT2-210251A08870-4ba00477-0"}
fpga -file F:/Project_mission/Detection_algorithm/code_1/pro_4_BB2_T2/cfg_20221102_mnlog/cfg_20221111_mnlog/aeag_top_20221111_mnlog.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw F:/Project_mission/Detection_algorithm/code_1/pro_4_BB2_T2/project_02_F_AD9361_D_scan/aeag100_plt/export/aeag100_plt/hw/aeag_top_20221111_mnlog.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source F:/Project_mission/Detection_algorithm/code_1/pro_4_BB2_T2/project_02_F_AD9361_D_scan_1/project_02_F_AD9361_D_scan/aeag100_app/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow F:/Project_mission/Detection_algorithm/code_1/pro_4_BB2_T2/project_02_F_AD9361_D_scan_1/project_02_F_AD9361_D_scan/aeag100_app/Debug/aeag100_app.elf
configparams force-mem-access 0
bpadd -addr &main
