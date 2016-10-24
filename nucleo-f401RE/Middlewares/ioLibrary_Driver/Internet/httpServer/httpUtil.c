/**
 * @file	httpUtil.c
 * @brief	HTTP Server Utilities	
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include "ioLibrary_Driver/Internet/httpServer/httpUtil.h"
#include "ioLibrary_Driver/Ethernet/wizchip_conf.h"
#include <stm32f4xx_hal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ff.h"

extern ADC_HandleTypeDef hadc1;
extern uint16_t adcConv[200];
extern TIM_HandleTypeDef htim2;

uint8_t http_get_cgi_handler(uint8_t * uri_name, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_FAILED;
	uint16_t len = 0;

	if(strcmp((const char*)uri_name, "temp.cgi") == 0) {
    uint16_t rawValue;
    float temp;
    uint8_t *pbuf = buf;

    pbuf += sprintf(pbuf, "[");
    for(uint8_t i = 0; i < 200; i++)
        pbuf += sprintf(pbuf, "%d,", adcConv[i]);
    sprintf(--pbuf, "]");
    *file_len = strlen(buf);
    return HTTP_OK;

	} else if(strcmp((const char*)uri_name, "network.cgi") == 0) {
	  wiz_NetInfo netInfo;
	  wizchip_getnetinfo(&netInfo);
	  sprintf((char*)buf, "{\"ip\":\"%d.%d.%d.%d\","
	                       "\"nm\":\"%d.%d.%d.%d\","
	                       "\"gw\":\"%d.%d.%d.%d\","
	                       "\"dns\":\"%d.%d.%d.%d\","
	                       "\"dhcp\":\"%d\"}", netInfo.ip[0], netInfo.ip[1], netInfo.ip[2], netInfo.ip[3],
                                             netInfo.sn[0], netInfo.sn[1], netInfo.sn[2], netInfo.sn[3],
	                                           netInfo.gw[0], netInfo.gw[1], netInfo.gw[2], netInfo.gw[3],
	                                           netInfo.dns[0], netInfo.dns[1], netInfo.dns[2], netInfo.dns[3],
	                                           netInfo.dhcp);
	  *file_len = strlen((char*)buf);
	  return HTTP_OK;
	} else if(strcmp((const char*)uri_name, "listdirs.cgi") == 0) {
    FRESULT res;
    DIR dir;
    UINT i;
    FILINFO fno;
    char *rbuf = buf;
    static TCHAR lfname[_MAX_LFN];
    TCHAR *fname;

    res = f_opendir(&dir, "/");                       /* Open the directory */
    fno.lfname = lfname;
    fno.lfsize = _MAX_LFN;
    while(1) {
      res = f_readdir(&dir, &fno);
      if (res != FR_OK || fno.fname[0] == 0) break;

      if(fno.fattrib & AM_DIR)
          sprintf(rbuf, "%s:D", *fno.lfname ? fno.lfname : fno.fname);
      else
          sprintf(rbuf, "%s:F", *fno.lfname ? fno.lfname : fno.fname);

      rbuf+=strlen(fno.fname);
      *(rbuf++) = ',';
    }
    *(rbuf++) = '\0';

	  *file_len = strlen((char*)buf);
    return HTTP_OK;
	}



//	if(predefined_get_cgi_processor(uri_name, buf, &len))
//	{
//		;
//	}
//	else if(strcmp((const char *)uri_name, "example.cgi") == 0)
//	{
//		// To do
//		;
//	}
//	else
//	{
//		// CGI file not found
//		ret = HTTP_FAILED;
//	}

	if(ret)	*file_len = len;
	return ret;
}

uint8_t http_post_cgi_handler(uint8_t * uri_name, st_http_request * p_http_request, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_OK;
	uint16_t len = 0;
	uint8_t val = 0;
	uint8_t *param = p_http_request->URI;

//	if(predefined_set_cgi_processor(uri_name, p_http_request->URI, buf, &len))
//	{
//		;
//	}
	  if(strcmp((const char *)uri_name, "sf.cgi") == 0) {
	    param = get_http_param_value((char*)p_http_request->URI, "f");
	    if(param != p_http_request->URI) {
	      uint16_t freq = atoi(param);
	      HAL_ADC_Stop_DMA(&hadc1);
        HAL_TIM_Base_Stop(&htim2);
	      memset(adcConv, 0, sizeof(uint16_t)*200);
	      htim2.Init.Period = freq;
	      HAL_TIM_Base_Init(&htim2);
	      HAL_TIM_Base_Start(&htim2);
	      HAL_ADC_Start_DMA(&hadc1, adcConv, 200);
	      sprintf(buf, "OK");
	      len = strlen(buf);
	    }

    }
	  else if(strcmp((const char *)uri_name, "network.cgi") == 0) {
      uint8_t ip[4];
      param = get_http_param_value((char*)p_http_request->URI, "ipaddr");
      inet_addr_((u_char*)param, ip);
    }


	if(ret)	*file_len = len;
	return ret;
}
