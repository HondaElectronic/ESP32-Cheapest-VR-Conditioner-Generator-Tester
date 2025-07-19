# ESP32-Cheapest-VR-Conditioner-Generator-Tester
This could be used to test Vr Conditioner boards, just proof of concept you should use dual channel signal generator

### This is just proof of concept but it might as well work in real life application ###
* This is still work in progress
* If you want you could add other systems, mising tooth etc

## I made this beacuse I wanted to test my lm1815 vr conditioner board but didnt have second channel on my arbitary generator and had esp32 laying around ##

# Limitations #
* Esp32 have only 2 dac's onboard, so if you want to generate 3 signals, you would need to use extra external dac or maybe mcu with 3 dac's
* Esp32 dac is only 8 bit so the signal is not ideal, its also noisy as you can see on the oscilloscope
* DAC generates lower voltage signal than real life magnetic sensor

# Hardware needed #
* 2x 10k-20k ohm resistor to pull down the signal to the gnd
* 2x 1uF non polar capacitor in series with signal's (also dont use non-polar electrolityc capacitors, tantalum, ceramic etc are fine)
* Optional:
* 100nF capacitor to filter the output signal
* esp32 of course

# To-Do #
* Make it actually works (There are some problems in current version with generation logic as well as html, I will update in near future)
* add support for external display, potentiometer, external 12bit dac
* Update the code so it supports different configurations such as missing tooth


![PXL_20250719_132511017 MP](https://github.com/user-attachments/assets/f91de46a-dcb9-4d25-bd55-f2e416bbf5d9)
![PXL_20250719_134212815](https://github.com/user-attachments/assets/58cf1886-b254-4d35-b99b-ac9b72b0e23f)
