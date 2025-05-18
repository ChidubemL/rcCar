/*
 * Copyright (c) 2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti/devices/msp/m0p/mspm0g350x.h"
#include "ti/driverlib/dl_gpio.h"
#include "ti/driverlib/m0p/dl_core.h"
#include "ti_msp_dl_config.h"
#include <string.h>

volatile uint8_t gEchoData = 0;
volatile bool gCheckADC;
volatile uint16_t joystickInputX;
volatile uint16_t joystickInputY;
char sendAscii;
const char* command = "AT+CON685E1C2B775F\r\n";
volatile bool commandSent = false;

// Function to send AT command over UART
void sendATCommand(const char* cmd) {
    uint16_t i = 0;
    while(cmd[i] != '\0') {
        // Wait until TX FIFO has space
        while(!DL_UART_Main_isTXFIFOEmpty(UART_0_INST));
        DL_UART_Main_transmitData(UART_0_INST, cmd[i]);
        i++;
    }
}

int main(void)
{
    SYSCFG_DL_init();

    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    DL_SYSCTL_enableSleepOnExit();

    gCheckADC = false;
    
    // Send the AT command once at startup
    sendATCommand(command);
    commandSent = true;
    
    // Simple delay to allow command to process
    for(volatile uint32_t i = 0; i < 100000; i++);

    while (1) {
        DL_ADC12_startConversion(ADC12_0_INST);

        joystickInputX = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
        if(joystickInputX > 3000) {
            sendAscii = '1';
            DL_UART_Main_transmitData(UART_0_INST, sendAscii);
        }
        else if (joystickInputX < 1000) {
            sendAscii = '0';
            DL_UART_Main_transmitData(UART_0_INST, sendAscii);
        }


        joystickInputY = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
        
        if(joystickInputY > 3000) {
            sendAscii = '3';
            DL_UART_Main_transmitData(UART_0_INST, sendAscii);
        }
        else if (joystickInputY < 1000) {
            sendAscii = '2';
            DL_UART_Main_transmitData(UART_0_INST, sendAscii);
        }
        gCheckADC = false;
        DL_ADC12_enableConversions(ADC12_0_INST);

    
    }
}


void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            gCheckADC = true;
            break;
        default:
            break;
    }
}


void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST)) {
        case DL_UART_MAIN_IIDX_RX:
            gEchoData = DL_UART_Main_receiveData(UART_0_INST);
            // Echo received data if needed
            // DL_UART_Main_transmitData(UART_0_INST, gEchoData);
            break;
        default:
            break;
    }
}