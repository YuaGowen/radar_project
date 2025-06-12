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

#ifndef __MDET_FMCW_H__
#define __MDET_FMCW_H__

#include "at_types.h"

#define ENABLE_FMCW_LOG    1
#define DISABLE_FMCW_LOG   0

#define MD_SWEEP_BW_1GHZ   0
#define MD_SWEEP_BW_5GHZ   1


#define HTM_MODE_STATIC_PROBE        (0)
#define HTM_MODE_DYNA_PROBE          (1)

/**
 * @brief Radar detection result definition
 */
#define MD_RESULT_NONE            (0x0)
#define MD_RESULT_MOT_CLOSE       (0x1 << 0) /* Motion detection: Close to the target */
#define MD_RESULT_MOT_FAR         (0x1 << 1) /* Motion detection: Far away from the target */
#define MD_RESULT_MOT_STAY        (0x1 << 2) /* Motion detection: Stay in place */
#define MD_RESULT_MICRO           (0x1 << 3) /* Micro-motion detection */
#define MD_RESULT_PRESENCE        (0x1 << 4) /* Presence detection */

typedef enum {
	DET_MOD_NORMAL,
	DET_MOD_HTM,
	DET_MOD_POD,
	DET_MOD_ULP,
	DET_MOD_BHR,
}mdet_mode_t;

typedef enum det_err_code {
	DET_ERRNU_OK,
	DET_HTM_ERRNU_NOINIT  = 1,
	DET_HTM_ERRNU_ANTBIAS = 2,
	DET_HTM_ERRNU_MOVE     = 3,
	DET_HTM_ERRNU_NOGROUNDFOUND = 4,
	DET_HTM_ERRNU_STAY_TOO_LONG = 5,
}det_err_code_t;

typedef enum pod_det_mode {
	POD_REPORT_NEAREST,
	POD_REPORT_STRONGEST,
	POD_REPORT_STRONGEST_WITH_RANGECOMPENSATION,
	POD_REPORT_SCORE_CNT,
	POD_REPORT_SCORE_CNT_RGE,
} pod_det_mode_t;

typedef struct {
	s32 threshold_DB;     /* Energy thresholds for different range bins [40, 120] */
	s32 rb_sta;           /* Starting Range Bin [0, 100] */
	s32 rb_num;           /* Number of Range Bin [5, 100] */
	s32 num_frame_acc;    /* Total Number of Frames Accumulating RB Energy */
	s32 ref_rb_cnt;       /* Count of RBs around the target exceeding the threshold */
	s32 ref_rb_threshold; /* Number of relevant RBs exceeding the threshold */
	s32 report_mode;      /* report mode */
	s32 report_delay;     /* history info delay */
} pod_cfg_t ;

typedef enum {
	/* FMCW range parameters and configurations */
	MD_FMCW_PARA_BL_DET_DELTA,
	MD_FMCW_PARA_BL_UNDET_DELTA,
	MD_FMCW_PARA_EXT_DET_DELTA,
	MD_FMCW_PARA_EXT_UNDET_DELTA,
	MD_FMCW_PARA_HIGH_DET_DELTA,
	MD_FMCW_PARA_HIGH_UNDET_DELTA,

	/* FMCW sentivity parameters and configurations */
	MD_FMCW_PARA_BL_SEN,
	MD_FMCW_PARA_EXT_SEN,
	MD_FMCW_PARA_HIGH_SEN,

	/* FMCW other parameters and configurations */
	MD_FMCW_PARA_SKIP_NUM,
} mdet_fmcw_para_t;

typedef enum {
	MD_PROFILE_BASELINE,
	MD_PROFILE_EXTENDED, /* unused profile,reserved for algo, */
	MD_PROFILE_HIGH,
} radar_md_profile_t;

typedef enum {
	MD_PARA_RESP_TRACK_COUNT = 1,
	MD_PARA_SHUTDOWN_PROT_TIME,
	MD_PARA_WIFI_SUPRESSION_RANK,
	MD_PARA_MOVING_ENTER_FRAME_COUNT,
	MD_PARA_MOVING_ACCURACY_VS_REALTIME,
	MD_PARA_MOVING_RANING_BARYCENTER_VS_NEAREST,
	MD_PARA_RESPIRATION_FUNC_BITMAP,
} md_param_type_t;

typedef struct {
	s8 port_sel;
} usr_calib_cfg_h;

typedef enum {
	PWR_DC_NS_NORMAL,  /* only for DC power supply*/
	PWR_AC_NS_LOW,     /* for AC power supply,low level noise suppress*/
	PWR_AC_NS_MID,
	PWR_AC_NS_HIGH,    /* for AC power supply,high level noise suppress*/
} pwr_ns_type_t;

typedef enum {
	MD_MV_TRIGGER = 0,       /* only triggered by movement*/
	MD_MV_BHR_TRIGGER = 2,   /* triggered by movement or breath*/
} md_trigger_mod_t;

typedef enum {
	DET_TYPE_ALL    = 0, /* see @ref fmcw_det_info_t */
	DET_TYPE_HTM    = 1, /* see @ref htm_det_info_t  */
	DET_TYPE_POD    = 2, /* see @ref pod_det_info_t  */
	DET_TYPE_MOT    = 3, /* see @ref mot_det_info_t  */
	DET_TYPE_BHR    = 4, /* see @ref bhr_det_info_t  */
	DET_TYPE_RGN    = 5, /* see @ref rgn_det_info_t  */
	DET_TYPE_BHR_SHORT = 6
	/* Add more here */
} det_result_type_t;

typedef struct {
	u8 is_detected;   /* the detected result that combined motion, micro-motion and presence. */
	u8 det_result;    /* detection result */
	u16 range_val;    /* range detection value, unit: mm */
	s16 angle_val;    /* angle detection value, unit: 1degree */
	s16 velo_val;     /* velocity, todo. */
	u8 reserved[6];   /* reserved for extend. */

	u8 rb_conf;       /* mix range detection confidence = 0~16, if confidence < 12, range_val may be incorrect */
	u8 angle_conf;    /* mix angle detection confidence = 0~16, if confidence < 8, angle_val may be incorrect */
	u32 frame_idx;    /* frame index. */
} fmcw_det_info_t;

typedef struct  {
	u16 range_val;
	s16 angle_val;
} rgn_obj_info_t;

typedef struct {
	u32 obj_num;           /* Number of object,only output the nearest object in each region,max obj num = 3 */
	rgn_obj_info_t obj[3]; /* Detection information of the nearest object in each region, max region = 3 */
} rgn_det_info_t;

/**
 * @brief Bound to @ref DET_TYPE_BHR
 *
 */
typedef struct {
	u8  det_result;   /* detection result */
	u8  br_val;       /* breathing rate */
	u8  hr_val;       /* heart rate */
	u8  angle_val;    /* reserve */
	u16 range_val;     /* range val mm */
	u16 padding;       /* reserve */
} bhr_det_info_t;

typedef struct {
	u8 is_detected;
	u8 det_result; /* detection result */
	u8 br_val;     /* breathing rate */
	u8 hr_val;     /* heart rate */
	u8 reserved;
} bhr_det_info_short_t;

/**
 * @brief Bound to @ref DET_TYPE_HTM
 *
 */
typedef struct {
	u16 htm;          /* height, mm */
	u8  status;       /* 0:invalid,1: calib done,2:enter htm, 3:htm done*/
	u8  reserved;
} htm_det_info_t;

typedef struct {
	s8  rb;
	u8  pwr_db_raw;  /* power raw */
	u8  pwr_db_rgc;  /* power range compensation */
	u8  num_rb_greater_than_thr; /* num neighbor RB greater than threshold */
} pod_inf_t, pod_det_info_t;

typedef struct {
#ifdef  CFG_APP_POD
	pod_inf_t mobj;
	pod_inf_t refobj[10];
#else
	u32       reserved;
#endif
} fmcw_det_auxinfo_t;

typedef struct {
	u8 is_detected;   /* the detected result that combined motion, micro-motion and presence. */
	u8 det_result;    /* detection result */
	u16 range_val;    /* range detection value, unit: mm */
	s16 angle_val;    /* angle detection value, unit: 1degree */
	s16 velo_val;     /* velocity, todo. */
} mot_det_info_t;

typedef void (*data_output_handle)(u32,u32);
typedef void (*htm_ca_notify)(u32);
typedef void (*htm_sta_notify)(void);


void *radar_fmcw_md_init(u8 md_profile, u8 max_rb_bl, u8 max_rb_high, u8 swp_bw, u8 mode);

u8 radar_fmcw_md_perform(void *md);
/**
 *
 * @param  md        Fmcw detector handler.
 * @param  info      Fmcw radar algo detect info
 * @return none
 *
 * @brief            Get fmcw radar algo detect info.
 * @note             If is_detected == 1 but (is_mov || is_bhr) == 0, then algo delay happened.
 */
void radar_fmcw_md_det_info(void *md, fmcw_det_info_t *info);

void radar_fmcw_md_set_sensitivity(void *md, u16 profile, u8 level);
u8 radar_fmcw_is_support_bhr(void *md);

/**
 *
 * @param  dist_cm        The move detect range,max dist:1100cm
 * @param  min_confidence The range min confidence,range will be invalid if less than min_confidence, max:16
 * @return none
 *
 * @brief                 Set move range filter.
 */
void radar_fmcw_mv_range_conf_set(s32 min_confidence);

/**
 *
 * @param  dist_cm        The move detect range,max dist:6500cm
 * @param  min_confidence The range min confidence,range will be invalid if less than min_confidence,max :16
 * @return none
 *
 * @brief                 Set breath range filter.
 * @note                  only filter distance info
 */
void radar_fmcw_bhr_range_conf_set(s32 min_confidence);

void radar_fmcw_trigger_mode_set(md_trigger_mod_t mode);

/**
*  @brief                 set some special algo cfg.
*/
s32 radar_fmcw_md_param_special_cfg(u8 type, u32 para);

/**
*  @brief                 get some special algo cfg.
*/
s32 radar_fmcw_md_param_special_get(u8 type);

/**
*  @brief                 only for algo developer debug.
*/
u32 radar_fmcw_md_interference_quality_get(void);

void radar_fmcw_active_time_keep(u32 hold_time);

/**
 *
 * @param  md    Detected algo instance
 * @param  delta The range where the detection is forbidden
 * @return none
 *
 * @brief         Set undetected range.
 */
void radar_fmcw_md_set_undet_delta(void *md,u16 profile,u16 delta);

/**
 *
 * @param md Detected algo instance
 * @param delta The range where the detection is allowed
 * @return none
 *
 * @brief  set detected range.
 */
 void fmcw_md_para_set(void *md, u32 value);

void radar_fmcw_md_set_det_delta(void *md, u16 profile, u16 delta);
void radar_fmcw_md_reset(void *md, u8 is_hard);
s32 radar_fmcw_md_pause(void *md, u8 is_hard);
s32 radar_fmcw_md_resume(void *md, u8 is_hard);
void radar_fmcw_md_log_onoff(void *md,bool onoff);
//void radar_fmcw_md_ca_conf(void *md, usr_calib_cfg_h cfg);
void radar_fmcw_md_destroy(void *md);
void radar_fmcw_mv_pwr_dump_cfg(void *md,u8 rb_num);
void radar_fmcw_mv_pwr_dump(void *md, u32 *cap);
void radar_fmcw_htm_mode_set(u8 htm_mode);
void radar_fmcw_htm_stat_ret_cfg(void *md,u8 stat_cnt,u8 repeat_cnt_min,u8 err_suppress);
void radar_fmcw_htm_ca_start(void *md);
void radar_fmcw_htm_offset_set(void *md,u8 offset);
void radar_fmcw_md_ulp_ca(void *md, data_output_handle cb);

void radar_fmcw_md_ca_cfg(void *md, data_output_handle cb);

void* radar_fmcw_md_ulp_pc_buf_get(void *md);/* point cloud data buffer size:68bytes*/

s32 radar_fmcw_pod_param_set(void *md,pod_cfg_t *cfg);
void radar_fmcw_md_det_auxinfo(void *md, void *info);

void radar_fmcw_rx1_phase_180_degree_shift(void);
void *radar_htm_ulp_cs_init(void);
u8 radar_htm_ulp_algo_perform(void *inst);
void radar_htm_ulp_destroy(void);
void radar_htm_ulp_hs_init(u8 md_profile, u8 max_rb_bl, u8 max_rb_high);
void radar_htm_ulp_mdet_hwa_cfg(void);
void radar_htm_ulp_det_info(fmcw_det_info_t *info);
void radar_fmcw_htm_ulp_notify(void *md, htm_ca_notify calib_done,htm_sta_notify htm_enter);

u8 mdet_postproc(void *md, u8 ret_val);


#endif /* end of __MDET_FMCW_H__ */
