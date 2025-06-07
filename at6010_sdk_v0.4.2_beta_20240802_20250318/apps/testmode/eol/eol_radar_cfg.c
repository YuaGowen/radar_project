#include "at_types.h"
#include "cpu.h"
#include "eol_radar_cfg.h"
#include "cfg_radar_hwa_eol.h"
#include "hal_radar.h"
#include "hal_fft.h"
#include "atlog.h"

#define FFT_WIN_ATTRIBUTES  __attribute__ ((section(".fft_win_mem"),__aligned__(32)))

static u16 eol_win_normal[] FFT_WIN_ATTRIBUTES = {
	#include "win_hann_eol.tab"
};

typedef struct {
	u32 pingpong_buf;
	volatile u32 frame_idx;
	u16 frame_len;
	volatile u8 fifo_state;
}eol_radar_cfg_t;

enum enum_fifo_wp_status {
	DEV_FIFO_WP_STA = 0,
	DEV_FIFO_WP_HALF = 1,
	DEV_FIFO_WP_END = 2,
	DEV_FIFO_WP_BUSY = 4, /* only for workaround fmcw */
};


static eol_radar_cfg_t m_eol_radar_cfg_ctx;

static void dev_fifo_half_handler(void)
{
	m_eol_radar_cfg_ctx.fifo_state = DEV_FIFO_WP_HALF;
	m_eol_radar_cfg_ctx.frame_idx++;
}

static void dev_fifo_end_handler(void)
{
	m_eol_radar_cfg_ctx.fifo_state = DEV_FIFO_WP_END;
	m_eol_radar_cfg_ctx.frame_idx++;
}

void eol_radar_rangefft_cfg(eol_radar_cfg_t *cfg,u8 adc_ch)
{
	adc_range_fft_para_t fft_cfg = {
		.enable = 1,
		.dc_cut = EOL_FFT_DC_CUT_ENABLE,
		.num_adc_ch = adc_ch,
		.shift_value = 0x4,
		.fft_point = EOL_FFT_SIZE,
		.nframe = EOL_FFT_NFRAME,
		.rfft_upper_rb = EOL_FFT_OUT_VALID_RB,
		.in_frame_size = EOL_FFT_INPUT_SIZE,
		.out_buf = (u32)cfg->pingpong_buf,
		.half_int_en = 1,
		.end_callback = dev_fifo_end_handler,
		.half_callback = dev_fifo_half_handler,
	};

	hal_fft_init();

	hal_fft_set_config(FFT_IDX_CONFIG_ADC_RANGE_FFT, (void *)&fft_cfg);

}

static void eol_radar_win_cfg(void)
{
	fft_win_para_t win_cfg = {
		.enable = 0,
		.waddr = (u32)eol_win_normal,
		.fft_mode = FFT_ADC_RANGE_FFT_MODE,
	};

	hal_fft_set_config(FFT_IDX_CONFIG_ADC_WIN, (void *)&win_cfg);
}

u32 eol_radar_rangefft_get(u16 *len)
{
	u32 d_update = 0;
	u32 start_addr = (u32)m_eol_radar_cfg_ctx.pingpong_buf;

	critical_section_start();

	if (m_eol_radar_cfg_ctx.fifo_state  & DEV_FIFO_WP_HALF) {
		d_update = start_addr;
	} else if (m_eol_radar_cfg_ctx.fifo_state  & DEV_FIFO_WP_END) {
		d_update = start_addr + m_eol_radar_cfg_ctx.frame_len;
	}

	m_eol_radar_cfg_ctx.fifo_state = DEV_FIFO_WP_STA;

	*len = m_eol_radar_cfg_ctx.frame_len;

	critical_section_end();

	return d_update;
}

void eol_radar_dev_init(void)
{
	u32 rx_num = 0,flen = 0;

	hal_radar_param_get(RADAR_PRM_RX_NUM,&rx_num);

	m_eol_radar_cfg_ctx.pingpong_buf= 0x20020000;

	flen = EOL_FFT_OUT_VALID_RB * EOL_FFT_NFRAME * EOL_FFT_NUM_ADC_CH;

	m_eol_radar_cfg_ctx.frame_len = (flen << 2);

	eol_radar_win_cfg();
	eol_radar_rangefft_cfg(&m_eol_radar_cfg_ctx,rx_num);
}

void eol_radar_rf_cfg(void)
{
	u16 chirp_size = 256;
	frame_cfg_t frm_cfg = {
			.repeat_times = 65536,
			.interval_us  = 24500U,
			.bpm_seed = 0,
			.subfrm_id = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U},
			.subfrm_num = 1,
	};

	radar_tx_gain_set(TX_GAIN_MAX);
	radar_param_hpf1_gain_set(HPF1_GAIN_MAX_MINUS_4dB);
	radar_param_hpf1_freq_set(HPF_CORNER_FREQ_175kHz);
	radar_frame_config(&frm_cfg);
	radar_filter_chirp_size_set(chirp_size);
}

