/********************************************************************************************************
 * @file	error.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#ifndef _ERROR_H
#define _ERROR_H

/** @addtogroup  Digger0_Host_API
 *  @{
 */

/** @addtogroup  Error_Code
 *  @{
 */


/** @defgroup Error_Module_Base
 *  @{
 */

#define ERR_NONE            0x00000000
#define ERR_SERIAL_BASE     0x00010000
#define ERR_BOOT_ROM_BASE   0x00020000
#define ERR_TIMER_BASE      0x00030000
#define ERR_CRC_BASE        0x00040000
#define ERR_AES_BASE        0x00050000

/*@} end of group Error_Module_Base */


/** @defgroup Error_Detail_Code
 *  @{
 */
typedef enum digger0_err_code {

    ERR_SERIAL_START = ERR_SERIAL_BASE,
    ERR_SERIAL_INVALID_PORT,
    ERR_SERIAL_INVALID_BAUD_RATE,
    ERR_SERIAL_CONFIG_FAIL,
    ERR_SERIAL_SEND_OVERFLOW,
    ERR_SERIAL_RECV_OVERFLOW,
    ERR_SERIAL_SEND_FRAME,
    ERR_SERIAL_RECV_FRAME,
    ERR_SERIAL_RECV_PARITY,
    ERR_SERIAL_RECV_EMPTY,
    ERR_SERIAL_RECV_OVERRUN,
    ERR_SERIAL_END,


    ERR_BOOT_ROM_START = ERR_BOOT_ROM_BASE,
    ERR_BOOT_ROM_INVALID_PARAMETER,
    ERR_BOOT_ROM_INVALID_ADDR,
    ERR_BOOT_ROM_ENTER_CLI,
    ERR_BOOT_ROM_NO_RESPONSE,
    ERR_BOOT_ROM_MALLOC_FAIL,
    ERR_BOOT_ROM_END,

    ERR_TIMER_START = ERR_TIMER_BASE,
    ERR_TIMER_CREATE_FAIL,
    ERR_TIMER_START_FAIL,
    ERR_TIMER_SET_FAIL,
    ERR_TIMER_END,

    ERR_CRC_START = ERR_CRC_BASE,
    ERR_CRC_MODE,
    ERR_CRC_END,

    ERR_AES_START = ERR_AES_BASE,
    ERR_AES_INVALID_PARAMETER,
    ERR_AES_INVALID_KEY_LEN,
    ERR_AES_KEY_INIT_FAIL,
    ERR_AES_ENCRYPT_FAIL,
    ERR_AES_DECRYPT_FAIL,
    ERR_AES_INVALID_CBC_IV_LEN,
    ERR_AES_END,
    
} digger0_err_code_t;

/*@} end of group Error_Detail_Code */

/*@} end of group Error_Code  */

/*@} end of group Digger0_Host_API */


#endif /* _ERROR_H */

