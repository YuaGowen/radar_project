#ifndef _CALIB_COMM_H_
#define _CALIB_COMM_H_

#ifdef __cplusplus
extern "C" {
#endif
#ifdef CFG_EOL_TESTMOD
#define C_IQ_I (0)
#define C_IQ_Q (1)
#define C_IQ   (2)
#define C_ANT0 (0)
#define C_ANT1 (1)
#define C_VITAL_PI  (3.1415926535897f)
	
#define CA_RB_COUNT                         (20)
#define MC_NUM_RX                           (2)
#define MAX_OBJ_NUM                         (0x5)
#define EOLCAL_MAX_RSP_FRAME_NUM            (0x5)
	
#define C_FREQ_DATA_QUEUE                   (4)

typedef struct dataQueue_tag {
	int32_t freqQueueDataPt;
	int32_t freqDataQueue[C_FREQ_DATA_QUEUE][CA_RB_COUNT * C_IQ];
	int32_t freqDataSum[CA_RB_COUNT * C_IQ];
} dataQueue_t; //2004Byte per Antenna

typedef struct _eolcal_obj_locinfo_t{
	uint32_t obj_num;
	s16 sRange[MAX_OBJ_NUM];
	s16 sAngle0[MAX_OBJ_NUM];
	s16 sAngle1[MAX_OBJ_NUM];
	s16 sAngle2[MAX_OBJ_NUM];
} eolcal_obj_locinfo_t;

typedef struct obj_det {
	uint8_t rx_num;
	uint8_t obj_num;
	uint16_t frame_seq;
	uint32_t rx0FFT[MAX_OBJ_NUM];
	uint32_t rx1FFT[MAX_OBJ_NUM];
 } obj_det_info_t;

typedef struct eolcal_locale_info_t {
	uint32_t obj_num;
	int16_t range[MAX_OBJ_NUM];
	int16_t angle[MAX_OBJ_NUM];
} eolcal_locale_info;

typedef struct eolcal_module_tag {
	uint32_t frame_num;
	eolcal_obj_locinfo_t loc;
	obj_det_info_t det_info[EOLCAL_MAX_RSP_FRAME_NUM];
	eolcal_obj_locinfo_t det_loc;
	uint8_t status;
	int8_t snr_val[2];
	uint8_t rsvd;
	s16 ca_res_delta;
	s16 ca_res_k;
	eolcal_locale_info locale_info;
	//uint32 *cust_ptr;
	int32_t fft_data[MC_NUM_RX][CA_RB_COUNT];
} eolcal_module_t;

typedef struct _eolcal_struct {
	int32_t isFirstFrameReady;
	uint32_t firstFrame;
	dataQueue_t pQueue[MC_NUM_RX];
	eolcal_module_t ca_module;
	int32_t rx_data[2][CA_RB_COUNT*C_IQ];
} eolcal_struct;

int32_t phasecal_angle_common(s16 delta, s16 K, int32_t ref0I, int32 ref0Q, int32 ref1I, int32 ref1Q);
int8_t snr_calculate(int32_t *data, int32_t len);
#endif
#ifdef __cplusplus
}
#endif
#endif


