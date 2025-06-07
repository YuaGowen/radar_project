#ifndef __AT_MALLOC_DEFS_H__
#define __AT_MALLOC_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ulp_mem.h"

#define AT_MEM_POOL_0_ID               ULP_MEM_POOL_SYS_ID
#define AT_MEM_POOL_0_BASE             ULP_MEM_POOL_SYS_BASE
#define AT_MEM_POOL_0_SIZE             ULP_MEM_POOL_SYS_SIZE

#define AT_MEM_POOL_1_ID               ULP_MEM_POOL_DSP_ID
#define AT_MEM_POOL_1_BASE             ULP_MEM_POOL_DSP_BASE
#define AT_MEM_POOL_1_SIZE             ULP_MEM_POOL_DSP_SIZE

#define AT_MEM_POOL_CNT                2 /* min: 1, max: 4 */

#ifdef __cplusplus
}
#endif

#endif /* !__AT_MALLOC_DEFS_H__ */
