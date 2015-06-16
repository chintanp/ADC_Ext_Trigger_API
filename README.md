# ADC_Ext_Trigger_API
Uses TIVAware API for ADC to UART using External Trigger

Main file ADC_Ext_Trigger_API.c, inspired from https://sites.google.com/site/luiselectronicprojects/tutorials/tiva-tutorials/tiva-gpio/simple-digital-input and https://sites.google.com/site/luiselectronicprojects/tutorials/tiva-tutorials/tiva-adc/internal-temperature-sensor 

Samples ADC and sends to UART. 
=============================

Hardware configuration:

1. Connect a slide potentiometer to PE2, (Pin 3 of pot goes to 3.3V, Pin 1 goes to GND and Pin 2 to PE2)
2. ADC Trigger on PF4, the left switch(SW1) on launchpad. 

Works with: 

1. KEIL uVision v4.74.0.22
2. TIVA-C LaunchPad - TM4C123GXL
3. uC: TM4C123GH6PM
4. The folder structure should be maintained as provided with Tivaware C Series/examples/boards/tm4c123gxl/project0

This project made my copying project0 and editing the C file, all other settings left unchanged. 

Current Issues: 

1. The ADC keeps sampling if the switch is kept pressed, in-place of the desired behavior of single sample per switch press
2. The ADC, sometimes, reports incorrect value (old value), if sampled too quickly after value change. 

