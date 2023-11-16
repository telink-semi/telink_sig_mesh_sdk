/********************************************************************************************************
 * @file	mesh_cdtp_vs.c
 *
 * @brief	for TLSR chips
 *
 * @author	Mesh Group
 * @date	2023
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#include "../../../ble_lt_mesh/proj_lib/sig_mesh/app_mesh.h"
//#include "third_party/zlib/zlib.h"
#include "./hw_fun.h"
#include "../../../ble_lt_mesh/proj_lib/cdtp/audio_otp.h"
#include "./mesh_cdtp_vs.h"


#define APPS_COC_SEND_INTERVAL_US     			(20*1000)  // us // sometimes failed if 10ms.

static cdtp_read_write_handle_t CDTP_rw_handle = {0};
static u8  CDTP_object_data[CDTP_OBJECT_SIZE_MAX]; 	// compress data. only one object.
static u8  CDTP_object_uncompress_data[CDTP_OBJECT_UNCOMPRESS_MAX];
static u8  CDTP_object_ready = 0; 					// ready for reading

const char CDTP_UNCOMPRESS_FILE_NAME[] = {"cdtp_rx_uncompress.json"};
const char CDTP_JSON_GET_FROM_RAM_FILE_NAME[] = {"cdtp_tx_uncompress_get_from_ram_temp.json"};

#if 1
typedef unsigned char           uint8_t;
typedef unsigned short          uint16_t;
typedef unsigned int			uint32_t;

static const uint32_t crc32_tab[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

static uint32_t soft_crc32_telink(const void* buf, size_t size, uint32_t crc)
{
	const uint8_t* p;

	p = (uint8_t*)buf;
	crc = crc ^ ~0U;

	while (size--)
		crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);

	return crc ^ ~0U;
}
#endif

int OTS_send_date2gateway(u8 *data, u16 len)
{
	int err = BLE_SUCCESS;
	cdtp_gw_ots_send_data_t ots_send_data = {0};
	if(len > sizeof(ots_send_data.data)){
		LOG_MSG_ERR(TL_LOG_GATEWAY,0, 0,"OTS send data size too much: %d", len);	
		return GAP_ERR_INVALID_PARAMETER;
	}
	
	ots_send_data.ini_type = HCI_CMD_GATEWAY_CTL;
	ots_send_data.hci_cmd = HCI_GATEWAY_CMD_OTS_TX;
	ots_send_data.oacp_opcode = OTS_OACP_OPCODE_USER_DATA_TX;
	ots_send_data.len = len;
	memcpy(ots_send_data.data, data, len);

	int send_len = OFFSETOF(cdtp_gw_ots_send_data_t, data) + len;
 	LOG_MSG_INFO (TL_LOG_GATEWAY, (u8 *)&ots_send_data, send_len, "OTS send date to gateway: ", 0);
	WriteFile_host_handle((u8 *)&ots_send_data, send_len);
	
	return err;
}

int OTS_send_checksum2gateway(u32 crc32)
{
	int err = BLE_SUCCESS;
	cdtp_gw_ots_send_checksum_t ots_send_checksum = {0};
	
	ots_send_checksum.ini_type = HCI_CMD_GATEWAY_CTL;
	ots_send_checksum.hci_cmd = HCI_GATEWAY_CMD_OTS_TX;
	ots_send_checksum.oacp_opcode = OTS_OACP_OPCODE_USER_CHECKSUM_TX;
	ots_send_checksum.len = sizeof(ots_send_checksum.checksum);
	ots_send_checksum.checksum = crc32;

 	LOG_MSG_INFO (TL_LOG_GATEWAY, 0, 0, "OTS send checksum to gateway: 0x%08x", crc32);
	WriteFile_host_handle((u8 *)&ots_send_checksum, sizeof(ots_send_checksum));
	
	return err;
}

int OTS_send_object_size2gateway(u32 current_size, u32 max_size)
{
	int err = BLE_SUCCESS;
	cdtp_gw_ots_send_object_size_t ots_send_obj_size = { 0 };

	ots_send_obj_size.ini_type = HCI_CMD_GATEWAY_CTL;
	ots_send_obj_size.hci_cmd = HCI_GATEWAY_CMD_OTS_TX;
	ots_send_obj_size.oacp_opcode = OTS_OACP_OPCODE_USER_TX_OBJECT_SIZE;
	ots_send_obj_size.len = sizeof(ots_send_obj_size.size);
	ots_send_obj_size.size.current_size = current_size;
	ots_send_obj_size.size.max_size = max_size;

	LOG_MSG_INFO(TL_LOG_GATEWAY, 0, 0, "OTS send object size to gateway, current size: %d(HEX:0x%x), max size: %d", current_size, current_size, max_size);
	WriteFile_host_handle((u8*)&ots_send_obj_size, sizeof(ots_send_obj_size));

	return err;
}

int OTS_send_ots_gatt_adv_onoff2gateway(u8 onoff) // or send INI command.
{
	cdtp_gw_ots_gatt_adv_onoff_t ots_send_adv_onoff = { 0 };

	ots_send_adv_onoff.ini_type = HCI_CMD_GATEWAY_CTL;
	ots_send_adv_onoff.hci_cmd = HCI_GATEWAY_CMD_OTS_TX;
	ots_send_adv_onoff.oacp_opcode = OTS_OACP_OPCODE_USER_ADV_ONOFF;
	ots_send_adv_onoff.len = sizeof(ots_send_adv_onoff.onoff);
	ots_send_adv_onoff.onoff = onoff;

	LOG_MSG_INFO(TL_LOG_GATEWAY, 0, 0, "OTS send gatt adv %s", (1 == onoff) ? "on" : "off");
	WriteFile_host_handle((u8*)&ots_send_adv_onoff, sizeof(ots_send_adv_onoff));

	return BLE_SUCCESS;
}

static u32 cdtp_tick_inputdb = 0;
static u32 cdtp_trigger_time_s = 6;

void CDTP_rw_handle_init()
{
	memset(&CDTP_rw_handle, 0, sizeof(CDTP_rw_handle)); // init
}

void CDTP_inputdb_trigger()
{
	LOG_MSG_INFO(TL_LOG_GATEWAY, 0, 0, "CDTP inputdb trigger, and waiting for crc from app in %d seconds", cdtp_trigger_time_s);
	#if 0 // no need factory reset first.
	//LOG_MSG_INFO(TL_LOG_GATEWAY, 0, 0, "step 1: factory reset and delete JSON ...", 0);
	//gateway_VC_factory_reset_and_del_JSON();
	#endif
	cdtp_tick_inputdb = clock_time() | 1;
}

void CDTP_inputdb_check_and_proc()
{
	if (cdtp_tick_inputdb && clock_time_exceed(cdtp_tick_inputdb, cdtp_trigger_time_s * 1000 * 1000)) { // waiting for factory reset complete
		cdtp_tick_inputdb = 0;
		int err = -1;
		if(CDTP_rw_handle.cal_crc_get_flag){
			if(CDTP_rw_handle.cal_crc_rx == CDTP_rw_handle.cal_crc_tx){
				err = 0;
				LOG_MSG_INFO(TL_LOG_GATEWAY, 0, 0, "step 2: enable new json ...", 0);
				CdtpInputDbFromJSON(CDTP_UNCOMPRESS_FILE_NAME); // CdtpInputDb("output_json.json"); // 
				// should not run here, because has reboot EXE before.
			}else{
				LOG_MSG_ERR(TL_LOG_GATEWAY, 0, 0, "failed to enable new json file due to checking checksum error", 0);
			}
		}else{
			LOG_MSG_ERR(TL_LOG_GATEWAY, 0, 0, "failed to enable new json file due to no checksum to check", 0);
		}

		if(err){
			CDTP_rw_handle_init();
		}
	}
}

u32 CDTP_get_buff_of_json()
{
	// TODO: check whether it is CDTP getting JSON from app now, if yes, return error, because both TX/RX are using CDTP_object_data and CDTP_object_uncompress_data.

#if 1
	CdtpOutputDbToJSON(CDTP_JSON_GET_FROM_RAM_FILE_NAME);
	u32 len_read = vc_file_read_all(CDTP_object_uncompress_data, sizeof(CDTP_object_uncompress_data), CDTP_JSON_GET_FROM_RAM_FILE_NAME);
#else
	//CdtpOutputDbToJSON(CDTP_JSON_GET_FROM_RAM_FILE_NAME);
	u32 len_read = vc_file_read_all(CDTP_object_uncompress_data, sizeof(CDTP_object_uncompress_data), "mesh_pts.json");//CDTP_JSON_GET_FROM_RAM_FILE_NAME
#endif

	unsigned long len_compress = sizeof(CDTP_object_data);
	int err = -1; // TODO: import zlib. // compress(CDTP_object_data, &len_compress, CDTP_object_uncompress_data, len_read);
	if( 0 == err){
		CDTP_object_ready = 1;
		LOG_MSG_INFO(TL_LOG_GATEWAY,0, 0,"CDTP get buffer of json success", 0); 
	}else{
		len_compress = 0;
		LOG_MSG_ERR(TL_LOG_GATEWAY,0, 0,"CDTP fail to get buffer of json", 0);	
	}

	return (u32)len_compress;
}

void OTS_read_object_len_cb(u8 *pData, u16 dataLen)
{
	cdtp_read_write_handle_t *p_cdtp = &CDTP_rw_handle;

	CDTP_rw_handle_init(); // set read_len = 0;
	OtsRWOpcodePar_t *p_rw = (OtsRWOpcodePar_t *)pData;
	p_cdtp->last_pos = p_rw->offset;
	if(p_rw->offset != 0){
		LOG_MSG_ERR(TL_LOG_GATEWAY,0, 0,"error ? should read from 0, last_pos: %d", p_cdtp->last_pos);	
	}
	
	p_cdtp->read_len = p_rw->len;
	p_cdtp->tick = clock_time();
	LOG_MSG_INFO(TL_LOG_GATEWAY,0, 0,"OTS read object len: %d", p_cdtp->read_len);	
}

void OTS_write_object_len_cb(u8 *pData, u16 dataLen)
{
	cdtp_read_write_handle_t *p_cdtp = &CDTP_rw_handle;
	CDTP_rw_handle_init(); // set read_len = 0;
	CDTP_object_ready = 0;
	
	OtsRWOpcodePar_t *p_rw = (OtsRWOpcodePar_t *)pData;
	p_cdtp->last_pos = p_rw->offset;
	if(p_cdtp->last_pos != 0){
		LOG_MSG_ERR(TL_LOG_GATEWAY, 0, 0,"error, must write from 0, last_pos: %d", p_cdtp->last_pos);	
	}
	
	p_cdtp->write_len = p_rw->len;
	LOG_MSG_INFO(TL_LOG_GATEWAY, 0, 0,"OTS write object len: %d", p_cdtp->write_len);	
}

int OTS_coc_datacb(u8 *pData, u16 dataLen)
{
	int err = 0;
	cdtp_read_write_handle_t *p_cdtp = &CDTP_rw_handle;
	LOG_MSG_INFO(TL_LOG_GATEWAY, pData, dataLen,"OTS write DATA CALLBACK, len: %d", dataLen);
	if(0 == p_cdtp->write_len){
		LOG_MSG_ERR(TL_LOG_GATEWAY, 0, 0,"no receive OTS_OACP_OPCODE_WRITE before", 0);
		return -1;
	}
	
	/****************more data***************/
	if(p_cdtp->last_pos + dataLen < sizeof(CDTP_object_data)){ // the last byte is always 0.
		memcpy((u8*)(CDTP_object_data + p_cdtp->last_pos), pData, dataLen);
		p_cdtp->last_pos += dataLen;
		LOG_MSG_INFO(TL_LOG_GATEWAY, 0, 0,"coc data total reveived length: %d", p_cdtp->last_pos);	
	}else{
		LOG_MSG_ERR(TL_LOG_GATEWAY, 0, 0,"coc data rx overflow, max rx size: %d, last_pos: %d", sizeof(CDTP_object_data), p_cdtp->last_pos);
		err = -2;
	}

	if(p_cdtp->last_pos == p_cdtp->write_len){
		// receive OK
		u32 crc = soft_crc32_telink(CDTP_object_data, p_cdtp->write_len, 0);
		LOG_MSG_INFO(TL_LOG_GATEWAY, 0, 0,"CDTP rx data all, checksum is: 0x%08x", crc); 
		if(1){ // TODO: CRC ok
			unsigned long len_uncomp = sizeof(CDTP_object_uncompress_data);
			int err = -1; // TODO: import zlib. // uncompress(CDTP_object_uncompress_data, &len_uncomp, CDTP_object_data, p_cdtp->write_len);
			if(0 == err){ // Z_OK == 0
				vc_file_write_with_new_or_rewrite(CDTP_object_uncompress_data, len_uncomp, CDTP_UNCOMPRESS_FILE_NAME);
				// input json
				if(ble_module_id_is_gateway()){
					p_cdtp->cal_crc_tx = crc;
					OTS_send_checksum2gateway(crc);
					CDTP_inputdb_trigger();
				}else {
					LOG_MSG_ERR(TL_LOG_GATEWAY, 0, 0, "not gateway mode", 0);
				}
			}else{
				LOG_MSG_ERR(TL_LOG_GATEWAY, 0, 0, "uncompress failed", 0);
			}
		}
	}

	return err;
}

u32 OTS_get_crc_data()
{
	return CDTP_rw_handle.cal_crc_tx;
}

void CDTP_send_data()
{
	cdtp_read_write_handle_t *p_cdtp = &CDTP_rw_handle;
	if(p_cdtp->read_len && CDTP_object_ready){
		u32 interval_cdtp_us = APPS_COC_SEND_INTERVAL_US;
		if(0 == p_cdtp->last_pos){
			// if send data at once, PTS will not receive data. PTS delay 100ms.
			interval_cdtp_us += 200*1000;
		}
		
		if(clock_time_exceed(p_cdtp->tick, interval_cdtp_us)){
			p_cdtp->tick = clock_time();
			u16 read_len = min(p_cdtp->read_len - p_cdtp->last_pos, CDTP_READ_LEN_ONCE_MAX);
			LOG_MSG_INFO(TL_LOG_GATEWAY, 0, 0,"CDTP send data offset: %d, len: %d", p_cdtp->last_pos, read_len); 
			int err = OTS_send_date2gateway((u8*)(CDTP_object_data + p_cdtp->last_pos), read_len);
			if(err == BLE_SUCCESS){
				p_cdtp->last_pos += read_len;
				if(p_cdtp->last_pos >= p_cdtp->read_len){
					// complete
					u32 crc = soft_crc32_telink(CDTP_object_data, p_cdtp->read_len, 0);
					CDTP_rw_handle_init(); // init to clear p_cdtp->read_len
					p_cdtp->cal_crc_tx = crc; // only keep cal
					LOG_MSG_INFO(TL_LOG_GATEWAY, 0, 0,"CDTP send data tx all, checksum is: 0x%08x", crc);
					OTS_send_checksum2gateway(crc);
				}
			}else{
				LOG_MSG_ERR(TL_LOG_GATEWAY, 0, 0,"Send Data error", 0);	
			}
		}
	}
}


void CDTP_ots_rx_handle(u8 *rx_buff, int buff_len)
{
	cdtp_gw_ots_rx_rsp_t *p_rx = (cdtp_gw_ots_rx_rsp_t *)rx_buff;
	if((buff_len < OFFSETOF(cdtp_gw_ots_rx_rsp_t, data))
	|| (buff_len != p_rx->len + OFFSETOF(cdtp_gw_ots_rx_rsp_t, data))){
		LOG_MSG_ERR(TL_LOG_COMMON, rx_buff, buff_len,"OTS rx len check error, buff", 0);
		return ;
	}
	
	LOG_MSG_INFO(TL_LOG_COMMON, &p_rx->oacp_opcode, buff_len - OFFSETOF(cdtp_gw_ots_rx_rsp_t, oacp_opcode),"OTS rx opcode: 0x%02x, buff", p_rx->oacp_opcode);

	if(OTS_OACP_OPCODE_WRITE == p_rx->oacp_opcode){
		OTS_write_object_len_cb(p_rx->data, p_rx->len);
	}else if(OTS_OACP_OPCODE_USER_DATA_RX == p_rx->oacp_opcode){
		OTS_coc_datacb(p_rx->data, p_rx->len);
	}else if(OTS_OACP_OPCODE_READ == p_rx->oacp_opcode){
		OTS_read_object_len_cb(p_rx->data, p_rx->len);
	}else if(OTS_OACP_OPCODE_CALC_CHECKSUM == p_rx->oacp_opcode){
		// have sent checksum right after receive all data.
		if(p_rx->len == sizeof(cdtp_ots_get_checksum_t)){
			cdtp_ots_get_checksum_t *p_cs = (cdtp_ots_get_checksum_t *)p_rx->data;
			CDTP_rw_handle.cal_crc_get_flag = 1;
			CDTP_rw_handle.cal_crc_rx = p_cs->checksum;
		}
		
		LOG_MSG_INFO(TL_LOG_GATEWAY, p_rx->data, p_rx->len,"OTS get checksum, len: %d, data", p_rx->len);
	}else if (OTS_OACP_OPCODE_USER_TX_OBJECT_SIZE == p_rx->oacp_opcode) {
		u32 current_len = CDTP_get_buff_of_json();
		OTS_send_object_size2gateway(current_len, sizeof(CDTP_object_data));
	}else if (OTS_OACP_OPCODE_USER_SMP_TK_DISPLAY == p_rx->oacp_opcode) {
		u32 pincode = 0;
		memcpy(&pincode, p_rx->data, min(sizeof(pincode), p_rx->len));
		LOG_MSG_INFO(TL_LOG_GATEWAY, 0, 0,"SMP LTK display(Pin Code): %06d", pincode);
	}
}

void CDTP_vs_loop()
{
	//mesh_cdtp_test();
	CDTP_send_data();
	CDTP_inputdb_check_and_proc();

#if 0 // test
	int test_key = 0;
	if (test_key) {
		CDTP_get_buff_of_json();
	}
#endif
}

#if 0 // test deflate/inflate function
#define OTS_OBJECT_SIZE_MAX								(1500)

static u8  gAppsCocData[OTS_OBJECT_SIZE_MAX]; // only one object.

const u8 CDTP_JSON_INIT_TEST[] = { // 0x05c5 for PTS, check sum is 0x9b1aa
	0x78,0x9c,0xed,0x58,0xdb,0x8e,0xdb,0x36,0x10,0x7d,0x2f,0xd0,0x7f,0x10,0xd4,0x3e,
	0x46,0x6b,0x92,0xa2,0x44,0xc9,0x6f,0x49,0x0a,0x14,0x41,0xdb,0xb4,0xc8,0x6e,0x5e,
	0x52,0x14,0x05,0x2f,0x43,0x5b,0x8d,0x2c,0x09,0xba,0xec,0x6e,0x10,0xe4,0xdf,0x3b,
	0x92,0x7c,0x91,0x2d,0x79,0x6d,0x27,0xdb,0x34,0x08,0xb2,0xc0,0xc2,0x36,0x39,0x9c,
	0x39,0x1c,0xce,0x1c,0x0e,0xe7,0xfd,0xf7,0xdf,0x39,0x8e,0xfb,0x63,0xa5,0x97,0xb0,
	0x92,0xee,0xdc,0x5d,0xd6,0x75,0x31,0x9f,0xcd,0xfe,0xa9,0xf2,0xcc,0xeb,0x07,0xaf,
	0xf2,0x72,0x31,0x33,0xa5,0xb4,0xb5,0x47,0xf8,0xac,0x1f,0xfb,0xc1,0x7d,0xd2,0xad,
	0x4b,0xcc,0x6e,0xc9,0xdd,0xdd,0xdd,0x95,0x4a,0x1b,0xa8,0xf3,0xbc,0x5e,0x5e,0xe9,
	0x7c,0x35,0xab,0x0a,0xd0,0x89,0x4d,0xb4,0xac,0x93,0x3c,0xab,0x66,0xb2,0xaa,0x92,
	0x45,0x06,0xc6,0xcb,0x9a,0x95,0x82,0xb2,0x9a,0xad,0xa0,0x5a,0x7a,0x45,0x99,0xdb,
	0x24,0x85,0x99,0x36,0x6a,0x63,0xb0,0x35,0xbe,0xb1,0x70,0x8b,0x82,0xb8,0x1a,0xcd,
	0xd0,0x2b,0x72,0x45,0xd6,0xa3,0xed,0xca,0xd7,0xaf,0x5f,0xfc,0x84,0xc3,0x8a,0xf8,
	0xc2,0x04,0xda,0x7a,0x4c,0x0a,0xf0,0x18,0x8b,0x03,0x4f,0x84,0x7e,0xe8,0x09,0xaa,
	0x54,0x60,0xad,0x12,0x81,0x84,0xc1,0xaa,0x97,0x72,0x05,0xb8,0xea,0x37,0xfc,0xea,
	0x3c,0xcf,0x33,0x9b,0x2c,0x9c,0xe7,0x69,0x02,0x59,0xed,0xbc,0x84,0xfa,0x2e,0x2f,
	0xdf,0xae,0x65,0xb3,0xdc,0x40,0xe5,0xce,0xff,0x6c,0x7f,0x38,0xce,0xfb,0xfe,0x03,
	0xc7,0xd7,0x56,0x83,0x38,0x20,0x34,0xb6,0xb1,0x47,0x89,0x31,0x9e,0xb2,0xbe,0xf1,
	0x88,0x44,0xab,0x71,0x44,0x04,0xd8,0x20,0x8a,0x74,0xa8,0x7b,0x4d,0xdd,0xaa,0x0a,
	0x74,0x53,0x26,0xf5,0x3b,0x5c,0xd9,0x7d,0x85,0xc1,0x1c,0xdc,0xeb,0xb4,0x31,0x80,
	0x9e,0xb4,0x32,0xad,0x60,0x37,0xa1,0x3b,0x78,0xcf,0xf3,0x55,0x91,0x42,0x0d,0xa3,
	0x69,0x48,0x61,0x85,0xb8,0x77,0x20,0xf7,0x80,0x76,0x22,0x2b,0xdc,0x44,0xba,0x2f,
	0x30,0x12,0xda,0x09,0xbe,0x68,0x0f,0x93,0xe0,0xdf,0x0e,0xdc,0x56,0x40,0x25,0x99,
	0x19,0xe9,0x71,0x9c,0xbf,0xc6,0x92,0x55,0xa3,0x2a,0x5d,0x26,0x0a,0xa6,0xc4,0xf7,
	0x07,0x3e,0x3c,0xb9,0x08,0x17,0xfd,0x6c,0xb8,0x86,0x3f,0xf7,0x55,0xb9,0x68,0x0f,
	0xee,0xdd,0x39,0xd9,0x1f,0x4d,0xf3,0x3e,0xc8,0x37,0x0e,0xdc,0x4d,0x6e,0x75,0xed,
	0xf4,0xb8,0x19,0xd4,0xbf,0xc0,0xbb,0x87,0x0e,0x6e,0xda,0x4a,0x53,0x18,0x59,0x6f,
	0x03,0x65,0x60,0xe3,0xc9,0x09,0x3d,0x3e,0x3f,0xa2,0xa8,0x2e,0x1b,0x78,0x18,0xab,
	0x2c,0x8a,0x03,0xac,0x83,0xc9,0x26,0xc3,0xe4,0xae,0xea,0xa7,0xc6,0x94,0x50,0x55,
	0xa3,0x53,0x72,0x75,0xc7,0x0f,0x24,0xb0,0xc3,0xc1,0x22,0x59,0x9f,0x27,0x1b,0x0c,
	0xde,0x26,0xe3,0x43,0x76,0x2d,0xc8,0x1a,0x73,0x05,0x15,0x0f,0x76,0xe5,0x22,0x61,
	0xdc,0x63,0x22,0xb1,0xc1,0x96,0xd0,0xff,0x77,0x7f,0xe4,0x77,0x50,0xe2,0xf0,0x66,
	0xf4,0xc3,0x00,0x46,0x59,0xa4,0xa8,0x5d,0x58,0x6b,0x07,0xda,0x0d,0xdc,0x26,0x1a,
	0x70,0x73,0x38,0x65,0x0d,0x07,0x1d,0x6b,0x15,0x1b,0x50,0x3a,0x66,0x11,0x68,0x01,
	0x9c,0xb7,0x47,0x69,0xb8,0x0c,0x98,0x59,0x1f,0xe8,0x46,0xe7,0x88,0x0e,0x48,0x4c,
	0x04,0xb1,0x84,0x21,0x09,0x90,0xd0,0x23,0x86,0xf8,0x1e,0x51,0x24,0xf2,0x08,0x25,
	0x01,0x0e,0x69,0x02,0x84,0x7f,0xa3,0x83,0x47,0xa4,0x03,0xfa,0x85,0xe2,0x6a,0x63,
	0xfd,0x32,0xaa,0x72,0x1c,0xfe,0x89,0x70,0x71,0x9a,0x4a,0xaa,0x84,0xa0,0x16,0x83,
	0x95,0x8b,0x28,0x96,0x4a,0x73,0x8e,0x21,0xc9,0xad,0x08,0x22,0x66,0x7c,0xe1,0x9e,
	0xda,0xe2,0x63,0x32,0xde,0x51,0x36,0xba,0x3c,0xfe,0x9c,0xf6,0xa0,0x8f,0x3b,0xd3,
	0x99,0xf0,0x05,0xa3,0x97,0xb9,0x73,0x4a,0x87,0xab,0x09,0x23,0x63,0x9f,0x8d,0xf5,
	0x14,0x8d,0x4a,0x93,0x6a,0x89,0x5a,0x46,0x9b,0x68,0xb9,0x73,0xc3,0x8b,0xad,0x42,
	0x3e,0x11,0xaf,0x3b,0xef,0x22,0xec,0xa9,0xd9,0xba,0x46,0xe2,0x72,0x28,0x13,0x53,
	0x93,0xba,0x04,0x83,0x19,0x9f,0xc8,0xd6,0xa7,0x0e,0x99,0x12,0x29,0xa1,0x2e,0x65,
	0x56,0xad,0x92,0x7a,0x1a,0x62,0x47,0x2b,0x4d,0xd6,0xce,0xfa,0x13,0xeb,0x3b,0x7c,
	0x35,0x94,0xb7,0xb2,0x85,0xe1,0x07,0x64,0x2c,0x72,0x98,0x1c,0xdd,0xa2,0x02,0xca,
	0x24,0x37,0x47,0x4d,0xf6,0xf5,0xdf,0xef,0xf6,0xba,0x86,0xa2,0x85,0x1e,0x1e,0xb1,
	0x8d,0xce,0xcb,0xd3,0xa6,0x8f,0x32,0x07,0x03,0x61,0xc2,0xfa,0xe1,0xd0,0x87,0x8b,
	0xe3,0x9a,0x9e,0x1d,0xd7,0x5f,0xdd,0x4d,0x7e,0xc4,0xc6,0x41,0x28,0x7e,0x3c,0x58,
	0xf6,0x48,0x7a,0x8e,0xed,0xf9,0x50,0xcf,0x79,0x15,0x8a,0x94,0x67,0x57,0x28,0xf4,
	0x3f,0xa9,0x50,0x86,0xc3,0xb6,0xc4,0xb7,0x87,0xd9,0x8f,0x0f,0x8c,0xfb,0x54,0xe2,
	0xfa,0x6d,0xbc,0x8f,0x4b,0x19,0x42,0xc2,0x61,0x3d,0x91,0xf5,0x4f,0x97,0x9b,0x6d,
	0xae,0x0f,0xd1,0xac,0x13,0x7c,0xcf,0xc2,0x2e,0xab,0x07,0x69,0x35,0x30,0x63,0xc0,
	0xca,0x26,0xad,0x6f,0x6e,0x7e,0x45,0x89,0x01,0xfb,0xf4,0xd0,0x5e,0x0d,0x58,0x65,
	0xc2,0x92,0x98,0xb6,0xe4,0xd3,0x69,0x4b,0xbb,0x2a,0x4c,0xa8,0x80,0x4a,0x74,0x30,
	0x96,0x4c,0x1a,0x38,0xfe,0xb3,0x28,0xa0,0x7e,0xa8,0x15,0x15,0xb1,0x11,0x7e,0x74,
	0xb2,0x0a,0x8b,0xb0,0xde,0x12,0xa4,0x7d,0x8a,0x11,0xe9,0x91,0x90,0x28,0x0f,0x8b,
	0xb2,0xc0,0xeb,0xea,0x2f,0x1f,0x19,0x38,0xde,0xab,0x39,0xbf,0x55,0x61,0xbd,0xf8,
	0xb7,0x2a,0xec,0x8b,0x7a,0x30,0x3e,0x62,0xf9,0xf4,0x70,0xf5,0xf4,0xf9,0x9d,0xf0,
	0x95,0xdc,0xb5,0x67,0xdc,0x3b,0x8f,0x7f,0xd9,0x9e,0x79,0xb9,0xe9,0xff,0xf9,0x72,
	0x3b,0xfb,0xce,0xea,0x29,0x77,0xdd,0x74,0x7b,0x06,0x52,0xb7,0x71,0xd0,0x56,0x31,
	0xd3,0x77,0x83,0x12,0x54,0x69,0xeb,0x2b,0x46,0xb9,0x66,0x42,0x1b,0xe4,0x72,0x3f,
	0x88,0xb4,0x8d,0x19,0xb3,0xc6,0x4a,0xbb,0xb9,0x1b,0xda,0x8f,0xde,0x45,0x2d,0xd2,
	0xdb,0xa4,0x6d,0x1e,0x42,0x39,0xd5,0xc9,0x1b,0x4c,0xaf,0x9b,0x82,0xcf,0xd2,0x06,
	0xde,0x38,0x5d,0x57,0x52,0xdb,0x85,0xee,0xda,0x82,0x03,0xc4,0x1f,0xd7,0xfa,0x93,
	0x69,0x17,0xe6,0x60,0x5e,0xf7,0xa7,0xf5,0x4a,0x66,0x0b,0x78,0x20,0x2e,0xd0,0x97,
	0xc7,0xaa,0x95,0x6e,0x7e,0x99,0x2c,0x96,0x3b,0x81,0xae,0xa7,0x71,0x22,0x14,0x37,
	0x00,0x7e,0x2e,0xf3,0xa6,0xb8,0xc8,0xbc,0x1e,0xf1,0xfb,0x81,0x79,0x0b,0xe7,0x9b,
	0xbf,0xd6,0x90,0xc1,0x29,0xf3,0x36,0x29,0xab,0xba,0x93,0x6c,0x5f,0x4d,0x63,0x0e,
	0x77,0x53,0xb9,0x37,0xcf,0xa7,0x59,0x63,0x1c,0x0c,0x87,0xec,0xb1,0x8b,0x83,0xac,
	0x3f,0xfc,0xeb,0x46,0xa1,0x8c,0xb3,0x7f,0xd5,0x8e,0x29,0xc5,0x5d,0x25,0xd9,0xf5,
	0x03,0x85,0x43,0x9d,0x60,0xfc,0xd4,0x72,0x55,0xe0,0x24,0xc3,0x57,0xa4,0x87,0xf5,
	0x07,0x13,0x37,0x34,0x9a,0x93,0x70,0x4e,0x82,0x37,0xc3,0x4c,0x5c,0xca,0x0a,0xf6,
	0x74,0xbf,0xed,0x9b,0x51,0x9a,0x19,0xe9,0x73,0x0e,0x96,0x13,0x61,0xac,0x08,0x03,
	0x6d,0x02,0x83,0x6f,0x7a,0x66,0xb9,0x05,0x75,0xbc,0x0c,0x3a,0xdc,0x09,0x63,0xe3,
	0x9d,0x0c,0x48,0xed,0x93,0xb6,0xe2,0xf3,0x89,0xad,0xd0,0xc3,0xad,0x44,0x21,0x53,
	0xc0,0x42,0xa3,0xa8,0x22,0x4a,0xb5,0xcd,0x35,0xcc,0x55,0x3f,0xe0,0xc0,0xe3,0xc8,
	0xca,0x30,0x1c,0xa8,0xc8,0x53,0xd3,0x67,0x3a,0xe6,0xb4,0x0c,0x34,0x51,0xbe,0x34,
	0x94,0x50,0x6d,0x84,0x8d,0x8c,0x01,0x29,0x84,0x4f,0x54,0x10,0xb3,0x78,0x22,0xd3,
	0x0f,0xd9,0x76,0xe4,0x92,0xa7,0x9d,0x80,0x73,0x40,0x70,0x0a,0xeb,0x55,0xf3,0xb2,
	0x0b,0x8c,0xbd,0x63,0x18,0xdd,0x56,0xeb,0xdd,0x28,0xe6,0x73,0x46,0x7c,0x1f,0xcf,
	0x03,0x3f,0xa8,0xa4,0xc8,0x49,0x9c,0x61,0xd1,0x69,0x18,0x0f,0x71,0xf4,0xd4,0xc1,
	0xec,0x50,0xb0,0xf3,0x51,0xb0,0x43,0x14,0x34,0xf2,0x45,0x1c,0x80,0xe0,0x10,0x10,
	0x63,0x95,0x1f,0x47,0xa1,0xe4,0x4a,0xf9,0x61,0x14,0x00,0x10,0x75,0x01,0x0a,0x7e,
	0x0c,0xc5,0x30,0x48,0x46,0x6f,0xb0,0x35,0x0c,0x3c,0xd3,0x58,0xc7,0x24,0x12,0x42,
	0x73,0xca,0x03,0xa9,0x14,0xf7,0x11,0x10,0x44,0xf8,0xd3,0x32,0xc3,0xcf,0x86,0x41,
	0x83,0x0b,0x60,0xb0,0xd1,0xa1,0xe0,0x83,0x41,0x48,0x61,0x81,0x84,0x2d,0xfb,0xda,
	0x36,0xbc,0x48,0xac,0xc1,0x00,0xb5,0x98,0x3f,0x18,0x2e,0x13,0x21,0x76,0x72,0xcd,
	0x38,0xc4,0x16,0x2d,0x79,0x3e,0x10,0x61,0x1d,0xb9,0xfe,0x1d,0xfb,0xd1,0x30,0xc0,
	0xb6,0x0d,0xa0,0xd3,0x0d,0xba,0x41,0x2e,0xc9,0x12,0xef,0x9d,0xbd,0x9e,0x3a,0x39,
	0xe5,0xcc,0xde,0x7a,0xd7,0xb5,0x9a,0xb2,0x7e,0x30,0xf1,0x29,0x16,0xf8,0x31,0x0b,
	0xfc,0x5c,0x0b,0x03,0xa7,0x1e,0x67,0x98,0x20,0x9a,0xfb,0x64,0xcd,0x30,0x6e,0xd5,
	0xf2,0xfd,0xc6,0xf7,0x9b,0xcb,0x5d,0x96,0x6d,0xe7,0x6b,0x5b,0x27,0xa1,0xda,0x7f,
	0x01,0xbd,0x9e,0x21,0xb2,
};


static u8  gAppsCocData_uncompress[100*1024];
static u8  gAppsCocData_compress[8*1024];
static unsigned long gApps1_LenUncomp,gApps2_LenComp;
static volatile int gApps1_errUncomp,gApps2_errcomp,gApps2;

void mesh_cdtp_test(void)
{
	if(sizeof(gAppsCocData) > sizeof(CDTP_JSON_INIT_TEST)){
		memcpy(gAppsCocData, CDTP_JSON_INIT_TEST, sizeof(CDTP_JSON_INIT_TEST)); 	// init tx data
	}

	gApps1_LenUncomp = sizeof(gAppsCocData_uncompress);
	gApps1_errUncomp = uncompress(gAppsCocData_uncompress, &gApps1_LenUncomp, CDTP_JSON_INIT_TEST, sizeof(CDTP_JSON_INIT_TEST));

	#if 1 // write/read json test
	char JSON_FILE_NAME_TEST[] = {"test_uncompress.json"};
	vc_file_write_with_new_or_rewrite(gAppsCocData_uncompress, gApps1_LenUncomp, JSON_FILE_NAME_TEST);
	u8 data_uncompress2[sizeof(gAppsCocData_uncompress)] = { 0 };
	memcpy(data_uncompress2, gAppsCocData_uncompress, gApps1_LenUncomp);
	memset(gAppsCocData_uncompress, 0, sizeof(gAppsCocData_uncompress));
	u32 len_read = vc_file_read_all(gAppsCocData_uncompress, sizeof(gAppsCocData_compress), JSON_FILE_NAME_TEST);
	gApps1_LenUncomp = len_read;
	#endif
	
	gApps2_LenComp = sizeof(gAppsCocData_compress);
	gApps2_errcomp = compress(gAppsCocData_compress, &gApps2_LenComp, gAppsCocData_uncompress, gApps1_LenUncomp);
}
#endif

