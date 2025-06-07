#ifndef __SYS_DATA_H__
#define __SYS_DATA_H__

#include "globals_macro.h"
#include "at_types.h"

typedef enum {
	INDEX_SYS_DATA_VERSION = 0,
	INDEX_ANT_PHASE,
	INDEX_INVALID
} sys_data_index_t;

typedef struct {
	u32 calib;
	u32 check_code;
} ant_phase_item_t;

void sys_data_init(u32 version);

void *sys_data_item_get(sys_data_index_t index);
void sys_data_item_set(sys_data_index_t index, void *p);

#endif
