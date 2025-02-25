/****************************************************************************/
/*                                                                          */
/*      AlphaProject サンプルプログラム                                     */
/*        CMT処理                                                           */
/*                                                                          */
/*          Copyright   :: 株式会社アルファプロジェクト                     */
/*          Cpu         :: RX71M                                            */
/*          File Name   :: cmt_dev.c                                        */
/*                                                                          */
/****************************************************************************/


#include "r_cmt_rx_if.h"
#include "cmt_dev.h"

/* --------------
 * define
 * -------------- */
#define CMT_1000HZ    (1000u)

/* --------------
 * variable
 * -------------- */
static uint32_t s_1msecCount;


/* --------------
 * function
 * -------------- */
static void cmtHandler(void * PtrData);


/****************************************************************************
 * CmtInit()
 * > CMT初期化
 * > input  : none
 * > output : bool  -> true:成功  false:失敗
 ****************************************************************************/
bool CmtInit(void)
{
	uint32_t Channel;
	bool Ret;

	s_1msecCount = 0u;

	/* タイマ初期化 */
	Ret = R_CMT_CreatePeriodic(CMT_1000HZ, &cmtHandler, &Channel);
	
	return Ret;
}

/****************************************************************************
 * GetCmt1msecCounter()
 * > CMT 1msecカウンタ取得
 * > input  : none
 * > output : unsigned long  -> 1msecカウンタ値
 ****************************************************************************/
uint32_t GetCmt1msecCounter(void)
{
	return s_1msecCount;
}

/****************************************************************************
 * cmtHandler()
 * > CMT コールバック関数
 * > input  : void *  -> 割り込みをトリガしたCMT番号を含む値へのポインタ
 * > output : none
 ****************************************************************************/
static void cmtHandler(void * PtrData)
{
	s_1msecCount++;
}

/* End of File */
