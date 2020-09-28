/********************************************************************************************************
 * @file     certify_base_crypto.c 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/

#include "../../../proj_lib/ble/ll/ll.h"
#include "../../../proj_lib/ble/blt_config.h"
#include "../user_config.h"
#include "../../../proj_lib/sig_mesh/app_mesh.h"
#include "../../../proj_lib/mesh_crypto/sha256_telink.h"
#include "sha1_telink.h"
#include "certify_base_crypto.h"
#include "pem_der.h"
#include "asn_telink.h"
#if CERTIFY_BASE_ENABLE
	const char uri_base[]="https://mesh.example.com/oob?uuid=b09dc8\
47-5408-40cc-9c54-0fe8c87429e7&content=device-certificate&content=a\
bcd-metadata";

#define PEM_CERT_S                   "-----BEGIN CERTIFICATE-----"
#define PEM_CERT_E                   "-----END CERTIFICATE-----"

	const char pem_cert[]="-----BEGIN CERTIFICATE-----\
MIICtDCCAlqgAwIBAgICEAAwCgYIKoZIzj0EAwIwgacxCzAJBgNVBAYTAkZJMRAw\
DgYDVQQIDAdVdXNpbWFhMQ4wDAYDVQQHDAVFc3BvbzEYMBYGA1UECgwPRXhhbXBs\
ZSBDb21wYW55MRkwFwYDVQQLDBBFbWJlZGRlZCBEZXZpY2VzMR0wGwYDVQQDDBRl\
bWJlZGRlZC5leGFtcGxlLmNvbTEiMCAGCSqGSIb3DQEJARYTc3VwcG9ydEBleGFt\
cGxlLmNvbTAeFw0xODExMTYwNzM0NDZaFw0xOTExMTYwNzM0NDZaMIGTMQswCQYD\
VQQGEwJGSTEQMA4GA1UECAwHVXVzaW1hYTEOMAwGA1UEBwwFRXNwb28xGDAWBgNV\
BAoMD0V4YW1wbGUgQ29tcGFueTEZMBcGA1UECwwQRW1iZWRkZWQgRGV2aWNlczEt\
MCsGA1UEAwwkYjA5ZGM4NDctNTQwOC00MGNjLTljNTQtMGZlOGM4NzQyOWU3MFkw\
EwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEjQKXzLPnx2sVLg+wJeTnHjkpoPCdK4xF\
8Wi4fhYEHeRLAky4BjT80HBsJKgz7dsutXFRUQMWyYk+5LS8hfbeWaOBhzCBhDAJ\
BgNVHRMEAjAAMAsGA1UdDwQEAwIDCDAdBgNVHQ4EFgQUKLxCamjbY5ZwhXHkz8ly\
HOmLaBUwHwYDVR0jBBgwFoAUw37qVGoCbXzO9fSg0/Wo1Jgmo0owKgYUaYLhneSR\
6sDCg5mcqoP9jMPQ02cEEgQQTlQ0b3BGclp0RFFoVll3ZzAKBggqhkjOPQQDAgNI\
ADBFAiEAxVl0uxS3poJWmKs/Nfh9YHBoXSY4V/RRQ5rL7q8V+iECIHdn9b7Knijd\
6Yqet2uGkdyTi9mFVafBMkSzE8wtaaMl\
-----END CERTIFICATE-----";

#define PEM_EC_S                  "-----BEGIN EC PARAMETERS-----"
#define PEM_EC_E                  "-----END EC PARAMETERS-----"
		const char para_cert[] = "-----BEGIN EC PARAMETERS-----\
BggqhkjOPQMBBw==\
-----END EC PARAMETERS-----";
#define PEM_PRIVATE_KEY_S 	"-----BEGIN EC PRIVATE KEY-----"
#define PEM_PRIVATE_KEY_E	"-----END EC PRIVATE KEY-----"

		const char private_cert[] = "-----BEGIN EC PRIVATE KEY-----\
MHcCAQEEIJYX5gCdXJn853I/OPzB3EObLfIWkRyLY+tcxSb9+SppoAoGCCqGSM49\
AwEHoUQDQgAEjQKXzLPnx2sVLg+wJeTnHjkpoPCdK4xF8Wi4fhYEHeRLAky4BjT8\
0HBsJKgz7dsutXFRUQMWyYk+5LS8hfbeWQ==\
-----END EC PRIVATE KEY-----";

typedef struct{
	int len;
	u8  key[0x20];
}cert_pri_t;

typedef struct{
	int len;
	u8  key[0x41];
	u8  rfu[3];
}cert_pub_t;

typedef struct{
	int len;
	u8 key[20];
}cert_idkey_t;

typedef struct{
	int len;
	u8 key[0x40];
}cert_sign_t;

typedef struct{
	int val;
	cert_pri_t pri;
	cert_pub_t pub;
}private_cert_str_t;


typedef struct{
	cert_pub_t pub;
	cert_idkey_t subj;
	cert_idkey_t author;
	cert_sign_t sign;
}dev_cert_tbs_part_t;


typedef struct{
	u16 id;	
	u16 len;
	const  char *p_item;
}cert_item_t;

#define MAX_CERT_ITEM_CNT	2
cert_item_t cert_item[MAX_CERT_ITEM_CNT];

void cert_item_init(u8 en)
{
	if(en){
		cert_item[0].id =0;
		cert_item[0].p_item = uri_base;
		cert_item[0].len =sizeof(uri_base);
		cert_item[1].id =1;
		cert_item[1].p_item = pem_cert;
		cert_item[1].len =sizeof(pem_cert);
	}else{
		cert_item[0].len =0;
		cert_item[0].len =0;
	}
}

int cert_item_rsp(u16 id,u16 offset,u16 max_size,u8 *p_buf,u16 *p_len)
{
	cert_item_t *p_cert;
	int i=0;
	for(i=0;i<MAX_CERT_ITEM_CNT;i++){
		p_cert = cert_item+i;
		if(p_cert->id == id){
			break;
		}
	}
	if(i == MAX_CERT_ITEM_CNT){
		return -1;
	}
	if(offset > p_cert->len){
		return -2;
	}
	if(p_cert->len >= (offset+max_size)){
		*p_len = max_size;
	}else{
		*p_len = p_cert->len - offset;
	}
	memcpy(p_buf,(u8 *)(p_cert->p_item + offset),*p_len);
	return 0;
}



void cert_id_get(u16 *p_id,u32 *p_cnt)
{
	u32 cnt =0;
	for(u32 i=0;i<MAX_CERT_ITEM_CNT;i++){
		cert_item_t *p_cert = cert_item+i;
		if(p_cert->len){
			*(p_id+i) = p_cert->id;
			*p_cnt = ++cnt;
		}
	}
	return ;
}


void get_cert_id_list(u8 *p_list,u32 *p_len)
{
	u32 idx =0;
	foreach_arr(i,cert_item){
		cert_item_t *p_cert = cert_item+i;
		if(p_cert->len){
			p_list[idx] = p_cert->id & 0xff;
			p_list[idx+1] = (p_cert->id >>8)&0xff;
			idx+=2;
		}
	}
	*p_len = idx;
}

const char * get_cert_content_by_id(u16 id,u32* p_len)
{
	foreach_arr(i,cert_item){
		cert_item_t *p_cert = cert_item+i;
		if(p_cert->len && p_cert->id == id){
			* p_len = p_cert->len;
			return p_cert->p_item;
		}
	}
	return NULL;
}

u8 * mebedtls_asn_get_ele(u8 *p,const u8 *end,u8 ele_cnt)
{
	int ret;
	size_t len;
	u8 *p_start = p;
	const u8 *p_end = end;
	for(int i=0;i< ele_cnt;i++ ){
		if( ( ret = mbedtls_asn1_get_tag( &p_start, p_end, &len,(int)p_start[0] ) ) == 0 ){
	        p_start += len;
	    }else{
			return NULL;
		}
	}
	return p_start;
}

u8 mebedtls_asn_get_pubkey(u8 *p,u8 *end,cert_pub_t *p_pub)
{
	size_t len;
	int ret;
	u8 *p_cert_pub = mebedtls_asn_get_ele(p,end,6);
	// get the ele header part 
	if( ( ret = mbedtls_asn1_get_tag( &p_cert_pub, end, &len,
            p_cert_pub[0] ) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -2;
        return ret;
    }
	// get the first ele part 
	if( ( ret = mbedtls_asn1_get_tag( &p_cert_pub, end, &len,
            p_cert_pub[0] ) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -3;
        return ret;
    }
	p_cert_pub+=len;
	// get the second ele part 
	if( ( ret = mbedtls_asn1_get_bitstring_null( &p_cert_pub, (const u8 *)end, &len) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -3;
        return ret;
    }
	// get the pubkey ponter
	p_pub->len = len;
	memcpy(p_pub->key,p_cert_pub,p_pub->len);
	return 0;
}

int mebedtls_ans_proc_id_key(u8 *p,u8 *end,cert_idkey_t *p_key)
{
	int ret;
	size_t len;
	u8 *p_start = p;
	if( ( ret = mbedtls_asn1_get_tag( &p_start, end, &len,
            MBEDTLS_ASN1_SEQUENCE|MBEDTLS_ASN1_CONSTRUCTED ) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -2;
        return ret;
    }
	// jump the first protocol part 
	if( ( ret = mbedtls_asn1_get_tag( &p_start, end, &len,
            MBEDTLS_ASN1_OID ) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -3;
        return ret;
    }
	p_start += len;
	// jump to the key identify part 
	if( ( ret = mbedtls_asn1_get_tag( &p_start, end, &len,
            MBEDTLS_ASN1_OCTET_STRING ) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -4;
        return ret;
    }
	if(*p_start & MBEDTLS_ASN1_CONSTRUCTED){
		// need to get the tag part again 
		if( ( ret = mbedtls_asn1_get_tag( &p_start, end, &len,
            p_start[0] ) ) != 0 
            || len > (size_t)(end - p))
    	{
        	ret = -5;
        	return ret;
    	}
	}
	// jump to the key identify part 
	if( ( ret = mbedtls_asn1_get_tag( &p_start, end, &len,
            p_start[0] ) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -4;
        return ret;
    }
	// use the p_start ,and the len to set the p_key part 
	p_key->len = len;
	memcpy(p_key->key,p_start,len);
	return 0;
}

u8 mebedtls_asn_get_key_identi(u8 *p,u8 *end,cert_idkey_t *p_sub,cert_idkey_t * p_auth)
{
	size_t len;
	int ret;
	u8 *p_cert_pub = mebedtls_asn_get_ele(p,end,7);
	// jump to the first  subsequence part 
	if( ( ret = mbedtls_asn1_get_tag( &p_cert_pub, (const u8 *)end, &len,
           (int)MBEDTLS_ASN1_CONTEXT_SPECIFIC|MBEDTLS_ASN1_CONSTRUCTED|MBEDTLS_ASN1_BIT_STRING ) ) != 0
            || len > (size_t)(end - p))
    {
        ret = -2;
        return ret;
    }
	// jump to the second subsequence part 
	if( ( ret = mbedtls_asn1_get_tag( &p_cert_pub, end, &len,
            (int)MBEDTLS_ASN1_SEQUENCE|MBEDTLS_ASN1_CONSTRUCTED ) ) != 0
            || len > (size_t)(end - p))
    {
        ret = -3;
        return ret;
    }
	u8 *p_sub_key_s = mebedtls_asn_get_ele(p_cert_pub,end,2);
	u8 *p_auth_key_s = mebedtls_asn_get_ele(p_cert_pub,end,3);
	mebedtls_ans_proc_id_key(p_sub_key_s,end,p_sub);
	mebedtls_ans_proc_id_key(p_auth_key_s,end,p_auth);
	return 0;
}


int mbedtls_crt_parse_dev_cert(const unsigned char *crt, u32 crt_sz,dev_cert_tbs_part_t *msc_crt)
{
	int ret = 0;
    unsigned char *p, *end, *p_tmp, *crt_end;
    size_t len;

    if (!crt || !crt_sz) {
        return -1;
    }
    p = (unsigned char *)crt;
    end = p + crt_sz;

    /*
     * Certificate  ::=  SEQUENCE  {
     *      tbsCertificate       TBSCertificate,
     *      signatureAlgorithm   AlgorithmIdentifier,
     *      signatureValue       BIT STRING  }
     */
    if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
            MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -2;
        goto done;
    }
    /* got cert raw and save start and end */
    crt_end = p+len;

    /*
     * TBSCertificate  ::=  
		get pubkey 
		Subject Key Identifier:
		Authority Key Identifier:
     */
    // get the tbs header part 
    if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
            MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -3;
        goto done;
    }
	p_tmp = p+len;// point to the next sequence
	mebedtls_asn_get_pubkey(p, end,&(msc_crt->pub));
	mebedtls_asn_get_key_identi(p,end,&(msc_crt->subj),&(msc_crt->author));
	
    /*
     *  }
     *  -- end of TBSCertificate
     *
     *  signatureAlgorithm   AlgorithmIdentifier,
     *  signatureValue       BIT STRING
     */
    p = p_tmp;
    end = crt_end;
	
	
    /* parse signatureAlgorithm */
    if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
            MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 )
    {
        ret = -13;
        goto done;
    }

    /* parse signatureValue */
    p += len;

	if( ( ret = mbedtls_asn1_get_bitstring_null( &p,(const u8 *) end, &len ) ) != 0 )
    {
        ret = -14;
        goto done;
    }
    if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
            MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 )
    {
        ret = -15;
        goto done;
		
    }
	
	// jump to the value part 
    if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
            MBEDTLS_ASN1_INTEGER ) ) != 0 )
    {
        ret = -16;
        goto done;
    }

    /* when the first byte > 0x7f, the ASN.1 will prepend 0x00, so remove it */
    if (len >= 0x20)
        memcpy(msc_crt->sign.key, p+len-0x20, 0x20);
    else /* if the bignum less than 32 bytes, prepend 0x00(the bignum is big endian) */
        memcpy(msc_crt->sign.key+0x20-len, p, len);

    p += len;
    if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
            MBEDTLS_ASN1_INTEGER ) ) != 0 )
    {
        ret = -17;
        goto done;
    }

    if (len >= 0x20)
        memcpy(msc_crt->sign.key+0x20, p+len-0x20, 0x20);
    else
        memcpy(&(msc_crt->sign.key[0x40-len]), p, len);
	msc_crt->sign.len = 0x40;
done:
    return ret;
}


int mbedtls_crt_parse_private(const unsigned char *crt, u32 crt_sz,private_cert_str_t *msc_crt)
{
    int ret = 0;
    unsigned char *p, *end;
    size_t len;

    if (!crt || !crt_sz) {
        return -1;
    }
    p = (unsigned char *)crt;
    end = p + crt_sz;
    if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
            MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -2;
        goto done;
    }
	/* got the interger part  */
	if( ( ret = mbedtls_asn1_get_int( &p, end, &msc_crt->val ) ) != 0)
    {
        ret = -3;
        goto done;
    }
	// get the pubkey part 
	if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
            MBEDTLS_ASN1_OCTET_STRING ) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -4;
        goto done;
    }
	msc_crt->pri.len = len;
	memcpy(msc_crt->pri.key ,p,len);
	p+=len;

	// get enc caculation part  
	if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
            MBEDTLS_ASN1_CONTEXT_SPECIFIC|MBEDTLS_ASN1_CONSTRUCTED ) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -5;
        goto done;
    }

	if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
            MBEDTLS_ASN1_OID ) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -5;
        goto done;
    }
	p+=len;
	// get the pubkey part 
	
	if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
				MBEDTLS_ASN1_CONTEXT_SPECIFIC|MBEDTLS_ASN1_CONSTRUCTED|MBEDTLS_ASN1_BOOLEAN ) ) != 0 
				|| len > (size_t)(end - p))
		{
			ret = -5;
			goto done;
		}
	if( ( ret = mbedtls_asn1_get_bitstring_null( &p, (const u8 *)end, &len) ) != 0 
            || len > (size_t)(end - p))
    {
        ret = -6;
        goto done;
    }
	msc_crt->pub.len = len;
	memcpy(msc_crt->pub.key,p,msc_crt->pub.len);//2nd is type (4 means uncompress )
	
done:
    return ret;

}
u8 cert_info_check_pubkey_info(u8 *pubkey,u8* key_info)
{
	u8 sha1out[20];
	u8 len;
	if(pubkey[0]==4){
		len =65;
	}else{
		len =33;
	}
	mbedtls_sha1((const u8 *) pubkey,len,sha1out);
	if(!memcmp(sha1out,key_info,sizeof(sha1out))){
		return 1;
	}else{
		return 0;
	}
}
static private_cert_str_t private_cert_dat;
int cert_info_parse_init()
{
	u8 der_cert_buf[MAX_PEM_CERT_LEN];
	u32 der_buf_len;
	
	dev_cert_tbs_part_t cert_dat;
	// transfer the private cert to the pubkey and private key 
	der_buf_len = mbedtls_crt_pem2der_define((const u8 *)private_cert,sizeof(private_cert),
			der_cert_buf,sizeof(der_cert_buf),PEM_PRIVATE_KEY_S,PEM_PRIVATE_KEY_E);
	mbedtls_crt_parse_private(der_cert_buf,der_buf_len,&private_cert_dat);
	
	der_buf_len = mbedtls_crt_pem2der_define((const u8 *)pem_cert,sizeof(pem_cert),
		der_cert_buf,sizeof(der_cert_buf),PEM_CERT_S,PEM_CERT_E);	
	mbedtls_crt_parse_dev_cert(der_cert_buf,der_buf_len,&cert_dat);
	// check the cert is valid or not 
	if(memcmp((u8 *)(&(private_cert_dat.pub)),(u8 *)(&(cert_dat.pub)),sizeof(private_cert_dat.pub))){
		return -1;
	}
	if(!cert_info_check_pubkey_info(cert_dat.pub.key,cert_dat.subj.key)){
		return -2;
	}
	// wait to check the whole cert use ecdsa-with-sha256 check part 
	return 0;
}

int  cert_base_func_init()
{
	if(cert_info_parse_init()==0){
		// check pass , need to set the pubkey and privatekey ,and then init the record list .
		cert_item_init(1);
		return 1;
	}else{
		// certify useless, use the normal mode ,and not support the record id 
		cert_item_init(0);
	}
	return 0;
}

void cert_base_set_key(u8 *pk,u8 *sk)
{
	u8 *cert_pk = private_cert_dat.pub.key+1;
	u8 *cert_sk = private_cert_dat.pri.key;
	memcpy(pk,cert_pk,64);
	memcpy(sk,cert_sk,32);
}

#if WIN32
typedef struct{
	u16 valid;
	u16 rec_id;
	u16 rec_len;
	u8  rec_buf[0x300];
}provisioner_record_t;

typedef struct{
	u16 rec_id;
	u16 offset;
	u16 max_size;
}provisioner_record_mag_t;
provisioner_record_mag_t record_mag;

void record_mag_init()
{
	memset(&record_mag,0,sizeof(record_mag));
}

void record_mag_set(u16 rec_id,u16 max_size,u16 offset)
{
	record_mag.rec_id = rec_id;
	record_mag.max_size = max_size;
	record_mag.offset = offset;
}

void record_mag_get(u16 *p_rec_id,u16 *p_max_size,u16 *p_offset)
{
	*p_rec_id = record_mag.rec_id;
	*p_max_size = record_mag.max_size;
	*p_offset = record_mag.offset;
}

void record_mag_get_max_size(u16 *p_max_size)
{
	*p_max_size = record_mag.max_size;
}

provisioner_record_t prov_record[MAX_PROV_RECORD_CNT];

void prov_clear_all_rec()
{
	memset(prov_record,0,sizeof(prov_record));
}

void prov_set_rec_id(u16 *p_rec_data,u8 len)
{
	for(u8 i=0;i<len;i++){
		provisioner_record_t *p_rec = &(prov_record[i]);
		p_rec->valid = 1;
		p_rec->rec_id = p_rec_data[i];
	}
}

int prov_set_buf_len(u16 rec_id,u16 offset,u8 *p_buf,u32 len)
{
	// find the rec_id part 
	provisioner_record_t *p_rec;
	for(u8 i=0;i<MAX_PROV_RECORD_CNT;i++){
		p_rec = &(prov_record[i]);
		if(p_rec->valid && p_rec->rec_id == rec_id){
			p_rec->rec_len = len;
			memcpy(p_rec->rec_buf+offset,p_buf,len);
			return 0;
		}
	}
	return -1;
}

int prov_use_rec_id_get_pubkey(u16 rec_id,u8 *p_pubkey)
{
	// find the rec_id part 
	provisioner_record_t *p_rec;
	for(u8 i=0;i<MAX_PROV_RECORD_CNT;i++){
		p_rec = &(prov_record[i]);
		if(p_rec->valid && p_rec->rec_id == rec_id){
			// use the data to get the pubkey part 
			u8 der_cert_buf[MAX_PEM_CERT_LEN];
			u32 der_buf_len;
			dev_cert_tbs_part_t cert_dat;
			// transfer the private cert to the pubkey and private key 
			der_buf_len = mbedtls_crt_pem2der_define((const u8 *)pem_cert,sizeof(pem_cert),
								der_cert_buf,sizeof(der_cert_buf),PEM_CERT_S,PEM_CERT_E);	
			if(mbedtls_crt_parse_dev_cert(der_cert_buf,der_buf_len,&cert_dat)!=0){
				return -2;
			}
			memcpy(p_pubkey,cert_dat.pub.key+1,64);
			return 0;
		}
	}
	return -1;
}

	#endif


#if 0 // ecc verify valid and ecdsa is valid or not .
void check_test_pubkey_privatekey_valid()
{
	const u8 ecdsa_hash[]={
		0xa0 ,0x03 ,0x02 ,0x01	,0x02 ,0x02 ,0x02 ,0x10,
		0x00,0x30,0x0a,0x06,	0x08 ,0x2a ,0x86 ,0x48	,0xce ,0x3d ,0x04 ,0x03 	,0x02 ,0x30 ,0x81 ,0xa7,
		0x31,0x0b,0x30,0x09,	0x06 ,0x03 ,0x55 ,0x04	,0x06 ,0x13 ,0x02 ,0x46 	,0x49 ,0x31 ,0x10 ,0x30,
		0x0e,0x06,0x03,0x55,	0x04 ,0x08 ,0x0c ,0x07	,0x55 ,0x75 ,0x73 ,0x69 	,0x6d ,0x61 ,0x61 ,0x31,
		0x0e,0x30,0x0c,0x06,	0x03 ,0x55 ,0x04 ,0x07	,0x0c ,0x05 ,0x45 ,0x73 	,0x70 ,0x6f ,0x6f ,0x31,
		0x18,0x30,0x16,0x06,	0x03 ,0x55 ,0x04 ,0x0a	,0x0c ,0x0f ,0x45 ,0x78 	,0x61 ,0x6d ,0x70 ,0x6c,
		0x65,0x20,0x43,0x6f,	0x6d ,0x70 ,0x61 ,0x6e	,0x79 ,0x31 ,0x19 ,0x30 	,0x17 ,0x06 ,0x03 ,0x55,
		0x04,0x0b,0x0c,0x10,	0x45 ,0x6d ,0x62 ,0x65	,0x64 ,0x64 ,0x65 ,0x64 	,0x20 ,0x44 ,0x65 ,0x76,
		0x69,0x63,0x65,0x73,	0x31 ,0x1d ,0x30 ,0x1b	,0x06 ,0x03 ,0x55 ,0x04 	,0x03 ,0x0c ,0x14 ,0x65,
		0x6d,0x62,0x65,0x64,	0x64 ,0x65 ,0x64 ,0x2e	,0x65 ,0x78 ,0x61 ,0x6d 	,0x70 ,0x6c ,0x65 ,0x2e,
		0x63,0x6f,0x6d,0x31,	0x22 ,0x30 ,0x20 ,0x06	,0x09 ,0x2a ,0x86 ,0x48 	,0x86 ,0xf7 ,0x0d ,0x01,
		0x09,0x01,0x16,0x13,	0x73 ,0x75 ,0x70 ,0x70	,0x6f ,0x72 ,0x74 ,0x40 	,0x65 ,0x78 ,0x61 ,0x6d,
		0x70,0x6c,0x65,0x2e,	0x63 ,0x6f ,0x6d ,0x30	,0x1e ,0x17 ,0x0d ,0x31 	,0x38 ,0x31 ,0x31 ,0x31,
		0x36,0x30,0x37,0x33,	0x34 ,0x34 ,0x36 ,0x5a	,0x17 ,0x0d ,0x31 ,0x39 	,0x31 ,0x31 ,0x31 ,0x36,
		0x30,0x37,0x33,0x34,	0x34 ,0x36 ,0x5a ,0x30	,0x81 ,0x93 ,0x31 ,0x0b 	,0x30 ,0x09 ,0x06 ,0x03,
		0x55,0x04,0x06,0x13,	0x02 ,0x46 ,0x49 ,0x31	,0x10 ,0x30 ,0x0e ,0x06 	,0x03 ,0x55 ,0x04 ,0x08,
		0x0c,0x07,0x55,0x75,	0x73 ,0x69 ,0x6d ,0x61	,0x61 ,0x31 ,0x0e ,0x30 	,0x0c ,0x06 ,0x03 ,0x55,
		0x04,0x07,0x0c,0x05,	0x45 ,0x73 ,0x70 ,0x6f	,0x6f ,0x31 ,0x18 ,0x30 	,0x16 ,0x06 ,0x03 ,0x55,
		0x04,0x0a,0x0c,0x0f,	0x45 ,0x78 ,0x61 ,0x6d	,0x70 ,0x6c ,0x65 ,0x20 	,0x43 ,0x6f ,0x6d ,0x70,
		0x61,0x6e,0x79,0x31,	0x19 ,0x30 ,0x17 ,0x06	,0x03 ,0x55 ,0x04 ,0x0b 	,0x0c ,0x10 ,0x45 ,0x6d,
		0x62,0x65,0x64,0x64,	0x65 ,0x64 ,0x20 ,0x44	,0x65 ,0x76 ,0x69 ,0x63 	,0x65 ,0x73 ,0x31 ,0x2d,
		0x30,0x2b,0x06,0x03,	0x55 ,0x04 ,0x03 ,0x0c	,0x24 ,0x62 ,0x30 ,0x39 	,0x64 ,0x63 ,0x38 ,0x34,
		0x37,0x2d,0x35,0x34,	0x30 ,0x38 ,0x2d ,0x34	,0x30 ,0x63 ,0x63 ,0x2d 	,0x39 ,0x63 ,0x35 ,0x34,
		0x2d,0x30,0x66,0x65,	0x38 ,0x63 ,0x38 ,0x37	,0x34 ,0x32 ,0x39 ,0x65 	,0x37 ,0x30 ,0x59 ,0x30,
		0x13,0x06,0x07,0x2a,	0x86 ,0x48 ,0xce ,0x3d	,0x02 ,0x01 ,0x06 ,0x08 	,0x2a ,0x86 ,0x48 ,0xce,
		0x3d,0x03,0x01,0x07,	0x03 ,0x42 ,0x00 ,0x04	,0x8d ,0x02 ,0x97 ,0xcc 	,0xb3 ,0xe7 ,0xc7 ,0x6b,
		0x15,0x2e,0x0f,0xb0,	0x25 ,0xe4 ,0xe7 ,0x1e	,0x39 ,0x29 ,0xa0 ,0xf0 	,0x9d ,0x2b ,0x8c ,0x45,
		0xf1,0x68,0xb8,0x7e,	0x16 ,0x04 ,0x1d ,0xe4	,0x4b ,0x02 ,0x4c ,0xb8 	,0x06 ,0x34 ,0xfc ,0xd0,
		0x70,0x6c,0x24,0xa8,	0x33 ,0xed ,0xdb ,0x2e	,0xb5 ,0x71 ,0x51 ,0x51 	,0x03 ,0x16 ,0xc9 ,0x89,
		0x3e,0xe4,0xb4,0xbc,	0x85 ,0xf6 ,0xde ,0x59	,0xa3 ,0x81 ,0x87 ,0x30 	,0x81 ,0x84 ,0x30 ,0x09,
		0x06,0x03,0x55,0x1d,	0x13 ,0x04 ,0x02 ,0x30	,0x00 ,0x30 ,0x0b ,0x06 	,0x03 ,0x55 ,0x1d ,0x0f,
		0x04,0x04,0x03,0x02,	0x03 ,0x08 ,0x30 ,0x1d	,0x06 ,0x03 ,0x55 ,0x1d 	,0x0e ,0x04 ,0x16 ,0x04,
		0x14,0x28,0xbc,0x42,	0x6a ,0x68 ,0xdb ,0x63	,0x96 ,0x70 ,0x85 ,0x71 	,0xe4 ,0xcf ,0xc9 ,0x72,
		0x1c,0xe9,0x8b,0x68,	0x15 ,0x30 ,0x1f ,0x06	,0x03 ,0x55 ,0x1d ,0x23 	,0x04 ,0x18 ,0x30 ,0x16,
		0x80,0x14,0xc3,0x7e,	0xea ,0x54 ,0x6a ,0x02	,0x6d ,0x7c ,0xce ,0xf5 	,0xf4 ,0xa0 ,0xd3 ,0xf5,
		0xa8,0xd4,0x98,0x26,	0xa3 ,0x4a ,0x30 ,0x2a	,0x06 ,0x14 ,0x69 ,0x82 	,0xe1 ,0x9d ,0xe4 ,0x91,
		0xea,0xc0,0xc2,0x83,	0x99 ,0x9c ,0xaa ,0x83	,0xfd ,0x8c ,0xc3 ,0xd0 	,0xd3 ,0x67 ,0x04 ,0x12,
		0x04,0x10,0x4e,0x54,	0x34 ,0x6f ,0x70 ,0x46	,0x72 ,0x5a ,0x74 ,0x44 	,0x51 ,0x68 ,0x56 ,0x59,
		0x77,0x67
	};

	u8 A_debug_privatekey[32]={
		0x96,0x17,0xe6,0x00,0x9d,0x5c,0x99,0xfc,0xe7,0x72,0x3f,0x38,0xfc,0xc1,0xdc,0x43,
		0x9b,0x2d,0xf2,0x16,0x91,0x1c,0x8b,0x63,0xeb,0x5c,0xc5,0x26,0xfd,0xf9,0x2a,0x69
	};
	u8 A_debug_pubkey[64]={
		0x8d,0x02,0x97,0xcc,0xb3,0xe7,0xc7,0x6b,0x15,0x2e,0x0f,0xb0,0x25,0xe4,
		0xe7,0x1e,0x39,0x29,0xa0,0xf0,0x9d,0x2b,0x8c,0x45,0xf1,0x68,0xb8,0x7e,0x16,
		0x04,0x1d,0xe4,0x4b,0x02,0x4c,0xb8,0x06,0x34,0xfc,0xd0,0x70,0x6c,0x24,0xa8,
		0x33,0xed,0xdb,0x2e,0xb5,0x71,0x51,0x51,0x03,0x16,0xc9,0x89,0x3e,0xe4,0xb4,
		0xbc,0x85,0xf6,0xde,0x59};
	static u32 A_debug_key_check=0;
	#if 1
	micro_ecc_init();
	u8 A_debug_sha256_hash[32];
	u8 A_debug_sign_dat[64];
	mbedtls_sha256(ecdsa_hash, sizeof(ecdsa_hash), A_debug_sha256_hash, 0);
	micro_ecc_sign(NULL,A_debug_privatekey,A_debug_sha256_hash,A_debug_sign_dat);
	if(micro_ecc_verify(NULL,A_debug_pubkey,A_debug_sha256_hash,A_debug_sign_dat)== 0){
		A_debug_key_check = 0x55;
	}else{
		A_debug_key_check = 0x44;
	}
	#endif
	#if 0 // key is valid
		unsigned char A_pro_dsk[] = { 0x52,0x9a,0xa0,0x67,0x0d,0x72,0xcd,0x64, 0x97,0x50,0x2e,0xd4,0x73,0x50,0x2b,0x03,
						0x7e,0x88,0x03,0xb5,0xc6,0x08,0x29,0xa5, 0xa3,0xca,0xa2,0x19,0x50,0x55,0x30,0xba};
		unsigned char A_pro_dpk[] = { 0xf4,0x65,0xe4,0x3f,0xf2,0x3d,0x3f,0x1b, 0x9d,0xc7,0xdf,0xc0,0x4d,0xa8,0x75,0x81,
						0x84,0xdb,0xc9,0x66,0x20,0x47,0x96,0xec, 0xcf,0x0d,0x6c,0xf5,0xe1,0x65,0x00,0xcc,
						0x02,0x01,0xd0,0x48,0xbc,0xbb,0xd8,0x99, 0xee,0xef,0xc4,0x24,0x16,0x4e,0x33,0xc2,
						0x01,0xc2,0xb0,0x10,0xca,0x6b,0x4d,0x43, 0xa8,0xa1,0x55,0xca,0xd8,0xec,0xb2,0x79};
		u8 k0[32],k1[32];
		micro_ecc_shared_secret_compute(NULL, A_pro_dsk, A_debug_pubkey, k0);
		micro_ecc_shared_secret_compute(NULL, A_debug_privatekey, A_pro_dpk, k1);
		if(!memcmp (k0, k1, 16)){
			A_debug_key_check =1;
		}else{
			A_debug_key_check=0x55;
		}
	#endif

}
#endif
#if 0 // wait for pass the ecdsa-with-sha256 result ,to pass the verify part
void sha1_test_der()
{
	/**********
	char input[3]="abc";
	{ 0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
		  0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D },
	***********/
	static u8 A_debug_pubkey[64]={
		0x8d,0x02,0x97,0xcc,0xb3,0xe7,0xc7,0x6b,0x15,0x2e,0x0f,0xb0,0x25,0xe4,
		0xe7,0x1e,0x39,0x29,0xa0,0xf0,0x9d,0x2b,0x8c,0x45,0xf1,0x68,0xb8,0x7e,0x16,
		0x04,0x1d,0xe4,0x4b,0x02,0x4c,0xb8,0x06,0x34,0xfc,0xd0,0x70,0x6c,0x24,0xa8,
		0x33,0xed,0xdb,0x2e,0xb5,0x71,0x51,0x51,0x03,0x16,0xc9,0x89,0x3e,0xe4,0xb4,
		0xbc,0x85,0xf6,0xde,0x59};

	static u8 A_debug_privatekey[32]={
		0x96,0x17,0xe6,0x00,0x9d,0x5c,0x99,0xfc,0xe7,0x72,0x3f,0x38,0xfc,0xc1,0xdc,0x43,
		0x9b,0x2d,0xf2,0x16,0x91,0x1c,0x8b,0x63,0xeb,0x5c,0xc5,0x26,0xfd,0xf9,0x2a,0x69
	};
	static u8 A_debug_subject[20]={
		0x28,0xBC,0x42,0x6A,0x68,0xDB,0x63,0x96,0x70,0x85,0x71,0xE4,0xCF,0xC9,0x72,
		0x1C,0xE9,0x8B,0x68,0x15
	};

	static u8 A_debug_authorkey[20]={
		0xC3,0x7E,0xEA,0x54,0x6A,0x02,0x6D,0x7C,0xCE,0xF5,0xF4,0xA0,0xD3,0xF5,0xA8,
		0xD4,0x98,0x26,0xA3,0x4A
	};

	const u8 ecdsa_hash[]={
		0x30,0x82,0x02,0x5a,	0xa0 ,0x03 ,0x02 ,0x01	,0x02 ,0x02 ,0x02 ,0x10,
		0x00,0x30,0x0a,0x06,	0x08 ,0x2a ,0x86 ,0x48	,0xce ,0x3d ,0x04 ,0x03 	,0x02 ,0x30 ,0x81 ,0xa7,
		0x31,0x0b,0x30,0x09,	0x06 ,0x03 ,0x55 ,0x04	,0x06 ,0x13 ,0x02 ,0x46 	,0x49 ,0x31 ,0x10 ,0x30,
		0x0e,0x06,0x03,0x55,	0x04 ,0x08 ,0x0c ,0x07	,0x55 ,0x75 ,0x73 ,0x69 	,0x6d ,0x61 ,0x61 ,0x31,
		0x0e,0x30,0x0c,0x06,	0x03 ,0x55 ,0x04 ,0x07	,0x0c ,0x05 ,0x45 ,0x73 	,0x70 ,0x6f ,0x6f ,0x31,
		0x18,0x30,0x16,0x06,	0x03 ,0x55 ,0x04 ,0x0a	,0x0c ,0x0f ,0x45 ,0x78 	,0x61 ,0x6d ,0x70 ,0x6c,
		0x65,0x20,0x43,0x6f,	0x6d ,0x70 ,0x61 ,0x6e	,0x79 ,0x31 ,0x19 ,0x30 	,0x17 ,0x06 ,0x03 ,0x55,
		0x04,0x0b,0x0c,0x10,	0x45 ,0x6d ,0x62 ,0x65	,0x64 ,0x64 ,0x65 ,0x64 	,0x20 ,0x44 ,0x65 ,0x76,
		0x69,0x63,0x65,0x73,	0x31 ,0x1d ,0x30 ,0x1b	,0x06 ,0x03 ,0x55 ,0x04 	,0x03 ,0x0c ,0x14 ,0x65,
		0x6d,0x62,0x65,0x64,	0x64 ,0x65 ,0x64 ,0x2e	,0x65 ,0x78 ,0x61 ,0x6d 	,0x70 ,0x6c ,0x65 ,0x2e,
		0x63,0x6f,0x6d,0x31,	0x22 ,0x30 ,0x20 ,0x06	,0x09 ,0x2a ,0x86 ,0x48 	,0x86 ,0xf7 ,0x0d ,0x01,
		0x09,0x01,0x16,0x13,	0x73 ,0x75 ,0x70 ,0x70	,0x6f ,0x72 ,0x74 ,0x40 	,0x65 ,0x78 ,0x61 ,0x6d,
		0x70,0x6c,0x65,0x2e,	0x63 ,0x6f ,0x6d ,0x30	,0x1e ,0x17 ,0x0d ,0x31 	,0x38 ,0x31 ,0x31 ,0x31,
		0x36,0x30,0x37,0x33,	0x34 ,0x34 ,0x36 ,0x5a	,0x17 ,0x0d ,0x31 ,0x39 	,0x31 ,0x31 ,0x31 ,0x36,
		0x30,0x37,0x33,0x34,	0x34 ,0x36 ,0x5a ,0x30	,0x81 ,0x93 ,0x31 ,0x0b 	,0x30 ,0x09 ,0x06 ,0x03,
		0x55,0x04,0x06,0x13,	0x02 ,0x46 ,0x49 ,0x31	,0x10 ,0x30 ,0x0e ,0x06 	,0x03 ,0x55 ,0x04 ,0x08,
		0x0c,0x07,0x55,0x75,	0x73 ,0x69 ,0x6d ,0x61	,0x61 ,0x31 ,0x0e ,0x30 	,0x0c ,0x06 ,0x03 ,0x55,
		0x04,0x07,0x0c,0x05,	0x45 ,0x73 ,0x70 ,0x6f	,0x6f ,0x31 ,0x18 ,0x30 	,0x16 ,0x06 ,0x03 ,0x55,
		0x04,0x0a,0x0c,0x0f,	0x45 ,0x78 ,0x61 ,0x6d	,0x70 ,0x6c ,0x65 ,0x20 	,0x43 ,0x6f ,0x6d ,0x70,
		0x61,0x6e,0x79,0x31,	0x19 ,0x30 ,0x17 ,0x06	,0x03 ,0x55 ,0x04 ,0x0b 	,0x0c ,0x10 ,0x45 ,0x6d,
		0x62,0x65,0x64,0x64,	0x65 ,0x64 ,0x20 ,0x44	,0x65 ,0x76 ,0x69 ,0x63 	,0x65 ,0x73 ,0x31 ,0x2d,
		0x30,0x2b,0x06,0x03,	0x55 ,0x04 ,0x03 ,0x0c	,0x24 ,0x62 ,0x30 ,0x39 	,0x64 ,0x63 ,0x38 ,0x34,
		0x37,0x2d,0x35,0x34,	0x30 ,0x38 ,0x2d ,0x34	,0x30 ,0x63 ,0x63 ,0x2d 	,0x39 ,0x63 ,0x35 ,0x34,
		0x2d,0x30,0x66,0x65,	0x38 ,0x63 ,0x38 ,0x37	,0x34 ,0x32 ,0x39 ,0x65 	,0x37 ,0x30 ,0x59 ,0x30,
		0x13,0x06,0x07,0x2a,	0x86 ,0x48 ,0xce ,0x3d	,0x02 ,0x01 ,0x06 ,0x08 	,0x2a ,0x86 ,0x48 ,0xce,
		0x3d,0x03,0x01,0x07,	0x03 ,0x42 ,0x00 ,0x04	,0x8d ,0x02 ,0x97 ,0xcc 	,0xb3 ,0xe7 ,0xc7 ,0x6b,
		0x15,0x2e,0x0f,0xb0,	0x25 ,0xe4 ,0xe7 ,0x1e	,0x39 ,0x29 ,0xa0 ,0xf0 	,0x9d ,0x2b ,0x8c ,0x45,
		0xf1,0x68,0xb8,0x7e,	0x16 ,0x04 ,0x1d ,0xe4	,0x4b ,0x02 ,0x4c ,0xb8 	,0x06 ,0x34 ,0xfc ,0xd0,
		0x70,0x6c,0x24,0xa8,	0x33 ,0xed ,0xdb ,0x2e	,0xb5 ,0x71 ,0x51 ,0x51 	,0x03 ,0x16 ,0xc9 ,0x89,
		0x3e,0xe4,0xb4,0xbc,	0x85 ,0xf6 ,0xde ,0x59	,0xa3 ,0x81 ,0x87 ,0x30 	,0x81 ,0x84 ,0x30 ,0x09,
		0x06,0x03,0x55,0x1d,	0x13 ,0x04 ,0x02 ,0x30	,0x00 ,0x30 ,0x0b ,0x06 	,0x03 ,0x55 ,0x1d ,0x0f,
		0x04,0x04,0x03,0x02,	0x03 ,0x08 ,0x30 ,0x1d	,0x06 ,0x03 ,0x55 ,0x1d 	,0x0e ,0x04 ,0x16 ,0x04,
		0x14,0x28,0xbc,0x42,	0x6a ,0x68 ,0xdb ,0x63	,0x96 ,0x70 ,0x85 ,0x71 	,0xe4 ,0xcf ,0xc9 ,0x72,
		0x1c,0xe9,0x8b,0x68,	0x15 ,0x30 ,0x1f ,0x06	,0x03 ,0x55 ,0x1d ,0x23 	,0x04 ,0x18 ,0x30 ,0x16,
		0x80,0x14,0xc3,0x7e,	0xea ,0x54 ,0x6a ,0x02	,0x6d ,0x7c ,0xce ,0xf5 	,0xf4 ,0xa0 ,0xd3 ,0xf5,
		0xa8,0xd4,0x98,0x26,	0xa3 ,0x4a ,0x30 ,0x2a	,0x06 ,0x14 ,0x69 ,0x82 	,0xe1 ,0x9d ,0xe4 ,0x91,
		0xea,0xc0,0xc2,0x83,	0x99 ,0x9c ,0xaa ,0x83	,0xfd ,0x8c ,0xc3 ,0xd0 	,0xd3 ,0x67 ,0x04 ,0x12,
		0x04,0x10,0x4e,0x54,	0x34 ,0x6f ,0x70 ,0x46	,0x72 ,0x5a ,0x74 ,0x44 	,0x51 ,0x68 ,0x56 ,0x59,
		0x77,0x67
	};
	static u8 A_debug_ecc_sign[64]={
		0xc5,0x59,0x74,0xbb,0x14,0xb7,0xa6,0x82,0x56,0x98,0xab,0x3f,0x35,0xf8,0x7d,0x60,
		0x70,0x68,0x5d,0x26,0x38,0x57,0xf4,0x51,0x43,0x9a,0xcb,0xee,0xaf,0x15,0xfa,0x21,
		0x77,0x67,0xf5,0xbe,0xca,0x9e,0x28,0xdd,0xe9,0x8a,0x9e,0xb7,0x6b,0x86,0x91,0xdc,
		0x93,0x8b,0xd9,0x85,0x55,0xa7,0xc1,0x32,0x44,0xb3,0x13,0xcc,0x2d,0x69,0xa3,0x25
	};
	// use the subject key to do the ecdsa-with_sha256
		static u8 A_debug_sha256_out[32];
		static int A_debug_esdsa_result=0x55;
		micro_ecc_init();
		#if 1
		mbedtls_sha256(ecdsa_hash, sizeof(ecdsa_hash), A_debug_sha256_out, 0);
		// need to transfer the pubkey to big endiness
		A_debug_esdsa_result = micro_ecc_verify(NULL,A_debug_pubkey,A_debug_sha256_out,A_debug_ecc_sign);
		#endif
}
#endif

#if 0// cert pem decode to der code buf part

void pem_to_der_test_sample()
{


	static u8 A_debug_cert_data[0x2c0],A_debug_cert_data1[128],A_debug_cert_data2[128];
	static u32 A_debug_cer_len,A_debug_cer_len1,A_debug_cer_len2;
	#if 1
	A_debug_cer_len = mbedtls_crt_pem2der_define((const u8 *)pem_cert,sizeof(pem_cert),
		A_debug_cert_data,sizeof(A_debug_cert_data),PEM_CERT_S,PEM_CERT_E);
	#endif
	#if 1
	A_debug_cer_len1 = mbedtls_crt_pem2der_define((const u8 *)para_cert,sizeof(para_cert),
			A_debug_cert_data1,sizeof(A_debug_cert_data1),PEM_EC_S,PEM_EC_E);
	#endif
	#if 1
	A_debug_cer_len2 = mbedtls_crt_pem2der_define((const u8 *)private_cert,sizeof(private_cert),
			A_debug_cert_data2,sizeof(A_debug_cert_data2),PEM_PRIVATE_KEY_S,PEM_PRIVATE_KEY_E);
	#endif

}
#endif


#endif