/**
 *******************************************************************************
 * @file    ymodem.c
 * @author  Airtouch Software Team
 * @brief   Ymodem protocol
 *******************************************************************************
 * @copyright Copyright (c) 2024, Airtouching Intelligence Technology.
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
 *    Intelligence Technology integrated circuit in a product or a software update
 *    for such product, must reproduce the above copyright notice, this list of
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
 ******************************************************************************/

/* Includes --------------------------------------------------------------------------------------*/

#include "ymodem.h"
#include "at6010.h"
#include "common.h"
#include "flash_if.h"
#include "string.h"
#include "uart_if.h"
#include "atlog.h"

#if defined(CFG_OTA)

/* Private typedef -------------------------------------------------------------------------------*/

#define FORCE_YMODEM_128        /**< Un-comment this line to enable Ymodem-1K mdoe */

#define FORCE_VERIFY_CRC16      /**< Un-comment this line to enable CHECKSUM verification */

/* Private variables -----------------------------------------------------------------------------*/

static u8 mPacketData[PACKET_1K_SIZE + PACKET_DATA_INDEX + PACKET_TRAILER_SIZE];

static u8 mRecvFileName[FILE_NAME_LENGTH];

/* Private Functions -----------------------------------------------------------------------------*/

static u16 UpdateCRC16(u16 crc_in, u8 data)
{
	u32 crc_out = crc_in;
	u32 in_byte = data | 0x100;

	do {
		crc_out <<= 1;
		in_byte <<= 1;

		if (in_byte & 0x100) {
			++crc_out;
		}

		if (crc_out & 0x10000) {
			crc_out ^= 0x1021;
		}
	} while (!(in_byte & 0x10000));

	crc_out = crc_out & 0xFFFF;
	return crc_out;
}

static u16 Cal_CRC16(const u8* buffer, u32 size)
{
	u32 crc_val = 0;
	u8 *p_start = (u8 *)buffer;
	u8 *p_end = p_start + size;

	while (p_start < p_end) {
		crc_val = UpdateCRC16(crc_val, *p_start++);
	}

	crc_val = UpdateCRC16(crc_val, 0);
	crc_val = UpdateCRC16(crc_val, 0);

	crc_val &= 0xFFFF;
	return crc_val;
}

static u8 CalcChecksum(const u8* buffer, u32 size)
{
	u32 sum = 0;
	u8 *p_start = (u8 *)buffer;
	const u8* p_end = p_start + size;

	while (p_start < p_end) {
		sum += *p_start++;
	}

	return (sum & 0xffu);
}

/**
 * @brief Receive a packet from sender.
 *
 * @param[out] recv_buffer A buffer to hold received data.
 * @param[out] recv_length The received packet length in bytes.
 * @param[in] timeout
 * @return status_code_t
 */
static status_code_t ReceivePacket(u8* recv_buffer, u32* recv_length, u32 timeout)
{
	u8  recv_char;
	u32 recv_crc;
	u32 recv_pkt_size = 0;
	status_code_t status;

	*recv_length = 0;

	status = uart_if_recv_timeout(&port_uart, &recv_char, 1, timeout);

	if (status == STATUS_OK) {

		switch (recv_char) {
		case SOH:
			recv_pkt_size = PACKET_SIZE;
			break;

		case STX:
			recv_pkt_size = PACKET_1K_SIZE;
			break;

		case EOT:
			break;

		case CAN:
			if ((uart_if_recv_timeout(&port_uart, &recv_char, 1, timeout) == STATUS_OK) && (recv_char == CAN)) {
				recv_pkt_size = 2;
			} else {
				status = STATUS_ERROR;
			}
			break;

		case ABORT1:
		case ABORT2:
			status = STATUS_BUSY;
			break;

		default:
			status = STATUS_ERROR;
			break;
		}

		*recv_buffer = recv_char;

		if (recv_pkt_size >= PACKET_SIZE) {
			status = uart_if_recv_timeout(&port_uart, &recv_buffer[PACKET_NUMBER_INDEX], recv_pkt_size + PACKET_OVERHEAD_SIZE, timeout);
			if (status == STATUS_OK) {
				if (recv_buffer[PACKET_NUMBER_INDEX] != ((recv_buffer[PACKET_CNUMBER_INDEX]) ^ NEGATIVE_BYTE)) {
					recv_pkt_size = 0;
					status = STATUS_ERROR;
				} else {
					recv_crc = recv_buffer[recv_pkt_size + PACKET_DATA_INDEX] << 8;
					recv_crc += recv_buffer[recv_pkt_size + PACKET_DATA_INDEX + 1];
					if (Cal_CRC16(&recv_buffer[PACKET_DATA_INDEX], recv_pkt_size) != recv_crc) {
						recv_pkt_size = 0;
						status = STATUS_ERROR;
					}
				}
			} else {
				recv_pkt_size = 0;
			}
		}
	}

	*recv_length = recv_pkt_size;

	return status;
}

static void PrepareIntialPacket(u8* p_data, const u8* name_buffer, u32 data_len)
{
	u32 i, j = 0;
	u8 len_string[FILE_SIZE_LENGTH + 1];

	/* first 3 bytes are constant */
	p_data[PACKET_START_INDEX] = SOH;
	p_data[PACKET_NUMBER_INDEX] = 0x00;
	p_data[PACKET_CNUMBER_INDEX] = 0xff;

	/* put file name */
	for (i = 0; (name_buffer[i] != '\0') && (i < FILE_NAME_LENGTH); i++) {
		p_data[i + PACKET_DATA_INDEX] = name_buffer[i];
	}

	p_data[i + PACKET_DATA_INDEX] = 0x00;

	/* put file length */
	ytool_int2str(data_len, len_string);

	i = i + PACKET_DATA_INDEX + 1;
	while (len_string[j] != '\0') {
		p_data[i++] = len_string[j++];
	}

	/* padding with zeros */
	for (j = i; j < PACKET_SIZE + PACKET_DATA_INDEX; j++) {
		p_data[j] = 0;
	}
}

static void PreparePacket(u8* p_source, u8* p_packet, u8 pkt_nr, u32 size_blk)
{
	u8* p_record;
	u32 i, size, packet_size;

#ifdef FORCE_YMODEM_128
	packet_size = PACKET_SIZE;
#else
	packet_size = size_blk >= PACKET_1K_SIZE ? PACKET_1K_SIZE : PACKET_SIZE;
#endif

	size = size_blk < packet_size ? size_blk : packet_size;
	if (packet_size == PACKET_1K_SIZE) {
		p_packet[PACKET_START_INDEX] = STX;
	} else {
		p_packet[PACKET_START_INDEX] = SOH;
	}
	p_packet[PACKET_NUMBER_INDEX] = pkt_nr;
	p_packet[PACKET_CNUMBER_INDEX] = (~pkt_nr);
	p_record = p_source;

	for (i = PACKET_DATA_INDEX; i < size + PACKET_DATA_INDEX; i++) {
		p_packet[i] = *p_record++;
	}

	/* End of File: padding with 0x1A or 0x00 */
	if (size <= packet_size) {
		for (i = size + PACKET_DATA_INDEX; i < packet_size + PACKET_DATA_INDEX; i++) {
			p_packet[i] = 0x1A;
		}
	}
}


/* Public functions ------------------------------------------------------------------------------*/

Ymodem_StatusTypeDef Ymodem_Receive(u32* recv_filesize, const u32 part_offset, const u32 part_capacity)
{
	u32 i = 0, packet_length = 0, errors = 0, session_begin = 0;
	volatile u32 session_done = 0, file_done = 0;
	u32 retry_cnt = 0;
	u32 filesize = 0, packets_received = 0;
	uint8    *ramsource = 0;
	u8* file_ptr = NULL;
	u8 file_size_str[FILE_SIZE_LENGTH];
	Ymodem_StatusTypeDef recv_result = YMODEM_STATUS_OK;

	__IO u32 flashdestination = part_offset;

	while ((session_done == 0) && (recv_result == YMODEM_STATUS_OK)) {
		packets_received = 0;
		file_done = 0;

		while ((file_done == 0) && (recv_result == YMODEM_STATUS_OK)) {
			switch (ReceivePacket(mPacketData, &packet_length, DOWNLOAD_TIMEOUT)) {
			case STATUS_OK:
				errors = 0;

				switch (packet_length) {
				case 2: /* Abort by sender */
					uart_if_send_byte(ACK);
					recv_result = YMODEM_STATUS_ABORT;
					break; // 2

				case 0: /* End of transmission */
					uart_if_send_byte(ACK);
					file_done = 1;
					break; // 0

				default: /* Normal packet */
					if (mPacketData[PACKET_NUMBER_INDEX] != (u8)packets_received) {
						uart_if_send_byte(NAK);
					} else {
						if (packets_received == 0) { /* File name packet */
							if (mPacketData[PACKET_DATA_INDEX] != 0) { /* File name extraction */
								i = 0;
								file_ptr = mPacketData + PACKET_DATA_INDEX;

								/* File size extraction */
								while ((*file_ptr != 0) && (i < FILE_NAME_LENGTH)) {
									mRecvFileName[i++] = *file_ptr++;
								}
								mRecvFileName[i++] = '\0';

								i = 0;
								file_ptr++;
								while ((*file_ptr != ' ') && (i < FILE_SIZE_LENGTH)) {
									file_size_str[i++] = *file_ptr++;
								}
								file_size_str[i++] = '\0';
								ytool_str2int(file_size_str, &filesize);

								/* Test the size of the image to be sent */
								/* Image size is greater than Flash size */
								if (*recv_filesize > (part_capacity + 1)) { /* End session */
									uart_if_send_byte(CAN);
									recv_result = YMODEM_STATUS_LIMIT;
								}

								/* erase user application area */
								flash_if_erase(part_offset, part_offset + part_capacity);
								*recv_filesize = filesize;

								uart_if_send_byte(ACK);
								uart_if_send_byte(CRC16);
							} else { /* File header packet is empty, end session */
								uart_if_send_byte(ACK);
								file_done = 1;
								session_done = 1;
								break;
							}
						} else { /* Data packet */
							ramsource = (u8 *)&mPacketData[PACKET_DATA_INDEX];
							if (flash_if_program(flashdestination, (u8*)ramsource, packet_length) == FLASHIF_OK) {
								flashdestination += packet_length;
								uart_if_send_byte(ACK);
							} else { /* An error occurred while writing to Flash memory */
							  /* End session */
								uart_if_send_byte(CAN);
								uart_if_send_byte(CAN);
								recv_result = YMODEM_STATUS_DATA;
							}
						}
						packets_received++;
						session_begin = 1;
					}
					break; // default
				}
				break; // case STATUS_OK

			case STATUS_BUSY: /* Abort actually */
				uart_if_send_byte(CAN);
				uart_if_send_byte(CAN);
				recv_result = YMODEM_STATUS_ABORT;
				break; // case STATUS_BUSY

			default:
				if (session_begin > 0) {
					errors++;
				}

				if (errors > MAX_ERRORS) { /* Abort communication */
					uart_if_send_byte(CAN);
					uart_if_send_byte(CAN);
					session_done = 1;
					file_done = 1;
					recv_result = YMODEM_STATUS_LIMIT;
				} else {
					uart_if_send_byte(CRC16); /* Ask for a packet by sending 'C' */
					if (++retry_cnt >= RETRY_TIMES_MAX) { // force exit
						uart_if_send_byte(CAN);
						uart_if_send_byte(CAN);
						session_done = 1;
						file_done = 1;
						recv_result = YMODEM_STATUS_TIMEOUT;
					}
				}
				break; // default

			} // switch ReceivePacket
		}
	}

	return recv_result;
}

Ymodem_StatusTypeDef Ymodem_Transmit(u8* file_buffer, const u8* name_buffer, u32 file_size)
{
	u32 errors = 0, ack_recpt = 0, size = 0, pkt_size;
	u8* p_file_buffer;
	Ymodem_StatusTypeDef send_result = YMODEM_STATUS_OK;
	u32 blk_number = 1, blk_max = file_size / PACKET_1K_SIZE + 1;
	u8 a_rx_ctrl[2];
	u8 i;

#ifdef FORCE_YMODEM_128
	blk_max = file_size / PACKET_SIZE + 1;
#endif

#ifdef FORCE_VERIFY_CRC16
	u32 temp_crc;
#else
	u8 temp_chksum;
#endif

	PrepareIntialPacket(mPacketData, name_buffer, file_size);
	logi("[YMODEM] Sending Initial Packet...\r\n");

	while ((!ack_recpt) && (send_result == YMODEM_STATUS_OK)) { /* Send Packet */
		uart_if_send_timeout(&port_uart, &mPacketData[PACKET_START_INDEX], PACKET_SIZE + PACKET_HEADER_SIZE);

#ifdef FORCE_VERIFY_CRC16
		temp_crc = Cal_CRC16(&mPacketData[PACKET_DATA_INDEX], PACKET_SIZE);
		uart_if_send_byte(temp_crc >> 8);
		uart_if_send_byte(temp_crc & 0xFF);
#else
		temp_chksum = CalcChecksum(&mPacketData[PACKET_DATA_INDEX], PACKET_SIZE);
		uart_if_send_byte(temp_chksum);
#endif

		if (uart_if_recv_timeout(&port_uart, &a_rx_ctrl[0], 1, NAK_TIMEOUT) == STATUS_OK) {
			if (a_rx_ctrl[0] == ACK) {
				ack_recpt = 1;
			} else if (a_rx_ctrl[0] == CAN) {
				if ((uart_if_recv_timeout(&port_uart, &a_rx_ctrl[0], 1, NAK_TIMEOUT) == STATUS_OK) && (a_rx_ctrl[0] == CAN)) {
					ytool_delay(2);
					uart_if_flush_recvd(&port_uart);
					send_result = YMODEM_STATUS_ABORT;
				}
			}
		} else {
			errors++;
		}

		if (errors >= MAX_ERRORS) {
			send_result = YMODEM_STATUS_ERROR;
		}
	}

	p_file_buffer = file_buffer;
	size = file_size;

	while ((size) && (send_result == YMODEM_STATUS_OK)) {
		PreparePacket(p_file_buffer, mPacketData, blk_number, size);
		ack_recpt = 0;
		a_rx_ctrl[0] = 0;
		errors = 0;

		while ((!ack_recpt) && (send_result == YMODEM_STATUS_OK)) {
#ifdef FORCE_YMODEM_128
			pkt_size = PACKET_SIZE;
#else
			if (size >= PACKET_1K_SIZE) {
				pkt_size = PACKET_1K_SIZE;
			} else {
				pkt_size = PACKET_SIZE;
			}
#endif

			uart_if_send_timeout(&port_uart, &mPacketData[PACKET_START_INDEX], pkt_size + PACKET_HEADER_SIZE);
			logi("[YMODEM] send packet %d \r\n", blk_number);

#ifdef FORCE_VERIFY_CRC16
			temp_crc = Cal_CRC16(&mPacketData[PACKET_DATA_INDEX], pkt_size);
			uart_if_send_byte(temp_crc >> 8);
			uart_if_send_byte(temp_crc & 0xFF);
#else
			temp_chksum = CalcChecksum(&mPacketData[PACKET_DATA_INDEX], pkt_size);
			uart_if_send_byte(temp_chksum);
#endif

			if ((uart_if_recv_timeout(&port_uart, &a_rx_ctrl[0], 1, NAK_TIMEOUT) == STATUS_OK) && (a_rx_ctrl[0] == ACK)) {
				ack_recpt = 1;
				if (size > pkt_size) {
					p_file_buffer += pkt_size;
					size -= pkt_size;

					if (blk_number > blk_max) {
						send_result = YMODEM_STATUS_LIMIT;
					} else {
						blk_number++;
					}
				} else {
					p_file_buffer += pkt_size;
					size = 0;
				}
			} else {
				errors++;
			}

			if (errors >= MAX_ERRORS) {
				send_result = YMODEM_STATUS_ERROR;
			}
		}
	}

	/* End Of Transmission */
	ack_recpt = 0;
	a_rx_ctrl[0] = 0x00;
	errors = 0;
	while ((!ack_recpt) && (send_result == YMODEM_STATUS_OK)) {
		uart_if_send_byte(EOT);

		if (uart_if_recv_timeout(&port_uart, &a_rx_ctrl[0], 1, NAK_TIMEOUT) == STATUS_OK) {
			if (a_rx_ctrl[0] == ACK) {
				ack_recpt = 1;
			} else if (a_rx_ctrl[0] == CAN) {
				if ((uart_if_recv_timeout(&port_uart, &a_rx_ctrl[0], 1, NAK_TIMEOUT) == STATUS_OK) && (a_rx_ctrl[0] == CAN)) {
					ytool_delay(2);
					uart_if_flush_recvd(&port_uart);
					send_result = YMODEM_STATUS_ABORT;
				}
			}
		} else {
			errors++;
		}

		if (errors >= MAX_ERRORS) {
			send_result = YMODEM_STATUS_ERROR;
		}
	}

	if (send_result == YMODEM_STATUS_OK) {
		mPacketData[PACKET_START_INDEX] = SOH;
		mPacketData[PACKET_NUMBER_INDEX] = 0;
		mPacketData[PACKET_CNUMBER_INDEX] = 0xFF;
		for (i = PACKET_DATA_INDEX; i < (PACKET_SIZE + PACKET_DATA_INDEX); i++) {
			mPacketData[i] = 0x00;
		}

		uart_if_send_timeout(&port_uart, &mPacketData[PACKET_START_INDEX], PACKET_SIZE + PACKET_HEADER_SIZE);

#ifdef FORCE_VERIFY_CRC16
		temp_crc = Cal_CRC16(&mPacketData[PACKET_DATA_INDEX], PACKET_SIZE);
		uart_if_send_byte(temp_crc >> 8);
		uart_if_send_byte(temp_crc & 0xFF);
#else
		temp_chksum = CalcChecksum(&mPacketData[PACKET_DATA_INDEX], PACKET_SIZE);
		uart_if_send_byte(temp_chksum);
#endif

		if (uart_if_recv_timeout(&port_uart, &a_rx_ctrl[0], 1, NAK_TIMEOUT) == STATUS_OK) {
			if (a_rx_ctrl[0] == CAN) {
				ytool_delay(2);
				uart_if_flush_recvd(&port_uart);
				send_result = YMODEM_STATUS_ABORT;
			}
		}
	}

	return send_result;
}

#endif /* !CFG_OTA */
