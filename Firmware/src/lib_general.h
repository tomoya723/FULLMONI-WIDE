
/*
 * lib_general.h
 * general purpose library
 *  Created on: 2025/03/25
 *      Author: hiroyuki
 */

//common function

/*usage
 * smooth(smoothed_variable,smoothing_factor, base_value);
 * smoothing_factor : 0.001(Heavy smoothing) to 1(No smoothing)
 * ex. smooth(fl_raw_Ga_sm,fl_VR2_reponse_comp_sm, fl_raw_Ga);
 * */

#define smooth(_value_sm, _sm_ratio, _value_current) smooth_fl( (float*)&_value_sm, _sm_ratio, _value_current);
extern void smooth_fl(float *value_sm, float sm_ratio, float value_current);

/*usage
 * 	guard(variable, HighValue, LoValue);
	ex. guard(fl_THA_comp, 1.20, 0.85);
*/
#define guard(_value, _hi_value, _lo_value) guard_fl( (float*)&_value, _hi_value, _lo_value);
extern void guard_fl(float *value, float hi_value, float lo_value);
