/*
===============================================================================
 Name        : nrf24l01.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>

// TODO: insert other include files here
#include "nrf24hal.h"
#include "nrf24lib.h"

// TODO: insert other definitions and declarations here
int main(void)
{
	SystemInit();

	SystemCoreClockUpdate();

    nrf24Init();

    while(1);
}
