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
#define TOTAL_LOG_AMOUNT    (10000)

int freq_counter_interrupt_seq = 0;
int selected_channel = 0;

int cap0_periods = 0;
int cap0_elapsed_time = 0;
double cap0_frequency = 0.0;
int cap0_skipped_first = 0;

int cap1_periods = 0;
int cap1_elapsed_time = 0;
int cap1_frequency = 0;
int cap1_skipped_first = 0;

double channel0_freq = 0;
double channel1_freq = 0;

uint8_t buf0_select = 1U;
uint8_t buf0_valid = 0U;
uint8_t buf1_valid = 0U;
double channel0_time_log0[TOTAL_LOG_AMOUNT];
double channel0_time_log1[TOTAL_LOG_AMOUNT];
int channel0_index = 0;

#define CHANNEL_0_ECAP_MODULE   (ecapREG3)
#define CHANNEL_1_ECAP_MODULE   (ecapREG2)

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
    if(etpwmREG1 == node){
        /* 20-80 PWM After 20% */
        /* Start the frequency capture */
        if(selected_channel == 0 && freq_counter_interrupt_seq == 0){
            /* PWM 50-50 level high */

            /* Start eCAP1 */
            ecapClearFlag(CHANNEL_0_ECAP_MODULE, ecapInt_CEVT1);
            CHANNEL_0_ECAP_MODULE->ECCTL2 |= 0x0008U;
            ecapEnableInterrupt(CHANNEL_0_ECAP_MODULE, ecapInt_CEVT1);

            gioToggleBit(gioPORTA, 7);

            freq_counter_interrupt_seq++;
        }
        else if(selected_channel == 1 && freq_counter_interrupt_seq == 2){
            /* PWM 50-50 level low */

            /* Start eCAP2 */
            ecapClearFlag(CHANNEL_1_ECAP_MODULE, ecapInt_CEVT1);
            ecapEnableInterrupt(CHANNEL_1_ECAP_MODULE, ecapInt_CEVT1);

            gioToggleBit(gioPORTA, 0);

            freq_counter_interrupt_seq++;
        }
    }
    else if(etpwmREG2 == node){
        /* 20-80 PWM used for internal synch */
        if(selected_channel == 0 && freq_counter_interrupt_seq == 1){
            /* PWM 50-50 level high */

            /* Stop eCAP1 */
            ecapDisableInterrupt(CHANNEL_0_ECAP_MODULE, ecapInt_CEVT1);

            cap0_frequency = (double)((double)cap0_elapsed_time / (double)cap0_periods);
            if(cap0_frequency > 0){
                /* Calculate frequency of channel 0 */
                channel0_freq = (double)((double)VCLK4_FREQ * 1000000.0 / cap0_frequency);

                if(channel0_index < TOTAL_LOG_AMOUNT){
                    if(buf0_select){
                        channel0_time_log0[channel0_index] = channel0_freq;
                    }
                    else{
                        channel0_time_log1[channel0_index] = channel0_freq;
                    }
                    channel0_index++;
                }
                else{
                    if(buf0_select){
                        buf0_valid = 1;
                        buf0_select = 0;
                    }
                    else{
                        buf1_valid = 1;
                        buf0_select = 1;
                    }

                    channel0_index = 0;
                }
            }

            selected_channel = 1;
            freq_counter_interrupt_seq++;
        }
        else if(selected_channel == 1 && freq_counter_interrupt_seq == 3){
            /* PWM 50-50 level low */

            /* Stop eCAP2 */
            ecapDisableInterrupt(CHANNEL_1_ECAP_MODULE, ecapInt_CEVT1);

            gioToggleBit(gioPORTA, 0);
            //gioSetBit(gioPORTA, 0, 0);

            if(cap1_frequency > 0){
                /* Calculate frequency of channel 1 */
                channel1_freq = VCLK4_FREQ * 1000000 / cap1_frequency;
            }

            selected_channel = 0;
            freq_counter_interrupt_seq = 0;
        }

        /* Reset internal variables */
        cap0_periods = 0;
        cap0_elapsed_time = 0;
        cap0_frequency = 0;
        cap0_skipped_first = 0;

        cap1_periods = 0;
        cap1_elapsed_time = 0;
        cap1_frequency = 0;
        cap1_skipped_first = 0;

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
    if(CHANNEL_0_ECAP_MODULE == ecap && flags == 2){
        if(cap0_skipped_first){
            gioToggleBit(gioPORTA, 2);
            cap0_periods += 4;

            cap0_elapsed_time = cap0_elapsed_time + ecapGetCAP1(ecap) +
                    ecapGetCAP2(ecap) +
                    ecapGetCAP3(ecap) +
                    ecapGetCAP4(ecap);

          //  cap0_frequency += (cap0_elapsed_time / 4);

            if(cap0_periods > 4){
              //  cap0_frequency = cap0_frequency / 2;
            }

        }
        else{
            gioToggleBit(gioPORTA, 2);
            gioToggleBit(gioPORTA, 2);
            cap0_skipped_first = 1;
        }
    }
    else if(CHANNEL_1_ECAP_MODULE == ecap && flags == 2){
        if(cap1_skipped_first){
            gioToggleBit(gioPORTA, 1);
            cap1_periods += 4;

            cap1_elapsed_time = ecapGetCAP1(ecap) +
                    ecapGetCAP2(ecap) +
                    ecapGetCAP3(ecap) +
                    ecapGetCAP4(ecap);

            cap1_frequency += (cap1_elapsed_time / 4);

            if(cap1_periods > 4){
                cap1_frequency = cap1_frequency / 2;
            }
        }
        else{
            gioToggleBit(gioPORTA, 1);
            gioToggleBit(gioPORTA, 1);
            cap1_skipped_first = 1;
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
