# CoffeeMeasurer

Arduino-based project to help brew the perfect coffee every time.

## Overview

This small hardware project was designed to take the guesswork out of knowing how much water should be added when brewing coffee. An Arduino interfaces with an HX711 connected to a 5kg load cell along with several LEDs and a push button to assist the user with making coffee. A configurable ratio dictates how strong the brewed coffee should be.

## Sequence

 When the Arduino is powered on:
 
 1. All connected input and output components are initialized, signaled by a red LED.
 2. The program waits for the user to add the desired amount of ground coffee to the scale and push the button, signaled by a blinking yellow LED and illuminated push button.
 3. The scale is queried for weight and the amount of water needed is calculated based on the configured strength ratio.
 4. The user is prompted to start adding water, signaled by a blinking blue LED.
 5. While water is being added, the scale weight is continuously checked to see if enough has been added.
 6. When the expected weight is detected, the user is signaled with a green LED and the microcontroller and scale go to sleep.

## Uploading the Sketch

Clone or download this repository, open `coffeemaker.ino` with the Arduino IDE, and upload it to the target Arduino board. This sketch also outputs some debug serial messages that can viewed with the Serial Monitor using a baud rate of 9600.

## Hardware
 
The hardware build for this project will be documented soon and a link added here.