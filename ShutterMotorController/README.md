# Shutter Motor Controller

As the name suggests, this project is about a connected controller for window shutter motors. 
It includes the schematics and PCB design as well as the firmware for the on-board ESP-12F 
module which was written in C++ using PlatformIO. As for the IoT part I have used Blynk but
the software is (in my opinion) well-modularized and easily be modified for other needs.

The firmware utilizes an NTP-controlled timer which can automatically open and close your
shutters (or control your connected appliance).

The controller has 4 reed relays so virtually it can interact with anything which has buttons
or any simple interface which can be driven by relays.

In my implementation I used 2 of these: https://www.amazon.de/gp/product/B074P186FX
![jarolift](https://images-na.ssl-images-amazon.com/images/I/61YMBwRKftL._SL1500_.jpg)

They are reasonably cheap and can be easily modified to interface with the controller 
by soldering a thin 4-wire ribbon cable to the test pads on the PCB of the up and down 
buttons and connecting them to the relays. The software has functions to pulse the relays 
simulating a button press.
