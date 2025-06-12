/**
 * Copyright (c) 2023, Airtouching Intelligence Technology.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Airtouching
 *    Intelligence Technology integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY AIRTOUCHING TECHNOLOGY "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AIRTOUCHING TECHNOLOGY OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __MPU_H__
#define __MPU_H__

#include "chip_defs.h"
#include "at_types.h"

typedef enum {
	MPU_REGION_SIZE_256b  = 7,        /**< 256  byte region size. */
	MPU_REGION_SIZE_512b  = 8,        /**< 512  byte region size. */
	MPU_REGION_SIZE_1Kb   = 9,        /**< 1K   byte region size. */
	MPU_REGION_SIZE_2Kb   = 10,       /**< 2K   byte region size. */
	MPU_REGION_SIZE_4Kb   = 11,       /**< 4K   byte region size. */
	MPU_REGION_SIZE_8Kb   = 12,       /**< 8K   byte region size. */
	MPU_REGION_SIZE_16Kb  = 13,       /**< 16K  byte region size. */
	MPU_REGION_SIZE_32Kb  = 14,       /**< 32K  byte region size. */
	MPU_REGION_SIZE_64Kb  = 15,       /**< 64K  byte region size. */
	MPU_REGION_SIZE_128Kb = 16,       /**< 128K byte region size. */
	MPU_REGION_SIZE_256Kb = 17,       /**< 256K byte region size. */
	MPU_REGION_SIZE_512Kb = 18,       /**< 512K byte region size. */
	MPU_REGION_SIZE_1Mb   = 19,       /**< 1M   byte region size. */
	MPU_REGION_SIZE_2Mb   = 20,       /**< 2M   byte region size. */
	MPU_REGION_SIZE_4Mb   = 21,       /**< 4M   byte region size. */
	MPU_REGION_SIZE_8Mb   = 22,       /**< 8M   byte region size. */
	MPU_REGION_SIZE_16Mb  = 23,       /**< 16M  byte region size. */
	MPU_REGION_SIZE_32Mb  = 24,       /**< 32M  byte region size. */
	MPU_REGION_SIZE_64Mb  = 25,       /**< 64M  byte region size. */
	MPU_REGION_SIZE_128Mb = 26,       /**< 128M byte region size. */
	MPU_REGION_SIZE_256Mb = 27,       /**< 256M byte region size. */
	MPU_REGION_SIZE_512Mb = 28,       /**< 512M byte region size. */
	MPU_REGION_SIZE_1Gb   = 29,       /**< 1G   byte region size. */
	MPU_REGION_SIZE_2Gb   = 30,       /**< 2G   byte region size. */
	MPU_REGION_SIZE_4Gb   = 31        /**< 4G   byte region size. */
} mpu_region_size;

typedef enum {
	MPU_REGION_1 = 0,
	MPU_REGION_2 = 1,
	MPU_REGION_3 = 2,
	MPU_REGION_4 = 3,
	MPU_REGION_5 = 4,
	MPU_REGION_6 = 5,
	MPU_REGION_7 = 6,
	MPU_REGION_8 = 7,
} mpu_region_nb;

/**
 * MPU access permission attributes.
 */
typedef enum {
	MPU_REGION_NO_ACCESS   = 0,  /**< No access what so ever.                   */
	MPU_REGION_PR_RW       = 1,  /**< Access from privileged software only.               */
	MPU_REGION_PR_RW_UR_RO = 2,  /**< Writes by unprivileged software generate a permission fault. */
	MPU_REGION_FULL_ACCESS = 3,  /**< Full access.        */
	MPU_REGION_PR_RO       = 5,  /**< Reads by privileged software only.                       */
	MPU_REGION_PR_RO_UR_RO = 6   /**< Read only, by privileged or unprivileged software.     */
} mpu_access_attr;


typedef struct {
	u8 region_enable; /**< MPU region enable.                */
	u8 region_nb;     /**< MPU region number.                */
	u32 base_addr;    /**< Region baseaddress.               */
	u8 region_size;   /**<  region size.               */
	u8 ap;            /**<  access permissions.   */
	bool exeable;     /**<  execution able                */
	bool shareable;   /**<  shareable attribute.       */
	bool cacheable;   /**<  cacheable attribute.       */
	bool bufferable;  /**<  bufferable attribute.      */
	u8 srd;           /**<  subregion disable bits.    */
	u8 tex;
} mpu_region_conf;

#define MPU_FLASH_CONF_DEFAULT                                      \
{                                                                   \
	true,                   /* Enable MPU region.            */ \
	0,                      /* MPU Region number.            */ \
	FLASH_BASE_ADDR,        /* Flash base address.           */ \
	MPU_REGION_SIZE_256Kb,  /* Size - Set to max. */            \
	MPU_REGION_FULL_ACCESS, /* Access permissions.           */ \
	true,                   /* Execution allowed.            */ \
	true,                   /* shareable.                */     \
	true,                   /* Cacheable.                    */ \
	true,                   /*  bufferable.               */    \
	0,                      /*  subregions.                */   \
	0                       /*  TEX attributes.            */   \
}

#define MPU_SRAM_CONF_DEFAULT_P0                                    \
{                                                                   \
	true,                   /* Enable MPU region.            */ \
	1,                      /* MPU Region number.            */ \
	SRAM_BASE_ADDR,         /* SRAM base address.            */ \
	MPU_REGION_SIZE_64Kb,   /* Size - Set to max. */            \
	MPU_REGION_FULL_ACCESS, /* Access permissions.           */ \
	true,                   /* Execution allowed.            */ \
	true,                   /* Shareable.                    */ \
	true,                   /* Cacheable.                    */ \
	true,                   /*  bufferable.               */    \
	0,                      /*  subregions.                */   \
	0                       /*  TEX attributes.            */   \
}

#define MPU_SRAM_CONF_DEFAULT_P1                                          \
{                                                                         \
	true,                         /* Enable MPU region.            */ \
	2,                            /* MPU Region number.            */ \
	SRAM_BASE_ADDR + (64 * 1024), /* SRAM base address.            */ \
	MPU_REGION_SIZE_16Kb,         /* Size - Set to max. */            \
	MPU_REGION_FULL_ACCESS,       /* Access permissions.           */ \
	true,                         /* Execution allowed.            */ \
	true,                         /* Shareable.                    */ \
	true,                         /* Cacheable.                    */ \
	true,                         /*  bufferable.               */    \
	0,                            /*  subregions.                */   \
	0                             /*  TEX attributes.            */   \
}

#define MPU_CACHE_CONF_DEFAULT                                      \
{                                                                   \
	true,                   /* Enable MPU region.            */ \
	3,                      /* MPU Region number.            */ \
	CACHE_FLASH_ADDR,       /* SRAM base address.            */ \
	MPU_REGION_SIZE_256Kb,  /* Size - Set to max. */            \
	MPU_REGION_FULL_ACCESS, /* Access permissions.           */ \
	true,                   /* Execution allowed.            */ \
	true,                   /* Shareable.                    */ \
	true,                   /* Cacheable.                    */ \
	true,                   /*  bufferable.               */    \
	0,                      /*  subregions.                */   \
	0                       /*  TEX attributes.            */   \
}

#define MPU_PERIPHERAL_CONF_DEFAULT_P0                               \
{                                                                    \
	true,                    /* Enable MPU region.            */ \
	4,                       /* MPU Region number.            */ \
	PERIPHERAL_BASE_ADDR_P0, /* Region base address.          */ \
	MPU_REGION_SIZE_64Kb,    /* Size - Set to minimum         */ \
	MPU_REGION_FULL_ACCESS,  /* Access permissions.           */ \
	true,                    /* Execution not allowed.        */ \
	true,                    /* Shareable.                    */ \
	true,                    /*  cacheable.                */    \
	true,                    /* Bufferable.                   */ \
	0,                       /*  subregions.                */   \
	0                        /*  TEX attributes.            */   \
}

#define MPU_PERIPHERAL_CONF_DEFAULT_P1                               \
{                                                                    \
	true,                    /* Enable MPU region.            */ \
	5,                       /* MPU Region number.            */ \
	PERIPHERAL_BASE_ADDR_P1, /* Region base address.          */ \
	MPU_REGION_SIZE_64Kb,    /* Size - Set to minimum         */ \
	MPU_REGION_FULL_ACCESS,  /* Access permissions.           */ \
	true,                    /* Execution not allowed.        */ \
	true,                    /* Shareable.                    */ \
	true,                    /*  cacheable.                */    \
	true,                    /* Bufferable.                   */ \
	0,                       /*  subregions.                */   \
	0                        /*  TEX attributes.            */   \
}

#define MPU_PERIPHERAL_CONF_DEFAULT_P2                                             \
{                                                                                  \
	true,                                  /* Enable MPU region.            */ \
	6,                                     /* MPU Region number.            */ \
	PERIPHERAL_BASE_ADDR_P0 + (64 * 1024), /* Region base address.          */ \
	MPU_REGION_SIZE_4Kb,                   /* Size - Set to minimum         */ \
	MPU_REGION_FULL_ACCESS,                /* Access permissions.           */ \
	true,                                  /* Execution not allowed.        */ \
	true,                                  /* Shareable.                    */ \
	true,                                  /*  cacheable.                */    \
	true,                                  /* Bufferable.                   */ \
	0,                                     /*  subregions.                */   \
	0                                      /*  TEX attributes.            */   \
}

void mpu_enable(void);
void mpu_disable(void);
void mpu_region_init(void);
void mpu_region_setup(u32 base_addr, mpu_region_size region_size,
		      mpu_region_nb region_nb, mpu_access_attr mode);

#endif
