#ifndef _ROM_TABLE_H_
#define _ROM_TABLE_H_

#define REG_ROM_TABLE_BASE               0xF0000000

typedef struct
{
    REG32                entry0;              // 0x00000000
    REG32                entry1;              // 0x00000004
    REG32                entry2;              // 0x00000008
    REG32                entry3;              // 0x0000000c
    //REG32                reserved[1008];      // 0x00000010-0x00000fcf
    REG32                pid4;                // 0x00000fd0
    REG32                pid5;                // 0x00000fd4
    REG32                pid6;                // 0x00000fd8
    REG32                pid7;                // 0x00000fdc
    REG32                pid0;                // 0x00000fe0
    REG32                pid1;                // 0x00000fe4
    REG32                pid2;                // 0x00000fe8
    REG32                pid3;                // 0x00000fec
    REG32                cid0;                // 0x00000ff0
    REG32                cid1;                // 0x00000ff4
    REG32                cid2;                // 0x00000ff8
    REG32                cid3;                // 0x00000ffc
} HWP_ROM_TABLE_T;

#define hwp_rom_table        ((HWP_ROM_TABLE_T *)REG_ROM_TABLE_BASE)

// 0x0 - 0xc, entry0-3
#define PRESENT                         (0x1 << 0)
#define FORMAT                          (0x1 << 1)
#define ADDR_OFFSET                     (0xfffff << 12)

#endif
