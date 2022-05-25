/***************************************************************************************************
                                   ExploreEmbedded    
****************************************************************************************************
 * File:   rtc.c
 * Version: 15.0
 * Author: ExploreEmbedded
 * Website: http://www.exploreembedded.com/wiki
 * Description: Contains the library functions for RTC-Ds1307 date and time read write

The libraries have been tested on ExploreEmbedded development boards. We strongly believe that the 
library works on any of development boards for respective controllers. However, ExploreEmbedded 
disclaims any kind of hardware failure resulting out of usage of libraries, directly or indirectly.
Files may be subject to change without prior notice. The revision history contains the information 
related to updates. 


GNU GENERAL PUBLIC LICENSE: 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

Errors and omissions should be reported to codelibraries@exploreembedded.com
***************************************************************************************************/



/***************************************************************************************************
                             Revision History
****************************************************************************************************
15.0: Initial version 
***************************************************************************************************/

#include "rtc.h"
#include "i2c.h"



void RTCinit() {
	i2cInit();
	i2cStart();
	i2cSendData(Device_Write_address);
	i2cSendData(C_Ds1307ControlRegAddress_U8);
	i2cSendData(C_Ds1307SecondRegAddress_U8 );
	i2cStop();
}

void RTC_SetDateTime(rtct *rtc) {
	i2cStart();
	i2cSendData(Device_Write_address);
	i2cSendData(C_Ds1307SecondRegAddress_U8 );
	i2cSendData(rtc->sec);
	i2cSendData(rtc->min);
	i2cSendData(rtc->hour);
	i2cSendData(rtc->weekDay);
	i2cSendData(rtc->date);
	i2cSendData(rtc->month);
	i2cSendData(rtc->year);
	
	
	i2cStop();
}

void RTC_GetDateTime(rtct *rtc) {
	i2cStart();
	i2cSendData(Device_Write_address);
	i2cSendData(C_Ds1307SecondRegAddress_U8 );
	i2cStop();
	i2cStart();
	i2cSendData(Device_Read_address);
	
	rtc->sec = i2cReadDataAck();
	rtc->min = i2cReadDataAck();
	rtc->hour = i2cReadDataAck();
	rtc->weekDay = i2cReadDataAck();
	rtc->date = i2cReadDataAck();
	rtc->month = i2cReadDataAck();
	rtc->year = i2cReadDataNotAck();
	
	i2cStop();
}