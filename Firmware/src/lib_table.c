#include <stdint.h>
#include <float.h>
#include <math.h>
#include "lib_table.h"


float table2D_getValue(table2D *fromTable, float X_in)
{
    float returnValue = 0;
    _Bool valueFound = 0;

    float X = X_in;
    float xMinValue = 0.0 , xMaxValue = 0.0;
    uint8_t xMin = 0;
    uint8_t xMax = fromTable->xSize - 1;

    //If the requested X value is greater/small than the maximum/minimum bin, simply return that value
    if(X >= table2D_getAxisValue(fromTable, xMax))
    {
      returnValue = table2D_getRawValue(fromTable, xMax);
      valueFound = 1;
    }
    else if(X <= table2D_getAxisValue(fromTable, xMin))
    {
      returnValue = table2D_getRawValue(fromTable, xMin);
      valueFound = 1;
    }
    //Finally if none of that is found
    else
    {
        //If we're not in the same bin, loop through to find where we are
        xMaxValue = table2D_getAxisValue(fromTable, fromTable->xSize-1); // init xMaxValue in preparation for loop.
        for (uint8_t x = fromTable->xSize-1; x > 0; x--)
        {
          xMinValue = table2D_getAxisValue(fromTable, x-1); // fetch next Min

          //Checks the case where the X value is exactly what was requested
          if ( fabsf(X - xMaxValue) < FLT_EPSILON )
          {
            returnValue = table2D_getRawValue(fromTable, x); //Simply return the corresponding value
            valueFound = 1;
            break;
          }
          else if (X > xMinValue)
          {
            // Value is in the current bin
            xMax = x;
            xMin = x-1;
            break;
          }
          // Otherwise, continue to next bin
          xMaxValue = xMinValue; // for the next bin, our Min is their Max
      }
    } //X_in same as last time

    if(valueFound == 0)
    {
        float   m = X - xMinValue;
        float  n = xMaxValue - xMinValue;

        float yMax = table2D_getRawValue(fromTable, xMax);
        float yMin = table2D_getRawValue(fromTable, xMin);

        /* interpolating midoint value */
        float yVal = (m * (yMax -yMin)) / n;
        returnValue = yMin + yVal;
    }

    return returnValue;

}

float table2D_getAxisValue(table2D *fromTable,uint8_t X_in)
{
    float return_value = 0;

    return_value = ((float*)fromTable->axisX)[X_in];
    return return_value;
}

float table2D_getRawValue(table2D *fromTable,uint8_t X_index)
{
    float return_value = 0;

    return_value = ((float*)fromTable->values)[X_index];
    return return_value;
}
