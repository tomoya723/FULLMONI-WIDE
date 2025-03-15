/*
 * lib_table.h
 *
 *  Created on: 2025/03/14
 *      Author: hiroyuki
 */


//-usage
//-define table data as below

//const float THA_axis_fl[19] = {
//      0.1272, 0.1584, 0.1994, 0.2514, 0.3216, 0.4101, 0.5307, 0.6907, 0.8987, 1.1733, 1.5083, 1.9279, 2.3946, 2.9115, 3.4215, 3.8776, 4.2520, 4.5283, 4.7169,
//};
//const float THA_value_fl[19] = {
//      140, 130, 120, 110, 100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 0, -10, -20, -30, -40,
//};
//const table2D THA_V_to_DEG_fl_fl = {19, THA_value_fl, THA_axis_fl};
//
//-extern const in .h file
//extern const table2D THA_V_to_DEG_fl_fl;
//
//-call lookuptable functon as below
//lookuped_value = table2D_getValue(&THA_V_to_DEG_fl_fl, fl_THA_V);
//


#include <stdint.h>

typedef struct st_table2D{
    uint8_t xSize;

    void *values;
    void *axisX;
}table2D;

extern float table2D_getAxisValue(table2D *fromTable, uint8_t X_in);
extern float table2D_getRawValue(table2D *fromTable, uint8_t X_index);

extern float table2D_getValue(table2D *fromTable, float X_in);


