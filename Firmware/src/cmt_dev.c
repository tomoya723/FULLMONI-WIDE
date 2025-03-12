/****************************************************************************/
/*                                                                          */
/*      AlphaProject �T���v���v���O����                                     */
/*        CMT����                                                           */
/*                                                                          */
/*          Copyright   :: ������ЃA���t�@�v���W�F�N�g                     */
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
 * > CMT������
 * > input  : none
 * > output : bool  -> true:����  false:���s
 ****************************************************************************/
bool CmtInit(void)
{
	uint32_t Channel;
	bool Ret;

	s_1msecCount = 0u;

	/* �^�C�}������ */
	Ret = R_CMT_CreatePeriodic(CMT_1000HZ, &cmtHandler, &Channel);
	
	return Ret;
}

/****************************************************************************
 * GetCmt1msecCounter()
 * > CMT 1msec�J�E���^�擾
 * > input  : none
 * > output : unsigned long  -> 1msec�J�E���^�l
 ****************************************************************************/
uint32_t GetCmt1msecCounter(void)
{
	return s_1msecCount;
}

/****************************************************************************
 * cmtHandler()
 * > CMT �R�[���o�b�N�֐�
 * > input  : void *  -> ���荞�݂��g���K����CMT�ԍ����܂ޒl�ւ̃|�C���^
 * > output : none
 ****************************************************************************/
static void cmtHandler(void * PtrData)
{
	s_1msecCount++;
}

/* End of File */
