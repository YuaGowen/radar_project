#include "bootrom_func.h"

typedef u32 (*pfunc_crc32)(const u8 *p, size_t len);

#if defined(CHIP_AT2410_U02)
#define CRC32_ADDR (0x1d0010a1U) /* at2410s boot v1.9 */
#elif defined(CHIP_AT6010_U01)
#define CRC32_ADDR (0x1d001085U) /* at6010 boot v1.9 */
#else
#error "Please select a chip!"
#endif

u32 bootrom_crc32(const u8 *p, size_t len)
{
	return ((pfunc_crc32)CRC32_ADDR)(p, len);
}
