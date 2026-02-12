/*
 * firmware_version.h
 *
 * 繝輔ぃ繝ｼ繝繧ｦ繧ｧ繧｢繝舌・繧ｸ繝ｧ繝ｳ螳夂ｾｩ
 *
 *  Created on: 2026/01/07
 *      Author: FULLMONI-WIDE Project
 */

#ifndef FIRMWARE_VERSION_H_
#define FIRMWARE_VERSION_H_

/* 繝舌・繧ｸ繝ｧ繝ｳ螳夂ｾｩ (繝薙Ν繝画凾縺ｫ繧ｫ繧ｹ繧ｿ繝槭う繧ｺ蜿ｯ閭ｽ) */
#ifndef FW_VERSION_MAJOR
#define FW_VERSION_MAJOR    1
#endif

#ifndef FW_VERSION_MINOR
#define FW_VERSION_MINOR    0
#endif

#ifndef FW_VERSION_PATCH
#define FW_VERSION_PATCH    1
#endif

/* 繝舌・繧ｸ繝ｧ繝ｳ譁・ｭ怜・繝槭け繝ｭ */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define FW_VERSION_STRING TOSTRING(FW_VERSION_MAJOR) "." TOSTRING(FW_VERSION_MINOR) "." TOSTRING(FW_VERSION_PATCH)

#endif /* FIRMWARE_VERSION_H_ */
