/*
 * config.h
 *
 *  Created on: 31 ott 2016
 *      Author: cnoviello
 */

#ifndef CONFIG_H_
#define CONFIG_H_

uint8_t ReadNetCfgFromFile(wiz_NetInfo *netInfo);
uint8_t WriteNetCfgInFile(wiz_NetInfo *netInfo);

#endif /* CONFIG_H_ */
