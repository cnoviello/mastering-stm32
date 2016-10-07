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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ff.h"

uint8_t http_get_cgi_handler(uint8_t * uri_name, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_FAILED;
	uint16_t len = 0;

	if(strcmp((const char*)uri_name, "temp.cgi") == 0) {
	  sprintf((char*)buf, "%.2f", 27.34);
	  *file_len = strlen((char*)buf);
	  return HTTP_OK;
	} if(strcmp((const char*)uri_name, "listdirs.cgi") == 0) {
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

//	if(predefined_set_cgi_processor(uri_name, p_http_request->URI, buf, &len))
//	{
//		;
//	}
//	else if(strcmp((const char *)uri_name, "example.cgi") == 0)
//	{
//		// To do
//		val = 1;
//		len = sprintf((char *)buf, "%d", val);
//	}
//	else
//	{
//		// CGI file not found
//		ret = HTTP_FAILED;
//	}

	if(ret)	*file_len = len;
	return ret;
}
