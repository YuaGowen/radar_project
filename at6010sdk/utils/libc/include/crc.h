#ifndef __CRC_H__
#define __CRC_H__

#include "at_types.h"

/**
 * crc32 - get crc32 data from crc32_tbl.
 */
u32 crc32(const u8 *p, size_t len);


#endif
