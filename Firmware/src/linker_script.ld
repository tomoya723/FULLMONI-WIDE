MEMORY
{
	RAM : ORIGIN = 0x00064040, LENGTH = 0x1BFC0
	RAM2 : ORIGIN = 0x00864000, LENGTH = 0x1C000 /* 高位RAM未使用領域 (112KB) - Issue #65 CAN設定用 */
	ECCRAM : ORIGIN = 0x00FF8000, LENGTH = 0x8000
	FW_HEADER : ORIGIN = 0xFFC20000, LENGTH = 64
	ROM : ORIGIN = 0xFFC20040, LENGTH = 0x3DFFC0 /* 全ROM領域�E�論理皁E��使用�E�E*/
	STARTUP_IMAGE : ORIGIN = 0xFFE20000, LENGTH = 0x60000 /* 起動画像用固定領域 (384KB, acmtc配置用) */
	OFS : ORIGIN = 0xFE7F5D00, LENGTH = 128
}
SECTIONS
{
	/* ファームウェアヘッダ (アプリ領域先頭 0xFFC20000) */
	.firmware_header 0xFFC20000 : AT(0xFFC20000)
	{
		KEEP(*(.firmware_header))
	} >FW_HEADER
	.exvectors 0xFFFFFF80 : AT(0xFFFFFF80)
	{
		"_exvectors_start" = .;
		KEEP(*(.exvectors))
		"_exvectors_end" = .;
	} >ROM
	/* .fvectors removed - owned by bootloader at 0xFFFFFFFC */
	.text 0xFFC20040 : AT(0xFFC20040)
	{
		*(.text)
		*(.text.*)
		*(P)
		etext = .;
	} >ROM
	.rvectors ALIGN(4) :
	{
		_rvectors_start = .;
		INCLUDE ../src/smc_gen/r_bsp/mcu/all/linker_script_rvectors.inc
		_rvectors_end = .;
	} >ROM
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
	} >ROM
	.fini :
	{
		KEEP(*(.fini))
	} >ROM
	.got :
	{
		*(.got)
		*(.got.plt)
	} >ROM
	.rodata :
	{
		*(.rodata)
		/* fmw_op0.oのacmtcは.startup_imageセクションに配置するため除夁E*/
		EXCLUDE_FILE(*fmw_op0.o) *(.rodata.*)
		*(C_1)
		*(C_2)
		*(C)
		_erodata = .;
	} >ROM
	/* 起動画像用セクション (固定アドレス 0xFFE00000) */
	/* ファームウェア更新後も画像データを保持するため、別領域に配置 */
	/* fmw_op0.o冁E�Eacfmw_op0配�E(.rodata.acfmw_op0)をここに配置 */
	.startup_image 0xFFE20000 : AT(0xFFE20000)
	{
		_startup_image_start = .;
		*fmw_op0.o(.rodata.*)
		_startup_image_end = .;
	} >STARTUP_IMAGE
	gcc_exceptions_table :
	{
		KEEP(*(.gcc_except_table))
		*(.gcc_except_table.*)
	} >ROM
	.eh_frame_hdr :
	{
		*(.eh_frame_hdr)
	} >ROM
	.eh_frame :
	{
		*(.eh_frame)
	} >ROM
	.jcr :
	{
		*(.jcr)
	} >ROM
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
	} >ROM
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
	/* 高位RAM2: LVGL描画バッファ等を配置 */
	/* NOLOAD: 初期化不要なBSSセクションはMOTファイルに含めなぁE*/
	.bss2 (NOLOAD) :
	{
		_bss2 = .;
		*(.bss2)
		*(.bss2.*)
		_ebss2 = .;
	} >RAM2
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
