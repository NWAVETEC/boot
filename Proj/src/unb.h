/*
 * UNB.h
 *
 *  Created on: 06.11.2013
 *      Author: офис
 */

#ifndef UNB_H_
#define UNB_H_
   
unsigned char NWAVE_send(unsigned char *packet, unsigned char length, unsigned char* packetRec, unsigned char protocol);

#define PROTOCOL_B 0
#define PROTOCOL_E 1

typedef enum {
	NormalPower = 0,
	HighPower = 1,
	LowPower = 2
} UNB_power_typedef;

int NWAVE_Set_Frequency(unsigned long carrying_frequency, unsigned long bandwidth, int channels);


#endif /* UNB_H_ */
