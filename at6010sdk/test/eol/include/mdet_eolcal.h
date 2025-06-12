#ifndef __MDET_EOLCAL_H__
#define __MDET_EOLCAL_H__

#include "at_types.h"
#include "cpu.h"

#define CA_LOCALE_OBJ_MAX                              5
#define BUFFER_PROP_TRIGGER_STATE_MSK                  (0x1 << 0)
#define BUFFER_PROP_TRIGGER_STATE(x)                   ((x & 0x1) << 0)

typedef struct {
	u32 obj_num;
	s16 range[CA_LOCALE_OBJ_MAX];
	s16 ang0[CA_LOCALE_OBJ_MAX];
	s16 ang1[CA_LOCALE_OBJ_MAX];
	s16 ang2[CA_LOCALE_OBJ_MAX];
} fmcw_det_ca_loc_t;

typedef struct  {
	u8 rx_num;
	u8 obj_num;
	u16 frame_seq;
	u32 rx0FFT[CA_LOCALE_OBJ_MAX];
	u32 rx1FFT[CA_LOCALE_OBJ_MAX];
 } fmcw_det_ca_info_t;

typedef struct _phase_ca_loc_info_t {
	u32 obj_num;
	s16 range[CA_LOCALE_OBJ_MAX];
	s16 angle[CA_LOCALE_OBJ_MAX];
}phase_ca_loc_info_t;

enum enum_fifo_wp_status {
	DEV_FIFO_WP_STA = 0,
	DEV_FIFO_WP_HALF = 1,
	DEV_FIFO_WP_END = 2,
	DEV_FIFO_WP_BUSY = 4, /* only for workaround fmcw */
};

typedef struct {
	u32 trigger_state_half: 1;
	u32 trigger_state_end: 1;
	u32 reserve: 14;
	u32 cbuffer_prop: 16;
} buf_prop_t;

typedef struct PutByteContext {
    uint8_t *buffer, *buffer_end, *buffer_start;
    int eof;
} PutByteContext;


typedef struct {
	u8 *data_buf;
	void *energy_storage;
	u32 pingpong_buf;
	//u8 framerate;
	u32 storage_rb_num;
	s32 buf_len;
	s32 frame_len;
	void *mdt_handler;
	s8 state;
	volatile s32 fifo_state ;
	volatile u32 *frame_idx;
	volatile u8 is_buffer_empty;
	PutByteContext hdr1;
	s8 ca_conf;
	s32 uart_dev_ch;
	u8 rb_base;
	u8 rb_ext;
	u8 rb_high;
	u8 is_hw_sp_bhr;
	u8 sweep_bw;
	u8 mode;
	u16 skip_chirp_num;
	buf_prop_t buffer_prop;
	u8 log_info_en;
	u8 num_chirp_per_frm;
	u16 vld_dat_per_chirp;
} radar_eol_md_proc_t;

#if defined(CFG_EOL_TESTMOD)

s32  eolcal_init(void);
void eolcal_free(void);
s32  radar_fmcw_ca_info_get(fmcw_det_ca_info_t *ca_det_info);
void radar_fmcw_ca_res_set(void *ca_res);
s32  radar_fmcw_ca_tgt_info_get(phase_ca_loc_info_t *ca_locale_info);
void radar_fmcw_ca_loc_set(void *loc_info);
s32  radar_fmcw_ca_loc_fft_dump_req(void);
s32  radar_fmcw_ca_snr_req(void);
s32  radar_fmcw_ca_locale_fft_get(void *res_p);
s32  radar_fmcw_ca_snr_get(void *res_p);
bool radar_eol_entry_flag(void);
u8   radar_eol_md_perform(void *md);
void radar_fmcw_ca_dispatch(const int16_t * sigData);

#endif /* !CFG_EOL_TESTMOD */

#endif /* !__MDET_EOLCAL_H__ */
