/*
 * global_defs.h
 *
 *  Created on: Jul 26, 2018
 *      Author: Pradeepa.Senanayake
 */

#ifndef INCLUDE_GLOBAL_DEFS_H_
#define INCLUDE_GLOBAL_DEFS_H_

#define TOTAL_LOG_AMOUNT    (100)

#define CHANNEL_0_ECAP_MODULE   (ecapREG3)
#define CHANNEL_1_ECAP_MODULE   (ecapREG2)
#define ECAP_INTERRUPT_EVENT    (ecapInt_CEVT1)

extern unsigned int ch0_max;
extern unsigned int ch0_min;
extern unsigned int ch1_max;
extern unsigned int ch1_min;
extern unsigned char print_now;

#endif /* INCLUDE_GLOBAL_DEFS_H_ */
