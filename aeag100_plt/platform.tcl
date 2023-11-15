# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct D:\Code\aeag100\aeag100_plt\platform.tcl
# 
# OR launch xsct and run below command.
# source D:\Code\aeag100\aeag100_plt\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {aeag100_plt}\
-hw {D:\Code\aeag100\hw_spec\aeag_top.xsa}\
-proc {ps7_cortexa9_0} -os {freertos10_xilinx} -out {D:/Code/aeag100}

platform write
platform generate -domains 
bsp reload
platform active {aeag100_plt}
bsp reload
bsp reload
bsp setlib -name lwip211 -ver 1.5
bsp config api_mode "SOCKET_API"
bsp config lwip_tcp_keepalive "true"
bsp config no_sys_no_timers "false"
bsp config socket_mode_thread_prio "5"
bsp config use_axieth_on_zynq "1"
bsp config use_emaclite_on_zynq "1"
bsp config tick_rate "1000"
bsp write
bsp reload
catch {bsp regenerate}
domain active {zynq_fsbl}
bsp reload
bsp write
platform generate
domain active {freertos10_xilinx_domain}
bsp reload
bsp config total_heap_size "65536"
bsp config total_heap_size "131072"
bsp write
bsp reload
catch {bsp regenerate}
platform clean
platform generate
bsp reload
bsp write
platform clean
platform generate
platform clean
platform clean
platform active {aeag100_plt}
domain active {zynq_fsbl}
bsp reload
bsp reload
domain active {freertos10_xilinx_domain}
bsp write
domain active {zynq_fsbl}
bsp write
platform clean
platform generate
domain active {freertos10_xilinx_domain}
domain config -qemu-data {D:\Xilinx\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu}
platform write
domain config -qemu-args {D:\Xilinx\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu\qemu_args.txt}
platform write
platform generate -domains 
platform clean
platform clean
platform generate
platform clean
platform clean
platform generate
platform clean
platform active {aeag100_plt}
domain active {freertos10_xilinx_domain}
domain config -qemu-data {D:\Xilinx_FPGA_IDE\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu}
platform write
domain config -qemu-args {D:\Xilinx_FPGA_IDE\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu\qemu_args.txt}
platform write
platform generate
platform clean
platform generate
platform clean
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform config -updatehw {F:/Project_mission/Gun_System/9-19_Git_dev_embed/cfg_20221007_1630/cfg_20221007_1630/aeag_top_1007.xsa}
platform generate -domains 
platform clean
platform generate
platform config -updatehw {F:/Project_mission/Gun_System/From_DuGong/cfg_20221005_1630/cfg_20221005_1630/aeag_top_1005.xsa}
platform clean
platform generate
platform clean
platform generate
platform config -updatehw {E:/work/aeag100/hw_spec/aeag_top_1007.xsa}
platform config -updatehw {E:/work/aeag100/hw_spec/aeag_top_1007.xsa}
platform active {aeag100_plt}
domain config -qemu-data {D:\Xilinx_FPGA_IDE\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu}
platform write
domain config -qemu-args {D:\Xilinx_FPGA_IDE\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu\qemu_args.txt}
platform write
platform generate
platform active {aeag100_plt}
domain config -qemu-data {D:\Xilinx_FPGA_IDE\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu}
platform write
domain config -qemu-args {D:\Xilinx_FPGA_IDE\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu\qemu_args.txt}
platform write
platform generate
platform generate -domains freertos10_xilinx_domain 
platform active {aeag100_plt}
domain config -qemu-data {C:\Xilinx\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu}
platform write
domain config -qemu-args {C:\Xilinx\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu\qemu_args.txt}
platform write
platform config -updatehw {E:/work/aeag100/aeag100_1025_1359_BB2_product_guhua/hw_spec/cfg_20221021_0838_newbb_t2/cfg_20221021_0838.xsa}
platform generate
platform config -updatehw {E:/work/aeag100/aeag100_1025_1359_BB2_product_guhua/hw_spec/cfg_20221021_0838_newbb_t2/cfg_20221021_0838.xsa}
platform generate -domains 
platform config -updatehw {E:/work/aeag100/aeag100_1025_1359_BB2_product_guhua/hw_spec/cfg_20221018_2000/aeag_top_1014_0140.xsa}
platform generate -domains 
platform active {aeag100_plt}
<<<<<<< HEAD
domain config -qemu-data {C:\Xilinx\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu}
platform write
domain config -qemu-args {C:\Xilinx\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu\qemu_args.txt}
platform write
platform generate
platform active {aeag100_plt}
platform generate -domains 
platform clean
platform generate
=======
domain config -qemu-data {D:\Xilinx_FPGA_IDE\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu}
platform write
domain config -qemu-args {D:\Xilinx_FPGA_IDE\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu\qemu_args.txt}
platform write
platform generate
platform clean
platform clean
platform generate
platform clean
platform generate
platform active {aeag100_plt}
domain config -qemu-data {E:\software\vivado2021\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu}
platform write
domain config -qemu-args {E:\software\vivado2021\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu\qemu_args.txt}
platform write
platform generate
platform config -updatehw {E:/workspace/L000/ps/xsa/aeag_top_i2c_uart.xsa}
platform generate -domains 
platform active {aeag100_plt}
domain config -qemu-data {D:\Xilinx_FPGA_IDE\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu}
platform write
domain config -qemu-args {D:\Xilinx_FPGA_IDE\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu\qemu_args.txt}
platform write
platform generate -domains 
platform clean
platform generate
platform generate -domains freertos10_xilinx_domain,zynq_fsbl 
platform generate
platform generate
platform active {aeag100_plt}
domain config -qemu-data {E:\software\vivado2021\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu}
platform write
domain config -qemu-args {E:\software\vivado2021\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu\qemu_args.txt}
platform write
platform config -updatehw {E:/workspace/L000/ps/xsa/aeag_top_i2c_uart.xsa}
platform generate
platform config -updatehw {E:/workspace/L000/ps/xsa/cfg_20221101_1035/aeag_top_1101.xsa}
platform generate -domains 
platform active {aeag100_plt}
domain config -qemu-data {E:\software\vivado2021\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu}
platform write
platform generate -domains 
platform active {aeag100_plt}
domain config -qemu-data {D:\Xilinx_FPGA_IDE\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu}
platform write
domain config -qemu-args {D:\Xilinx_FPGA_IDE\Vitis\2021.1\data\emulation\platforms\zynq\sw\a9_standalone\qemu\qemu_args.txt}
platform write
platform generate -domains 
platform clean
platform generate
platform clean
platform generate
platform generate
platform generate
platform generate
platform active {aeag100_plt}
platform active {aeag100_plt}
platform config -updatehw {F:/Project_mission/Detection_algorithm/cfg_20221102_mnlog/aeag_top_1102_mnlog.xsa}
platform generate -domains 
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
bsp reload
bsp config total_heap_size "131072"
bsp config total_heap_size "10485760"
bsp write
bsp reload
catch {bsp regenerate}
platform generate -domains freertos10_xilinx_domain 
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform active {aeag100_plt}
platform config -updatehw {F:/Project_mission/Detection_algorithm/code_1/pro_4_BB2_T2/cfg_20221102_mnlog/cfg_20221108_mnlog/aeag_top_20221108_mnlog.xsa}
platform generate -domains 
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform generate -domains 
platform clean
platform generate
platform clean
platform generate
platform active {aeag100_plt}
platform config -updatehw {F:/Project_mission/Detection_algorithm/code_1/pro_4_BB2_T2/cfg_20221102_mnlog/cfg_20221111_mnlog/aeag_top_20221111_mnlog.xsa}
platform generate -domains 
platform clean
platform generate
platform clean
platform generate
platform active {aeag100_plt}
platform generate
platform active {aeag100_plt}
platform config -updatehw {F:/Project_mission/Detection_algorithm/code_1/pro_4_BB2_T2/cfg_20221102_mnlog/cfg_20221111_mnlog/aeag_top_20221111_mnlog.xsa}
platform generate
platform clean
platform generate
platform generate
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform clean
platform generate
platform generate
platform clean
platform generate
platform clean
platform generate
platform active {aeag100_plt}
platform generate -domains freertos10_xilinx_domain,zynq_fsbl 
platform clean
platform generate
platform generate -domains freertos10_xilinx_domain 
platform generate -domains freertos10_xilinx_domain 
platform generate -domains freertos10_xilinx_domain 
platform generate
platform generate
platform active {aeag100_plt}
platform config -updatehw {F:/Project_mission/Detection_algorithm/code_1/pro_4_BB2_T2/cfg_20221102_mnlog/cfg_20221111_mnlog/aeag_top_20221111_mnlog.xsa}
platform generate -domains 
platform clean
platform generate
platform generate -domains freertos10_xilinx_domain 
platform generate -domains freertos10_xilinx_domain,zynq_fsbl 
>>>>>>> dev_embed_101
