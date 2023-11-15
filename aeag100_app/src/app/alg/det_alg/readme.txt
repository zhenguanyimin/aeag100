反制枪无人机型识别算法代码修改说明
2023.2.2

droneSniffer.c
1. 修改图传特征库参数；

downLoadSniffer.c
2. 修正threshold函数（注释时间20230202）中检测脉宽的计算；
3. 修改uavRowDetect函数（注释时间20230202）中Delt参数，改善图传检测分裂。

