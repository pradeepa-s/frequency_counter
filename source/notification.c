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
#define TOTAL_LOG_AMOUNT    (500)

unsigned int freq_counter_interrupt_seq = 0;
unsigned int selected_channel = 0;

volatile unsigned int cap0_periods = 0;
volatile unsigned int cap0_elapsed_time = 0;
double cap0_frequency = 0.0;
unsigned int cap0_skipped_first = 0;

unsigned int cap1_periods = 0;
unsigned int cap1_elapsed_time = 0;
unsigned int cap1_frequency = 0;
unsigned int cap1_skipped_first = 0;

double channel0_freq = 0;
double channel1_freq = 0;

uint8_t buf0_select = 1U;
uint8_t buf0_valid = 0U;
uint8_t buf1_valid = 0U;
unsigned int channel0_time_log0[TOTAL_LOG_AMOUNT];
unsigned int channel0_time_log1[TOTAL_LOG_AMOUNT];
unsigned int channel0_period_log0[TOTAL_LOG_AMOUNT];
unsigned int channel0_period_log1[TOTAL_LOG_AMOUNT];

unsigned int p_arr[100];

unsigned int channel0_index = 0;

uint8_t now_print = 0;
int print_timer = 0;

unsigned int max = 0;
unsigned int min = 16000000;

unsigned int cool_down = 101;

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

        if(print_timer > 8000){
            now_print = 1;
            print_timer = 0;
        }
        else{
            print_timer++;
        }

        /* 20-80 PWM After 20% */
        /* Start the frequency capture */
        if(selected_channel == 0 && freq_counter_interrupt_seq == 0){
            /* PWM 50-50 level high */
            gioSetBit(gioPORTA, 7,1); // J11, 19
            /* Start CHANNEL_0 */
            ecapClearFlag(CHANNEL_0_ECAP_MODULE, ecapInt_CEVT1);
            CHANNEL_0_ECAP_MODULE->ECCTL2 |= 0x0008U;
            ecapEnableInterrupt(CHANNEL_0_ECAP_MODULE, ecapInt_CEVT1);

            freq_counter_interrupt_seq++;
        }
        else if(selected_channel == 1 && freq_counter_interrupt_seq == 2){
            /* PWM 50-50 level low */

            /* Start CHANNEL_1 */
            ecapClearFlag(CHANNEL_1_ECAP_MODULE, ecapInt_CEVT1);
            ecapEnableInterrupt(CHANNEL_1_ECAP_MODULE, ecapInt_CEVT1);

            freq_counter_interrupt_seq++;
        }
    }
    else if(etpwmREG2 == node){

        gioToggleBit(gioPORTA, 0);  // J11, 7

        /* 20-80 PWM used for internal synch */
        if(selected_channel == 0 && freq_counter_interrupt_seq == 1){
            /* PWM 50-50 level high */
            gioSetBit(gioPORTA, 7,0); // J11, 19
            /* Stop CHANNEL_0 */
            ecapDisableInterrupt(CHANNEL_0_ECAP_MODULE, ecapInt_CEVT1);

            cap0_frequency = (double)((double)cap0_elapsed_time / (double)(cap0_periods));
           // cap0_frequency = cap0_frequency * 1000;
            if(cap0_frequency > 0){
                /* Calculate frequency of channel 0 */
                channel0_freq = (double)((double)VCLK4_FREQ * 1000000.0 * 2.0 / cap0_frequency);

                if(cool_down == 1){
                    if(cap0_frequency < min){
                        min = cap0_frequency;
                    }

                    if(cap0_frequency > max){
                        max = cap0_frequency;
                    }
                }
                else{
                        cool_down--;
                }

                if(channel0_index < TOTAL_LOG_AMOUNT){
                    if(buf0_select){
                        channel0_time_log0[channel0_index] = cap0_frequency;
                    }
                    else{
                        channel0_time_log1[channel0_index] = cap0_frequency;
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

            /* Stop CHANNEL_1 */
            ecapDisableInterrupt(CHANNEL_1_ECAP_MODULE, ecapInt_CEVT1);

            gioToggleBit(gioPORTA, 0);

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
    unsigned int capt1;

    if(CHANNEL_0_ECAP_MODULE == ecap && flags == 2){
        if(cap0_skipped_first){
            gioToggleBit(gioPORTA, 2); // J11, 12

            capt1 = ecapGetCAP1(ecap);
            cap0_elapsed_time += capt1;
            cap0_periods++;

#if 0
            if(cap0_periods == 100U){

                cap0_frequency = (double)((double)cap0_elapsed_time / (double)(cap0_periods));
                cap0_frequency = cap0_frequency * 1000;

                if(channel0_index < TOTAL_LOG_AMOUNT){
                    if(buf0_select){
                        channel0_time_log0[channel0_index] = cap0_frequency;
                    }
                    else{
                        channel0_time_log1[channel0_index] = cap0_frequency;
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


                cap0_periods = 0;
                cap0_elapsed_time = 0;
                cap0_frequency = 0;
            }
#endif
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

            capt1 = ecapGetCAP1(ecap);
            cap1_elapsed_time += capt1;
            cap1_periods++;
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
