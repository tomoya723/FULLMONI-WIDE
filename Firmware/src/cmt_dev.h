/****************************************************************************/
/*                                                                          */
/*      AlphaProject サンプルプログラム                                     */
/*        CMT処理 ヘッダファイル                                            */
/*                                                                          */
/*          Copyright   :: 株式会社アルファプロジェクト                     */
/*          Cpu         :: RXファミリ                                       */
/*          File Name   :: cmt_dev.h                                        */
/*                                                                          */
/****************************************************************************/

#ifndef CMT_DEV_H
#define CMT_DEV_H


#include "platform.h"

/* --------------
 * define
 * -------------- */
#define ERROR_LED_ON   (1)
#define ERROR_LED_OFF  (0)

/* --------------
 * function
 * -------------- */
bool CmtInit(void);
uint32_t GetCmt1msecCounter(void);


#endif /* CMT_DEV_H */
/* End of File */
