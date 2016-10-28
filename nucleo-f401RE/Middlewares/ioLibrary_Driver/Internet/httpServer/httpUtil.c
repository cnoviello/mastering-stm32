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
#include <cmsis_os.h>
#include "ff.h"

extern ADC_HandleTypeDef hadc1;
extern uint16_t adcConv[100], adcConv_[200];
extern uint8_t convComplete;
extern TIM_HandleTypeDef htim2;
extern osSemaphoreId adcSemID;

uint8_t http_get_cgi_handler(uint8_t * uri_name, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_FAILED;
	uint16_t len = 0;

	if(strcmp((const char*)uri_name, "adc.cgi") == 0) {
    uint16_t rawValue;
    float temp;
    uint8_t *pbuf = buf;

    uint32_t freq = HAL_RCC_GetPCLK2Freq() / (((htim2.Init.Prescaler + 1) * (htim2.Init.Period + 1)));

    pbuf += sprintf(pbuf, "{\"f\":%d,\"d\":[", freq);

    osSemaphoreWait(adcSemID, osWaitForever);

    for(uint8_t i = 0; i < 100; i++)
        pbuf += sprintf(pbuf, "%.2f,", adcConv[i]*0.805);

    osSemaphoreRelease(adcSemID);

    sprintf(--pbuf, "]}");
    *file_len = strlen(buf);


    convComplete = 0;
//    HAL_ADC_Start_DMA(&hadc1, adcConv, 100);

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
        HAL_ADC_Stop_DMA(&hadc1);
        HAL_TIM_Base_Stop(&htim2);
        uint32_t cfreq = HAL_RCC_GetPCLK2Freq() / (((htim2.Init.Prescaler + 1) * (htim2.Init.Period + 1))), nfreq = 0;

        if(*param == '1')
          nfreq = cfreq * 2;
        else
          nfreq = cfreq / 2;

        htim2.Init.Prescaler = 0;
        htim2.Init.Period = 1;
        while(1) {
          cfreq = HAL_RCC_GetPCLK2Freq() / (((htim2.Init.Prescaler + 1) * (htim2.Init.Period + 1)));
          if (nfreq < cfreq) {
            if(++htim2.Init.Period == 0) {
              htim2.Init.Prescaler++;
              htim2.Init.Period++;
            }
          } else {
              break;
          }
        }
        HAL_TIM_Base_Init(&htim2);
	      HAL_TIM_Base_Start(&htim2);
	      HAL_ADC_Start_DMA(&hadc1, adcConv_, 200);
	      sprintf(buf, "OK");
	      len = strlen(buf);
	    }

    }
	  else if(strcmp((const char *)uri_name, "network.cgi") == 0) {
      uint8_t ip[4];
      param = get_http_param_value((char*)p_http_request->URI, "ipaddr");
      inet_addr_((u_char*)param, ip);
    }

    else if(strcmp((const char *)uri_name, "sync.cgi") == 0) {

      sprintf(buf, "OK");
      len = strlen(buf);
    }



	if(ret)	*file_len = len;
	return ret;
}
