# AED-trainer-with-Arduino
 Build an AED trainer with Arduino Pro Mini

# CAUTION
Warning! This is a self-made AED trainer, it's NOT a functional Automatic External Defibrillator! DO NOT ATTEMPT TO APPLY IT TO ANY MEDICAL USAGE

# Introduction
Automatic External Defibrillator, or AED, defib, is an important tool during CPR.
To help the publics understand when to use an AED and how it work, it's necessary to use an AED trainer as examples.

# Materials
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

# Audio Files
To simulate the vocal instructions of a regular AED, some audio files are required.
These files should comply the requiremnets of DFPlayer Mini.
To store the audio files to the micro SD card, you might need a card reader.
The content and the file names are listed as follow:

"System booting up" (File: 0001.mp3)

"Stay calm and call 911" (0002.mp3)

"Plug the electrode and apply the pads to bare skin as instructions" (0003.mp3)

"Evaluating heart rhythm, do not touch the patient" (0004.mp3)

"Shock recommended, press the shock button to deliver shocks" (0005.mp3)

"Shock delivered, continue CPR" (0006.mp3)

"No shock advised, please continue CPR" (0007.mp3)

#The audio files are not included in this project and should be prepared by your own.

# IR Remote control
The signals of the remote controls are different among them, you should run the "IRreceive" demo to figure out the corresponding signals and the buttons.
To connect the Arduino Pro Mini to your PC, you might need a USB to UART tool such as CP2102 or CH340G.


The wiring is shown as the PNG file.

The .ino file "AED_trainer" is the code for the Arduino Mini Pro (ATmega 328 5V/16MHz).

# Acknowledgement

This project used some examples from Angelo qiao and [Ken Shirriff].
