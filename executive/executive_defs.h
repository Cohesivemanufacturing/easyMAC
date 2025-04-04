#pragma once

/***************************************************
* easyMAC. executive constants and macros 1.0      *
*                                                  *
*
* Jorge Correa & Placid Ferreira                   *
* 2017                                             *
*                                                  *
****************************************************/

/**************************************************************
Message Handling
***************************************************************/

#define MSG_INICONFIG		    100
#define MSG_PROGCHECK           120
#define MSG_STARTPROG           130 
#define MSG_EXITPROG	        140 
#define MSG_STARTINTP           150
#define MSG_OPENPROG            141
#define MSG_EXITINTP            160
#define MSG_CONINTP             170
#define MSG_CONSOCKET           171
#define MSG_SOCKETCREATED       172
#define MSG_CONCLNCSOCKET       173
#define MSG_CLNCSOCKETCREATED   174
#define MSG_STARTVIRTMAC        180
#define MSG_EXITVIRTMAC         190
#define MSG_STARTCLOUDNC        191
#define MSG_EXITCLOUDNC         192
 
/**************************************************************
Error Handler
***************************************************************/

#define ERR_CONFIGFILE          100
#define ERR_INICONFIG		    110
#define ERR_PROGCHECK           120  
#define ERR_STARTPROG           130 
#define ERR_EXITPROG	        140 
#define ERR_OPENPROG            141
#define ERR_STARTINTP           150
#define ERR_EXITINTP            160
#define ERR_MULTINTP            161
#define ERR_CONINTP             170
#define ERR_DISCSOCKET          171
#define ERR_SOCKETCREATED       172
#define ERR_DISCCLNCSOCKET      173
#define ERR_CLNCSOCKETCREATED   174
#define ERR_COMPORTAV           180
#define ERR_STARTVIRTMAC        190
#define ERR_EXITVIRTMAC         200
#define ERR_MULTVIRTMAC         210
#define ERR_STARTCLOUDNC        211
#define ERR_EXITCLOUDNC         212
#define ERR_MULTCLOUDNC         213

