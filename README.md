# CapacitiveSliderKeyboard
2026 side project


# Overview

This project is about integrating capacitive touch control slider with keyboard. 
Including custom FPCB & control code. 


## Features

- Capacitive touch slider for horizontal and vertical scrolling
- Rotary encoder with scroll input
- Joystick
- button for mode switching
- LED feedback
- keyboard


## Hardware

Custom hausing was 3D printed on Bambulab P1S. Due to limitation with build plate size, It designed as three components. 
Upper plate was designed and laser cutted. 

Control panel, which includes button, wheel, and thumbstick, was designed as two parts. The wheel was assembled with two bearing and 3Pi-10mm shaft. 
As the magnetic encoder's disc was press-fitted on wheel itself. 



| Part | Description |
|---|---|
| Arduino Leonardo | HID keyboard/mouse controller |
| MPR121 | Capacitive touch sensor |
| Rotary Encoder | Scroll input |
| Joystick | Arrow keys or mouse pointer |
| LED | Mode feedback |






## Pin Map

| Component | Pin |
|---|---|
| LED | D10 |
| Button | D14 |
| Encoder A | D5 |
| Encoder B | D6 |
| MPR121 INT | D4 |
| Joystick X | A0 |
| Joystick Y | A1 |

## code & design availability
Code and FPCB design were uploaded. 
Keyboard housing and control panel PCB were also designed by author, but panel design includes altered shcemetic of other commercial parts, and the housing is based on substrate of original keyboard(Lenovo GK10 98-key). So, I decided not to upload them.
If needed, contact me. 







## Notes

The horizontal scroll function uses a custom HID report for real horizontal scrolling.
