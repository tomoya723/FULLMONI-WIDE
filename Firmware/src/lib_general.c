
/*
 * lib_general.c
 * general purpose library
 *  Created on: 2025/03/25
 *      Author: hiroyuki
 */
#include "lib_general.h"

void smooth_fl(float *value_sm, float sm_ratio, float value_current)
{
    float smooth_cal;
    smooth_cal = *value_sm + (value_current- *value_sm) * sm_ratio;
    *value_sm = smooth_cal;
}

void guard_fl(float *value, float hi_value, float lo_value)
{
    float guarded_value;

    guarded_value = *value;

    if( *value < lo_value){
        guarded_value = lo_value;
    }
    else if( *value > hi_value){
        guarded_value = hi_value;
    }
    else{
        // nothing to do
    }

    *value = guarded_value;

}
