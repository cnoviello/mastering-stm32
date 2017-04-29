/*
 * config.c
 *
 *  Created on: 31 ott 2016
 *      Author: cnoviello
 */
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "ioLibrary_Driver/Ethernet/wizchip_conf.h"
#include "ioLibrary_Driver/Internet/httpServer/httpParser.h"

uint8_t ReadNetCfgFromFile(wiz_NetInfo *netInfo) {
#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
  FIL cfgFile;
  UINT br;
#else
  FILE *cfgFile = NULL;
#endif
  char buf[30], *p;

  p = buf;
#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
  FRESULT res = f_open(&cfgFile, "/net.cfg", FA_READ);
  if(res == FR_OK) {
#else
  if((cfgFile = fopen(OS_BASE_FS_PATH"net.cfg","r")) != NULL) {
#endif
    for(uint8_t i = 0; i < 6; i++) {
      while(1) {
#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
        if(f_read(&cfgFile, p, 1, &br) == FR_OK) {
#else
        if(fread(p, sizeof(char), 1, cfgFile)) {
#endif
          if (*p == '\r') {
            continue;
          }
          else if(*p == '\n') {
            *p = '\0';
            break;
          }
          else {
            p++;
          }
        } else { /* Reached the EOF */
          break;
        }
      }

      if(p != buf)  {
          switch(i) {
          case 0: //DHCP
            if(strcmp(buf, "NODHCP") == 0)
              netInfo->dhcp = NETINFO_STATIC;
            else
              netInfo->dhcp = NETINFO_DHCP;
            break;
          case 1: //MAC Address
            mac_addr_((uint8_t*)buf, netInfo->mac);
            break;
          case 2: //IP Address
            inet_addr_((uint8_t*)buf, netInfo->ip);
            break;
          case 3: //Netmask
            inet_addr_((uint8_t*)buf, netInfo->sn);
            break;
          case 4: //Gateway Address
            inet_addr_((uint8_t*)buf, netInfo->gw);
            break;
          case 5: //DNS Address
            inet_addr_((uint8_t*)buf, netInfo->dns);
            break;
          }
          p = buf;
        }
      else {
        return  0;
      }
    }
#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
    if(f_close(&cfgFile) != FR_OK)
#else
    if(fclose(cfgFile))
#endif
      return 0;

    return 1;
  }

  return 0;
}

uint8_t WriteNetCfgInFile(wiz_NetInfo *netInfo) {
#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
  FIL cfgFile;
  UINT bw;
#else
  FILE *cfgFile = NULL;
#endif
  char buf[30];

#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
  if(f_open(&cfgFile, "0:/net.cfg",FA_WRITE) == FR_OK) {
#else
  if((cfgFile = fopen(OS_BASE_FS_PATH"net.cfg","w ")) != NULL) {
#endif
    if(netInfo->dhcp == NETINFO_DHCP)
      sprintf(buf, "DHCP\n");
    else
      sprintf(buf, "NODHCP\n");
#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
    if(f_write(&cfgFile, buf, strlen(buf), &bw) != FR_OK)
#else
    if(!fwrite(buf, sizeof(char), strlen(buf), cfgFile))
#endif
      return 0;

    sprintf(buf, "%x:%x:%x:%x:%x:%x\n", netInfo->mac[0], netInfo->mac[1], netInfo->mac[2],
                                        netInfo->mac[3], netInfo->mac[4], netInfo->mac[5]);
#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
    if(f_write(&cfgFile, buf, strlen(buf), &bw) != FR_OK)
#else
    if(!fwrite(buf, sizeof(char), strlen(buf), cfgFile))
#endif
      return 0;

    sprintf(buf, "%d.%d.%d.%d\n", netInfo->ip[0], netInfo->ip[1], netInfo->ip[2], netInfo->ip[3]);
#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
    if(f_write(&cfgFile, buf, strlen(buf), &bw) != FR_OK)
#else
    if(!fwrite(buf, sizeof(char), strlen(buf), cfgFile))
#endif
      return 0;

    sprintf(buf, "%d.%d.%d.%d\n", netInfo->sn[0], netInfo->sn[1], netInfo->sn[2], netInfo->sn[3]);
#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
    if(f_write(&cfgFile, buf, strlen(buf), &bw) != FR_OK)
#else
    if(!fwrite(buf, sizeof(char), strlen(buf), cfgFile))
#endif
      return 0;

    sprintf(buf, "%d.%d.%d.%d\n", netInfo->gw[0], netInfo->gw[1], netInfo->gw[2], netInfo->gw[3]);
#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
    if(f_write(&cfgFile, buf, strlen(buf), &bw) != FR_OK)
#else
    if(!fwrite(buf, sizeof(char), strlen(buf), cfgFile))
#endif
      return 0;

    sprintf(buf, "%d.%d.%d.%d\n", netInfo->dns[0], netInfo->dns[1], netInfo->dns[2], netInfo->dns[3]);
#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
    if(f_write(&cfgFile, buf, strlen(buf), &bw) != FR_OK)
#else
    if(!fwrite(buf, sizeof(char), strlen(buf), cfgFile))
#endif
      return 0;

#if defined(_USE_SDCARD_) && !defined(OS_USE_SEMIHOSTING)
    if(f_close(&cfgFile) != FR_OK)
#else
    if(fclose(cfgFile))
#endif
      return 0;

    return 1;
  }
  return 0;
}
