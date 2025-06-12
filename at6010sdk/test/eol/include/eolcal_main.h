#ifndef _CALIB_CORE_H_
#define _CALIB_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif
#ifdef CFG_EOL_TESTMOD
#define C_COMPENSATE  (1)

#define CA_STATUS_IDLE                  0
#define CA_STATUS_ANGLEREF_FFT_REQ      1
#define CA_STATUS_ANGLEREF_FFT_READY    2
#define CA_STATUS_LOCALE_INFO_REQ       3
#define CA_STATUS_LOCALE_INFO_READY     4
#define CA_STATUS_LOCALE_FFT_REQ        5
#define CA_STATUS_LOCALE_FFT_READY      6
#define CA_STATUS_SNR_REQ               7
#define CA_STATUS_SNR_READY             8

int32_t eolcal_calib_dispatch(const int16_t *sigData, uint32_t frameId, uint16_t numSample,uint16_t numChirp);

int32_t eolcal_locale_fft_take(eolcal_module_t *calib_module, void *result);
int32_t eolcal_res_take(eolcal_module_t *calib_module, eolcal_locale_info *result);
int32_t eolcal_snr_take(eolcal_module_t *calib_module, void *result);
void eolcal_rsp_timer_start(void);

void vitalmc_calib_loc_set(void *loc_info);
int vitalmc_calib_locale_fft_get_req(void);
int vitalmc_calib_snr_get_req(void);
int vitalmc_calib_loc_get(void *det_info);
void vitalmc_calib_result_set(void *ca_res);
int vitalmc_calib_locale_info_get(void *locale_info);
int vitalmc_calib_locale_fft_get_rsp(void *result);
int vitalmc_calib_snr_get_rsp(void *result);

bool vitalmc_eol_entry_flag(void);
#endif

#ifdef __cplusplus
}
#endif
#endif

