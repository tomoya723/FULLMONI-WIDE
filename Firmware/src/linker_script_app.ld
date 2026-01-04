/*
 * Bootloader対応アプリケーション用リンカスクリプト
 * RX72N/RX72T (R5F572NNDxBD) - Renesas RXマイコン
 *
 * メモリマップ:
 *   FW_HEADER: 0xFFC10000 - 0xFFC1013F (320B) - ファームウェアヘッダ
 *   APP_ROM:   0xFFC20000 - 0xFFFFFFFF (3.875MB) - アプリケーションコード
 *   RAM:       0x00064040 - 0x0007FFFF (111.9KB) - Frame Buffer 2の後から
 *   ECCRAM:    0x00FF8000 - 0x00FFFFFF (32KB) - スタック
 *
 * 注意:
 *   - Bootloader領域 (0xFFC00000-0xFFC0FFFF) は使用禁止
 *   - Frame Buffer 1: 0x00800000-0x00863FFF (Block 2)
 *   - Frame Buffer 2: 0x00000040-0x0006403F (Block 1)
 */

MEMORY
{
	/* ファームウェアヘッダ領域 (320バイト) */
	FW_HEADER : ORIGIN = 0xFFC10000, LENGTH = 320

	/* アプリケーションROM (Bootloaderとヘッダの後) */
	APP_ROM : ORIGIN = 0xFFC20000, LENGTH = 0x003E0000

	/* RAM (Frame Buffer 2の後から使用) */
	RAM : ORIGIN = 0x00064040, LENGTH = 0x1BFC0

	/* ECC RAM (スタック用) */
	ECCRAM : ORIGIN = 0x00FF8000, LENGTH = 0x8000

	/* オプション設定領域 */
	OFS : ORIGIN = 0xFE7F5D00, LENGTH = 128
}

SECTIONS
{
	/* ファームウェアヘッダ (0xFFC10000) */
	.firmware_header 0xFFC10000 : AT(0xFFC10000)
	{
		KEEP(*(.firmware_header))
	} >FW_HEADER

	/* 拡張ベクタテーブル */
	.exvectors 0xFFFFFF80 : AT(0xFFFFFF80)
	{
		_exvectors_start = .;
		KEEP(*(.exvectors))
		_exvectors_end = .;
	} >APP_ROM

	/* 固定ベクタテーブル - Bootloader使用時はコメントアウト */
	/* .fvectors 0xFFFFFFFC : AT(0xFFFFFFFC)
	{
		KEEP(*(.fvectors))
	} >APP_ROM */

	/* プログラムコード (0xFFC20000から開始) */
	.text 0xFFC20000 : AT(0xFFC20000)
	{
		*(.text)
		*(.text.*)
		*(P)
		etext = .;
	} >APP_ROM

	/* 可変ベクタテーブル */
	.rvectors ALIGN(4) :
	{
		_rvectors_start = .;
		INCLUDE ../src/smc_gen/r_bsp/mcu/all/linker_script_rvectors.inc
		_rvectors_end = .;
	} >APP_ROM

	/* 初期化/終了処理 */
	.init :
	{
		KEEP(*(.init))
		__preinit_array_start = .;
		KEEP(*(.preinit_array))
		__preinit_array_end = .;
		__init_array_start = (. + 3) & ~ 3;
		KEEP(*(.init_array))
		KEEP(*(SORT(.init_array.*)))
		__init_array_end = .;
		__fini_array_start = .;
		KEEP(*(.fini_array))
		KEEP(*(SORT(.fini_array.*)))
		__fini_array_end = .;
	} >APP_ROM

	.fini :
	{
		KEEP(*(.fini))
	} >APP_ROM

	.got :
	{
		*(.got)
		*(.got.plt)
	} >APP_ROM

	/* 読み込み専用データ */
	.rodata :
	{
		*(.rodata)
		*(.rodata.*)
		*(C_1)
		*(C_2)
		*(C)
		_erodata = .;
	} >APP_ROM

	/* C++例外処理テーブル */
	gcc_exceptions_table :
	{
		KEEP(*(.gcc_except_table))
		*(.gcc_except_table.*)
	} >APP_ROM

	.eh_frame_hdr :
	{
		*(.eh_frame_hdr)
	} >APP_ROM

	.eh_frame :
	{
		*(.eh_frame)
	} >APP_ROM

	.jcr :
	{
		*(.jcr)
	} >APP_ROM

	/* コンストラクタ/デストラクタ */
	.tors :
	{
		__CTOR_LIST__ = .;
		. = ALIGN(2);
		___ctors = .;
		*(.ctors)
		___ctors_end = .;
		__CTOR_END__ = .;
		__DTOR_LIST__ = .;
		___dtors = .;
		*(.dtors)
		___dtors_end = .;
		__DTOR_END__ = .;
		. = ALIGN(2);
		_mdata = .;
	} >APP_ROM

	/* 初期化データ (ROMからECCRAMへコピー) */
	.data : AT(_mdata)
	{
		_data = .;
		*(.data)
		*(.data.*)
		*(D)
		*(D_1)
		*(D_2)
		_edata = .;
	} >ECCRAM

	/* 未初期化データ (RAMに配置) */
	.bss :
	{
		_bss = .;
		*(.bss)
		*(.bss.**)
		*(COMMON)
		*(B)
		*(B_1)
		*(B_2)
		_ebss = .;
		. = ALIGN(128);
		_end = .;
	} >RAM AT>RAM

	/* オプション設定 */
	.ofs1 0xFE7F5D00 : AT(0xFE7F5D00)
	{
		KEEP(*(.ofs1))
	} >OFS

	.ofs2 0xFE7F5D10 : AT(0xFE7F5D10)
	{
		KEEP(*(.ofs2))
	} >OFS

	.ofs3 0xFE7F5D20 : AT(0xFE7F5D20)
	{
		KEEP(*(.ofs3))
	} >OFS

	.ofs4 0xFE7F5D40 : AT(0xFE7F5D40)
	{
		KEEP(*(.ofs4))
	} >OFS

	.ofs5 0xFE7F5D48 : AT(0xFE7F5D48)
	{
		KEEP(*(.ofs5))
	} >OFS

	.ofs6 0xFE7F5D50 : AT(0xFE7F5D50)
	{
		KEEP(*(.ofs6))
	} >OFS

	.ofs7 0xFE7F5D64 : AT(0xFE7F5D64)
	{
		KEEP(*(.ofs7))
	} >OFS

	.ofs8 0xFE7F5D70 : AT(0xFE7F5D70)
	{
		KEEP(*(.ofs8))
	} >OFS

	/* スタック領域 */
	.r_bsp_NULL :
	{
		. += 0x100;
		"_r_bsp_NULL_end" = .;
	} >ECCRAM AT>ECCRAM

	.r_bsp_istack ALIGN(0x4) (NOLOAD) :
	{
		KEEP(*(.r_bsp_istack))
	} >ECCRAM AT>ECCRAM

	.istack :
	{
		"_istack" = .;
	} >ECCRAM

	.r_bsp_ustack ALIGN(0x4) (NOLOAD) :
	{
		KEEP(*(.r_bsp_ustack))
	} >ECCRAM AT>ECCRAM

	.ustack :
	{
		"_ustack" = .;
	} >ECCRAM
}
