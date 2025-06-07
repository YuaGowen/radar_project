#ifndef __BOOTROM_FUNC_H__
#define __BOOTROM_FUNC_H__

#include "at_types.h"

u32 bootrom_crc32(const u8 *p, size_t len);

#endif
