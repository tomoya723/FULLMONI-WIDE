/*
 * firmware_version.h
 *
 * ファームウェアバージョン定義
 *
 *  Created on: 2026/01/07
 *      Author: FULLMONI-WIDE Project
 */

#ifndef FIRMWARE_VERSION_H_
#define FIRMWARE_VERSION_H_

/* バージョン定義 (ビルド時にカスタマイズ可能) */
#ifndef FW_VERSION_MAJOR
#define FW_VERSION_MAJOR    1
#endif

#ifndef FW_VERSION_MINOR
#define FW_VERSION_MINOR    0
#endif

#ifndef FW_VERSION_PATCH
#define FW_VERSION_PATCH    0
#endif

/* バージョン文字列マクロ */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define FW_VERSION_STRING TOSTRING(FW_VERSION_MAJOR) "." TOSTRING(FW_VERSION_MINOR) "." TOSTRING(FW_VERSION_PATCH)

#endif /* FIRMWARE_VERSION_H_ */
