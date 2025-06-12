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

#ifndef __RADAR_SHELL_IMPL_H__
#define __RADAR_SHELL_IMPL_H__
s32 radar_shell_power_set(size_t argc, char **argv);
s32 radar_shell_rf_fmcw_init(size_t argc, char **argv);
s32 radar_shell_rf_fmcw_freq_set(size_t argc, char **argv);
s32 radar_shell_rf_fmcw_tx_gain_set(size_t argc, char **argv);
s32 radar_shell_detect_stop(size_t argc, char **argv);
s32 radar_shell_detect_start(size_t argc, char **argv);
s32 radar_shell_user_md_set(size_t argc, char **argv);
s32 radar_shell_user_bhr_set(size_t argc, char **argv);
s32 radar_shell_user_md_get(size_t argc, char **argv);
s32 radar_shell_user_bhr_get(size_t argc, char **argv);
s32 radar_shell_mdet_sen_set(size_t argc, char **argv);
s32 radar_shell_mdet_sen_get(size_t argc, char **argv);
s32 radar_shell_bhr_sen_get(size_t argc, char **argv);
s32 radar_shell_bhr_sen_set(size_t argc, char **argv);
s32 radar_shell_bhr_det_start(size_t argc, char **argv);
s32 radar_shell_bhr_det_stop(size_t argc, char **argv);
s32 radar_shell_pod_param_set(size_t argc, char **argv);
s32 radar_shell_pod_det_log_en(size_t argc, char **argv);
s32 radar_shell_md_log_en(size_t argc, char **argv);
s32 radar_shell_md_log_disable(size_t argc, char **argv);

#endif
