#ifndef __MMDEFS_H__
#define __MMDEFS_H__

/* rom code(boot loader) memory map */
#define MM_ROM_START                    0x1D000000
#define MM_ROM_END                      0x1EFFFFFF

/* sram memory map */
#define MM_SRAM_START                   0x20000000
#define MM_SRAM_END                     0x21FFFFFF

/* register memory map */
#define MM_REG_START                    0x04000000
#define MM_REG_END                      0xF0000FFF

/* flash(cache access) memory map */
#define MM_FLASH_CACHE_START            0x08000000
#define MM_FLASH_CACHE_END              0x09FFFFFF

/* flash(direct access) memory map */
#define MM_FLASH_START                  0x0A000000
#define MM_FLASH_END                    0x0BFFFFFF

/* sram(in cache) memory map */
#define MM_CACHE_SRAM_START             0x22000000
#define MM_CACHE_SRAM_END               0x22001FFF

#endif
