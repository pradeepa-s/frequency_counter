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
#define TOTAL_LOG_AMOUNT    (10000)

extern int channel0_freq;
extern int channel1_freq;
extern uint8_t buf0_select;
extern double channel0_time_log0[TOTAL_LOG_AMOUNT];
extern double channel0_time_log1[TOTAL_LOG_AMOUNT];
extern uint8_t buf0_valid;
extern uint8_t buf1_valid;


static void get_noise_floor(double samp[], uint32_t size);
static void print_number(int32_t num);
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
    int delay = DELAY_VAL;
    char val[100];
    int32_t samples[100];
    uint32_t index = 0U;


    gioInit();
    esmInit();

    _enable_interrupt_();

    etpwmInit();

    etpwmSetCounterMode(etpwmREG1, CounterMode_Up);
    etpwmSetCount(etpwmREG1, 0);
    etpwmSetSyncOut(etpwmREG1, SyncOut_CtrEqZero);

    etpwmSetCount(etpwmREG2, 0);
    etpwmEnableCounterLoadOnSync(etpwmREG2, 0, 0);
    etpwmSetSyncOut(etpwmREG2, SyncOut_CtrEqZero);

    etpwmSetCount(etpwmREG3, 0);
    //etpwmEnableCounterLoadOnSync(etpwmREG3, 0, 0);
    //etpwmActionQualConfig_t pwm3a_50_50_action_qualifier;

    //pwm3a_50_50_action_qualifier.CtrEqPeriod_Action = ActionQual_Toggle;
   //pwm3a_50_50_action_qualifier.CtrEqZero_Action = ActionQual_Disabled;

   // etpwmSetActionQualPwmA(etpwmREG3, pwm3a_50_50_action_qualifier);

    ecapInit();
    ecapDisableInterrupt(ecapREG2, ecapInt_CEVT1);
    ecapDisableInterrupt(ecapREG3, ecapInt_CEVT1);

    etpwmStartTBCLK();

    sciInit();

    while(1){
        if(channel0_freq){
            //ftoa(channel0_freq, val);
            //sciSend(scilinREG, strlen(val), (uint8*)val);
            //sciSendByte(scilinREG, '\n');
            //sciSendByte(scilinREG, '\r');

            if(buf0_select && buf0_valid){
                buf0_valid = 0;
                get_noise_floor(channel0_time_log1, TOTAL_LOG_AMOUNT);
            }

            if(buf0_select == 0 && buf1_valid){
                buf1_valid = 0;
                get_noise_floor(channel0_time_log0, TOTAL_LOG_AMOUNT);
            }

        }
        gioToggleBit(gioPORTB, 1);
        while(delay--);
        delay = DELAY_VAL;
    }
/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */
static void get_noise_floor(double samp[], uint32_t size){
    uint32_t i = 0U;
    double max = samp[0];
    double min = samp[0];

    for(i = 0U; i < size; i++){
        if(samp[i] > max){
            max = samp[i];
        }

        if(samp[i] < min){
            min = samp[i];
        }
    }


    sciSendByte(scilinREG, 'M');
    print_number(max);
    sciSendByte(scilinREG, 'm');
    print_number(min);
    sciSendByte(scilinREG, 'd');
    print_number(max - min);

}

static void print_number(int32_t num){
    char str[100];
    ltoa(num, str);
    sciSend(scilinREG, strlen(str), (uint8*)str);
    sciSendByte(scilinREG, '\n');
    sciSendByte(scilinREG, '\r');
}
/* USER CODE END */
