# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
<<<<<<< HEAD
# Path of this script: E:\work\aeag100\aeag100_1028_1032_BB2_T2_battery_low_refuse_action\aeag100_app_system\_ide\scripts\systemdebugger_aeag100_app_system_standalone.tcl
=======
# Path of this script: F:\Project_mission\Detection_algorithm\code_1\pro_4_BB2_T2\project_02\aeag100_app_system\_ide\scripts\systemdebugger_aeag100_app_system_standalone.tcl
>>>>>>> dev_embed_101
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
<<<<<<< HEAD
# source E:\work\aeag100\aeag100_1028_1032_BB2_T2_battery_low_refuse_action\aeag100_app_system\_ide\scripts\systemdebugger_aeag100_app_system_standalone.tcl
=======
# source F:\Project_mission\Detection_algorithm\code_1\pro_4_BB2_T2\project_02\aeag100_app_system\_ide\scripts\systemdebugger_aeag100_app_system_standalone.tcl
>>>>>>> dev_embed_101
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent JTAG-SMT2 210251A08870" && level==0 && jtag_device_ctx=="jsn-JTAG-SMT2-210251A08870-03736093-0"}
<<<<<<< HEAD
fpga -file E:/work/aeag100/aeag100_1028_1032_BB2_T2_battery_low_refuse_action/aeag_top.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw E:/work/aeag100/aeag100_1028_1032_BB2_T2_battery_low_refuse_action/aeag100_plt/export/aeag100_plt/hw/aeag_top_1014_0140.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source E:/work/aeag100/aeag100_1028_1032_BB2_T2_battery_low_refuse_action/aeag100_app/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow E:/work/aeag100/aeag100_1028_1032_BB2_T2_battery_low_refuse_action/aeag100_app/Debug/aeag100_app.elf
=======
fpga -file F:/Project_mission/Detection_algorithm/code_1/pro_4_BB2_T2/cfg_20221102_mnlog/cfg_20221102_mnlog/aeag_top_1102_mnlog2030.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw F:/Project_mission/Detection_algorithm/code_1/pro_4_BB2_T2/project_02/aeag100_plt/export/aeag100_plt/hw/aeag_top_1102_mnlog.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source F:/Project_mission/Detection_algorithm/code_1/pro_4_BB2_T2/project_02/aeag100_app/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow F:/Project_mission/Detection_algorithm/code_1/pro_4_BB2_T2/project_02/aeag100_app/Debug/aeag100_app.elf
>>>>>>> dev_embed_101
configparams force-mem-access 0
bpadd -addr &main
