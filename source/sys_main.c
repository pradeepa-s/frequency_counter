/** @file sys_main.c 
*   @brief Application main file
*   @date 07-July-2017
*   @version 04.07.00
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2016 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gio.h"
#include "esm.h"
#include "ecap.h"
#include "etpwm.h"
#include "sys_core.h"
#include "sci.h"
#include "global_defs.h"
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
#define DELAY_VAL   (0xFFFFF)

static void get_noise_floor(unsigned int samp1[], unsigned int samp[], uint32_t size);
static void print_number(unsigned int num);
static void print_stats(void);
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
    int delay = DELAY_VAL;
    char val[100];
    int32_t samples[100];
    uint32_t index = 0U;

    _disable_interrupt_();

    gioInit();
    esmInit();

    etpwmStopTBCLK();
    etpwmInit();

    etpwmDisableInterrupt(etpwmREG1);
    etpwmDisableInterrupt(etpwmREG2);
    etpwmDisableInterrupt(etpwmREG3);

    etpwmSetCount(etpwmREG1, 0);
    etpwmSetCount(etpwmREG2, 0);
    etpwmSetCount(etpwmREG3, 0);

    ecapInit();

    ecapDisableInterrupt(CHANNEL_1_ECAP_MODULE, ecapInt_CEVT1);
    ecapDisableInterrupt(CHANNEL_0_ECAP_MODULE, ecapInt_CEVT1);

    ecapClearFlag(CHANNEL_0_ECAP_MODULE, ecapInt_All);
    ecapClearFlag(CHANNEL_1_ECAP_MODULE, ecapInt_All);

    ecapSetCounter(CHANNEL_0_ECAP_MODULE, 0);
    ecapSetCounter(CHANNEL_1_ECAP_MODULE, 0);

    etpwmStartTBCLK();

    etpwmClearEventFlag(etpwmREG1, (Event_Interrupt | Event_SOCA | Event_SOCB));
    etpwmClearEventFlag(etpwmREG2, (Event_Interrupt | Event_SOCA | Event_SOCB));
    etpwmClearEventFlag(etpwmREG3, (Event_Interrupt | Event_SOCA | Event_SOCB));

    etpwmEnableInterrupt(etpwmREG1, CTR_UP_CMPB, EventPeriod_FirstEvent);   // PWM 20-80, 20%
    etpwmEnableInterrupt(etpwmREG2, CTR_ZERO, EventPeriod_FirstEvent);


    ecapEnableInterrupt(CHANNEL_0_ECAP_MODULE, ecapInt_CEVT1);

    _enable_interrupt_();

    sciInit();

    while(1){
        if(print_now){
            print_now = 0;
            print_stats();
        }
        while(delay--);
        delay = DELAY_VAL;
    }
/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */
static void print_stats(void){
    const uint8 str1[11] = "Ch0: M -> ";
    const uint8 str2[11] = "Ch0: m -> ";
    const uint8 str3[11] = "Ch1: M -> ";
    const uint8 str4[11] = "Ch1: m -> ";

    sciSend(scilinREG, sizeof(str1), str1);
    print_number(ch0_max);

    sciSend(scilinREG, sizeof(str2), str2);
    print_number(ch0_min);

    sciSend(scilinREG, sizeof(str3), str3);
    print_number(ch1_max);

    sciSend(scilinREG, sizeof(str4), str4);
    print_number(ch1_min);

    sciSendByte(scilinREG, '\n');
    sciSendByte(scilinREG, '\r');
}

static void get_noise_floor(unsigned int samp2[], unsigned int samp[], uint32_t size){
    uint32_t i = 0U;
    unsigned int max = samp[0];
    unsigned int min = samp[0];
    unsigned int avg = 0;

    for(i = 0U; i < size; i++){
        if(samp[i] > max){
            max = samp[i];
        }

        if(samp[i] < min){
            min = samp[i];
        }

        avg += samp[i];
    }

    avg /= size;
    print_number(avg);

}

static void print_number(unsigned int num){
    char str[100];
    ltoa(num, str);
    sciSend(scilinREG, strlen(str), (uint8*)str);
    sciSendByte(scilinREG, '\t');
}
/* USER CODE END */
