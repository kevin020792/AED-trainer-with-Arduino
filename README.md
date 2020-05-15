# AED-trainer-with-Arduino
 Build an AED trainer with Arduino Pro Mini
//CAUCION

Warning! This is a self-made AED trainer, it's NOT a functional Automatic External Defibrillator!
DO NOT ATTEMPT TO APPLY IT TO ANY MEDICAL USAGE

//Introduction
Automatic External Defibrillator, or AED, defib, is an important tool during CPR.
To help the publics understand when to use an AED and how it work, it's necessary to use an AED trainer as examples.

//Materials
This project requires the items as follow:

Arduino Pro Mini X1
DFPlayer Mini Module X1
microSD card X1
5V LED X3
IRM-3638N3 IR Receiver X1
IR Remote control X1
Momentary Switch with LED X1
Seesaw Switch with LED X1
5V 3W 4Ω Speaker X1
1/4W 220Ω Resistor X5
1/4W 1KΩ Resistor X1
1/4W 10KΩ Resistor X1 
3W 4Ω Speaker X1
5V buzzer X1
LM2596 DC-DC module X1 (Calibrated to 5V output)
9V battery X1
Wires and a circuit board
Suitable case for all the electronics
Some voice files that can simulate an working AED, for instance: "Apply the pads to the bare skin as instructions", "Do not touch the patient"...and so on. (Not included in this github project, they should be prepared by your own.)

To connect the Arduino Pro Mini to your PC, you might need a USB to UART tool such as CP2102 or CH340G.
To store the audio files to the micro SD card, you might need a card reader.

The wiring is shown as the PNG file.
The .ino file "AED_trainer" is the code for the Arduino Mini Pro (ATmega 328 5V/16MHz).


//Acknowledgement
This project used some examples from [Angelo qiao](Angelo.qiao@dfrobot.com) and [Ken Shirriff].
