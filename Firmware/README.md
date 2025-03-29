## FULLMONI-WIDE：Firmware
Renesas e2Studio + GNU GCC-RX + SEGGER emWIN<br>
順次更新予定

## CAN data frame setting
head frame address : 1000(dec) , 0x3E8(hex)
|CAN Address|frame no| byte |assign|multiplier(LSB)|
|---|---|---|---|---|
|0x38E|1|0|RPM High byte|1|
|0x38E|1|1|RPM Lo byte|1|
|0x38E|1|2|Throttle Positon H|?|
|0x38E|1|3|Throttle Positon L|?|
|0x38E|1|4|Manifold pressure H|0.1|
|0x38E|1|5|Manifold pressure L|0.1|
|0x38E|1|6|Inlet Air Temp H|0.1|
|0x38E|1|7|Inlet Air Temp L|0.1|
||||
