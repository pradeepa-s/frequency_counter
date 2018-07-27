/** @file notification.c 
*   @brief User Notification Definition File
*   @date 07-July-2017
*   @version 04.07.00
*
*   This file  defines  empty  notification  routines to avoid
*   linker errors, Driver expects user to define the notification. 
*   The user needs to either remove this file and use their custom 
*   notification function or place their code sequence in this file 
*   between the provided USER CODE BEGIN and USER CODE END.
*
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


/* Include Files */

#include "esm.h"
#include "sys_selftest.h"
#include "gio.h"
#include "sci.h"
#include "etpwm.h"
#include "ecap.h"
#include "sys_dma.h"

/* USER CODE BEGIN (0) */

#include "global_defs.h"

static unsigned int cap0_periods = 0U;
static unsigned int cap0_elapsed_time = 0U;
static unsigned int cap0_count = 0U;
static unsigned int cap0_skipped_first = 0U;

static unsigned int cap1_periods = 0U;
static unsigned int cap1_elapsed_time = 0U;
static unsigned int cap1_count = 0U;
static unsigned int cap1_skipped_first = 0U;

unsigned int ch0_max = 0U;
unsigned int ch0_min = 0xFFFFFFFFU;
unsigned int ch1_max = 0U;
unsigned int ch1_min = 0xFFFFFFFFU;

unsigned int print_delay = 0;
unsigned char print_now = 0;

/* USER CODE END */
#pragma WEAK(esmGroup1Notification)
void esmGroup1Notification(uint32 channel)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (1) */
/* USER CODE END */
}

/* USER CODE BEGIN (2) */
/* USER CODE END */
#pragma WEAK(esmGroup2Notification)
void esmGroup2Notification(uint32 channel)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (3) */
/* USER CODE END */
}

/* USER CODE BEGIN (4) */
/* USER CODE END */
#pragma WEAK(memoryPort0TestFailNotification)
void memoryPort0TestFailNotification(uint32 groupSelect, uint32 dataSelect, uint32 address, uint32 data)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (5) */
/* USER CODE END */
}

/* USER CODE BEGIN (6) */
/* USER CODE END */
#pragma WEAK(memoryPort1TestFailNotification)
void memoryPort1TestFailNotification(uint32 groupSelect, uint32 dataSelect, uint32 address, uint32 data)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (7) */
/* USER CODE END */
}

/* USER CODE BEGIN (8) */
/* USER CODE END */
#pragma WEAK(gioNotification)
void gioNotification(gioPORT_t *port, uint32 bit)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (19) */
/* USER CODE END */
}

/* USER CODE BEGIN (20) */
/* USER CODE END */

#pragma WEAK(sciNotification)
void sciNotification(sciBASE_t *sci, uint32 flags)     
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (29) */
/* USER CODE END */
}

/* USER CODE BEGIN (30) */
/* USER CODE END */



/* USER CODE BEGIN (43) */
/* USER CODE END */

#pragma WEAK(etpwmNotification)
void etpwmNotification(etpwmBASE_t *node)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (44) */

    unsigned int en_pwm_ctr_val = 0U;

    en_pwm_ctr_val = etpwmREG3->TBCTR;
    /*
     * Interrupt sequence:
     *
     * 1. PWM-20-80 20% interrupt while PWM-50-50 is high (Channel 0 selected)
     * 2. PWM-20-80 100% interrupt while PWM-50-50 is high (Channel 0 selected) - Calculate freq for channel 0
     * 3. PWM-20-80 20% interrupt while PWM-50-50 is low (Channel 1 selected)
     * 4. PWM-20-80 100% interrupt while PWM-50-50 is low (Channel 1 selected) - Calculate freq for channel 1
     *
     */

    if(etpwmREG1 == node){

        if(print_delay == 0){
            print_now = 1;
            print_delay = 2000;
        }
        else{
            print_delay--;
        }

        if(en_pwm_ctr_val < 20000U){
            // Enable signal is high

            /* Start CHANNEL_0 */
            ecapClearFlag(CHANNEL_0_ECAP_MODULE, ECAP_INTERRUPT_EVENT);
            CHANNEL_0_ECAP_MODULE->ECCTL2 |= 0x0008U;
            ecapEnableInterrupt(CHANNEL_0_ECAP_MODULE, ECAP_INTERRUPT_EVENT);
        }
        else{
            // Enable signal is low

            /* Start CHANNEL_1 */
            ecapClearFlag(CHANNEL_1_ECAP_MODULE, ECAP_INTERRUPT_EVENT);
            CHANNEL_1_ECAP_MODULE->ECCTL2 |= 0x0008U;
            ecapEnableInterrupt(CHANNEL_1_ECAP_MODULE, ECAP_INTERRUPT_EVENT);
        }
    }
    else if(etpwmREG2 == node){

        if(en_pwm_ctr_val < 20000U){
            // Enable signal is high now, but the interrupt fired when EN was low

            /* stop CHANNEL 1 */
            ecapDisableInterrupt(CHANNEL_1_ECAP_MODULE, ECAP_INTERRUPT_EVENT);

            if(cap1_periods > 0U){
                cap1_count = (double)((double)cap1_elapsed_time / (double)(cap1_periods));

                if(ch1_max < cap1_count){
                    ch1_max = cap1_count;
                }

                if(ch1_min > cap1_count){
                    ch1_min = cap1_count;
                }
            }

            cap1_periods = 0U;
            cap1_elapsed_time = 0U;
            cap1_elapsed_time = 0U;
            cap1_skipped_first = 0U;
        }
        else{
            // Enable signal is low now, but the interrupt fired when EN was low
            ecapDisableInterrupt(CHANNEL_0_ECAP_MODULE, ECAP_INTERRUPT_EVENT);

            if(cap0_periods > 0U){
                cap0_count = (double)((double)cap0_elapsed_time / (double)(cap0_periods));

                if(ch0_max < cap0_count){
                    ch0_max = cap0_count;
                }

                if(ch0_min > cap0_count){
                    ch0_min = cap0_count;
                }
            }

            cap0_periods = 0U;
            cap0_elapsed_time = 0U;
            cap0_elapsed_time = 0U;
            cap0_skipped_first = 0U;
        }
    }
/* USER CODE END */
}
#pragma WEAK(etpwmTripNotification)
void etpwmTripNotification(etpwmBASE_t *node,uint16 flags)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (45) */
/* USER CODE END */
}

/* USER CODE BEGIN (46) */
/* USER CODE END */

/* USER CODE BEGIN (47) */
/* USER CODE END */


/* USER CODE BEGIN (50) */
/* USER CODE END */

#pragma WEAK(ecapNotification)
void ecapNotification(ecapBASE_t *ecap,uint16 flags)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (51) */
    unsigned int capt1 = 0U;

    if((CHANNEL_0_ECAP_MODULE == ecap) && (flags == ecapInt_CEVT1)){
        if(cap0_skipped_first){
            capt1 = ecapGetCAP1(ecap);
            cap0_elapsed_time += capt1;
            cap0_periods++;
        }
        else{
            cap0_skipped_first = 1U;
        }
    }
    else if((CHANNEL_1_ECAP_MODULE == ecap) && (flags == ecapInt_CEVT1)){
        if(cap1_skipped_first){
            gioToggleBit(gioPORTA, 1);

            capt1 = ecapGetCAP1(ecap);
            cap1_elapsed_time += capt1;
            cap1_periods++;
        }
        else{
            cap1_skipped_first = 1U;
        }
    }
/* USER CODE END */
}
/* USER CODE BEGIN (52) */
/* USER CODE END */

/* USER CODE BEGIN (53) */
/* USER CODE END */

#pragma WEAK(dmaGroupANotification)
void dmaGroupANotification(dmaInterrupt_t inttype, uint32 channel)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (54) */
/* USER CODE END */
}
/* USER CODE BEGIN (55) */
/* USER CODE END */

/* USER CODE BEGIN (56) */
/* USER CODE END */

/* USER CODE BEGIN (58) */
/* USER CODE END */

/* USER CODE BEGIN (60) */
/* USER CODE END */
