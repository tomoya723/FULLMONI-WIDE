/*
 * Reset Program for Bootloader
 * RX72N FULLMONI-WIDE
 */

extern void bootloader_main(void);

/* リセットハンドラ */
void PowerON_Reset(void) __attribute__((noreturn));

/* 固定ベクタテーブル（リセットベクタ） */
const void *fvectors[] __attribute__((section(".fvectors"))) = {
    (void *)PowerON_Reset  /* 0xFFFFFFFC: RESET */
};

void PowerON_Reset(void)
{
    /* メイン関数へ */
    bootloader_main();
    
    /* ここには到達しない */
    while (1);
}
