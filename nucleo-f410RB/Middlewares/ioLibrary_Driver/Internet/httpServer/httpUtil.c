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
#include "config.h"

extern ADC_HandleTypeDef hadc1;
extern uint16_t adcConv[100], _adcConv[200];
extern TIM_HandleTypeDef htim2;
extern osSemaphoreId adcSemID;

uint8_t http_get_cgi_handler(uint8_t * uri_name, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_FAILED;
	uint16_t len = 0;

	if(strcmp((const char*)uri_name, "adc.cgi") == 0) {
    char *pbuf = (char*)buf;

    /* Compute the current TIM2 frequency */
    uint32_t freq = HAL_RCC_GetPCLK2Freq() / (((htim2.Init.Prescaler + 1) * (htim2.Init.Period + 1)));
    pbuf += sprintf(pbuf, "{\"f\":%lu,\"d\":[", freq);

    /* Wait until the HAL_ADC_ConvCpltCallback() or HAL_ADC_HalfConvCpltCallback() finish */
    osSemaphoreWait(adcSemID, osWaitForever);
    for(uint8_t i = 0; i < 100; i++)
        pbuf += sprintf(pbuf, "%.2f,", adcConv[i]*0.805);
    osSemaphoreRelease(adcSemID);

    sprintf(--pbuf, "]}");
    *file_len = strlen((char*)buf);

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

	if(ret)	*file_len = len;
	return ret;
}

uint8_t http_post_cgi_handler(uint8_t * uri_name, st_http_request * p_http_request, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_OK;
	uint16_t len = 0;
	uint8_t *param = p_http_request->URI;

  if(strcmp((const char *)uri_name, "sf.cgi") == 0) {
    param = get_http_param_value((char*)p_http_request->URI, "f");
    if(param != p_http_request->URI) {
      /* User wants to change ADC sampling frequency. We so stop conversion */
      HAL_ADC_Stop_DMA(&hadc1);
      HAL_TIM_Base_Stop(&htim2);

      /* Obtain the current TIM2 frequency */
      uint32_t cfreq = HAL_RCC_GetPCLK2Freq() / (((htim2.Init.Prescaler + 1) * (htim2.Init.Period + 1))), nfreq = 0;

      if(*param == '1')
        nfreq = cfreq * 2;
      else
        nfreq = cfreq / 2;

      htim2.Init.Prescaler = 0;
      htim2.Init.Period = 1;
      /* We cycle until we reach the wanted frequency. At frequencies below 30Hz, this algorithm is
       * largely inefficient */
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
      HAL_ADC_Start_DMA(&hadc1, (uint32_t*)_adcConv, 200);

      sprintf((char*)buf, "OK");
      len = strlen((char*)buf);
    }

  }
  else if(strcmp((const char *)uri_name, "network.cgi") == 0) {
    wiz_NetInfo netInfo;
    wizchip_getnetinfo(&netInfo);

    param = get_http_param_value((char*)p_http_request->URI, "dhcp");
    if(param != 0) {
      netInfo.dhcp = NETINFO_DHCP;
    } else {
      netInfo.dhcp = NETINFO_STATIC;

      param = get_http_param_value((char*)p_http_request->URI, "ip");
      if(param != 0)
        inet_addr_((u_char*)param, netInfo.ip);
      else
        return HTTP_FAILED;

      param = get_http_param_value((char*)p_http_request->URI, "sn");
      if(param != 0)
        inet_addr_((u_char*)param, netInfo.sn);
      else
        return HTTP_FAILED;

      param = get_http_param_value((char*)p_http_request->URI, "gw");
      if(param != 0)
        inet_addr_((u_char*)param, netInfo.gw);
      else
        return HTTP_FAILED;

      param = get_http_param_value((char*)p_http_request->URI, "dns");
      if(param != 0)
        inet_addr_((u_char*)param, netInfo.dns);
      else
        return HTTP_FAILED;
    }
    if(!WriteNetCfgInFile(&netInfo))
      sprintf((char*)buf, "FAILED");
    else
      sprintf((char*)buf, "OK");

    /* Change network parameters */
    wizchip_setnetinfo(&netInfo);
    len = strlen((char*)buf);
  }

	if(ret)	*file_len = len;
	return ret;
}
