## FULLMONI-WIDE：Firmware
Renesas e2Studio + GNU GCC-RX + SEGGER emWIN<br>
順次更新予定
***
## CAN data frame setting
frame No1 address : 1000(dec) , 0x3E8(hex)
|CAN Address|frame no| byte |assign|unit|multiplier(LSB)|
|---|---|---|---|---|---|
|0x3E8|1|0|RPM High byte|[rpm]|1|
|0x3E8|1|1|RPM Lo byte||1|
|0x3E8|1|2|Throttle Positon H|[%]|0.1|
|0x3E8|1|3|Throttle Positon L|[%]|0.1|
|0x3E8|1|4|Manifold pressure H|[kPa]|0.1|
|0x3E8|1|5|Manifold pressure L|[kPa]|0.1|
|0x3E8|1|6|Inlet Air Temp H|[degC]|0.1|
|0x3E8|1|7|Inlet Air Temp L|[degC]|0.1|

frame No2 address : 1001(dec) , 0x3E9(hex)
|CAN Address|frame no| byte |assign|unit|multiplier(LSB)|
|---|---|---|---|---|---|
|0x3E9|2|0|Engine Temp H|[degC]|0.1|
|0x3E9|2|1|Engine Temp L|[degC]|0.1|
|0x3E9|2|2|Lambda1 H|[-]|0.0147(to PetrolAFR)|
|0x3E9|2|3|Lambda1 L|[-]|0.0147|
|0x3E9|2|4|Lambda2 H|[-]|0.0147|
|0x3E9|2|5|Lambda2 L|[-]|0.0147|
|0x3E9|2|6|Exhaust MAP H|[kPa]|0.1|
|0x3E9|2|7|Exhaust MAP L|[kPa]|0.1|


frame No3 address : 1002(dec) , 0x3EA(hex)
|CAN Address|frame no| byte |assign|unit|multiplier(LSB)|
|---|---|---|---|---|---|
|0x3EA|3|0|Mass Air Flow H|[g/s]|0.1?|
|0x3EA|3|1|Mass Air Flow L|[g/s]|0.1?|
|0x3EA|3|2|Fuel Temp H|[degC]|0.1|
|0x3EA|3|3|Fuel Temp L|[degC]|0.1|
|0x3EA|3|4|Fuel Pressure H|[kPa]|0.1|
|0x3EA|3|5|Fuel Pressure L|[kPa]|0.1|
|0x3EA|3|6|Oil Temp H|[degC]|0.1|
|0x3EA|3|7|Oil Temp L|[degC]|0.1|

frame No4 address : 1003(dec) , 0x3EB(hex)
|CAN Address|frame no| byte |assign|unit|multiplier(LSB)|
|---|---|---|---|---|---|
|0x3EB|4|0|Oil Pressure H|[kPa]|0.1|
|0x3EB|4|1|Oil Pressure L|[kPa]|0.1|
|0x3EB|4|2|Exh Gas Temp1 H|[degC]|0.1|
|0x3EB|4|3|Exh Gas Temp1 L|[degC]|0.1|
|0x3EB|4|4|Exh Gas Temp2 H|[degC]|0.1|
|0x3EB|4|5|Exh Gas Temp2 L|[degC]|0.1|
|0x3EB|4|6|Battery Voltage H|[V]|0.01|
|0x3EB|4|7|Battery Voltage L|[V]|0.01|

frame No5 address : 1004(dec) , 0x3EC(hex)
|CAN Address|frame no| byte |assign|unit|multiplier(LSB)|
|---|---|---|---|---|---|
|0x3EC|5|0|ECU internal Temp H|[degC]|0.1|
|0x3EC|5|1|ECU internal Temp L|[degC]|0.1|
|0x3EC|5|2|Ground Speed H|[km/h]|1?|
|0x3EC|5|3|Ground Speed L|[km/h]|1?|
|0x3EC|5|4|La1 Aim value H|[-]|0.0147(PetrolAFR)|
|0x3EC|5|5|La1 Aim value L|[-]|0.0147|
|0x3EC|5|6|La2 Aim value H|[-]|0.0147|
|0x3EC|5|7|La2 Aim value L|[-]|0.0147|

frame No6 address : 1005(dec) , 0x3ED(hex)
|CAN Address|frame no| byte |assign|unit|multiplier(LSB)|
|---|---|---|---|---|---|
|0x3ED|6|0|Ignition advance H|[degBTDC]|0.1|
|0x3ED|6|1|Ignition advance L|[degBTDC]|0.1|
|0x3ED|6|2|Fuel Inj Duty Cycle H|[%]|0.1|
|0x3ED|6|3|Fuel Inj Duty Cycle L|[%]|0.1|
|0x3ED|6|4|Gear H|[-]|1|
|0x3ED|6|5|Gear L|[-]|1|
|0x3ED|6|6|NoData||
|0x3ED|6|7|NoData||
***
## emWin AppWizard resource swith setting
The project provides multiple screen resources, so select the one you want to use.
Switch display resources by changing Eclipse source path settings and resource includes.
![alt text](image.png)
In the build path source folder settings, set resource paths such as "aw001", "aw002", etc. Set both resources and sources.
<br>
Change the contents of "src/settings.h"
![alt text](image-1.png)
Set the "DISP" number appropriately.