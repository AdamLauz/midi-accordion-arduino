# Incorporating MIDI Functionality into an Acoustic Accordion

Accordions, traditionally recognized as acoustic musical instruments, operate by utilizing air pressure to induce vibrations in internal metallic reeds, resulting in the production of audible sound. In recent years, there has been a notable convergence of technology and music, leading to the development of digital accordions. This evolution encompasses the creation of entirely digital accordion instruments, as well as the integration of electronic components into conventional acoustic accordions. These advancements enable the generation of digital soundscapes reminiscent of those produced by digital keyboards.

The transformation of an acoustic accordion into a digital sound-producing instrument typically involves the integration of physical sensors within the accordion apparatus. These sensors serve as intermediaries between the mechanical actions of playing the accordion and the digital realm. They are responsible for capturing key press actions and transmitting corresponding signals. Subsequently, these signals are transformed into Musical Instrument Digital Interface (MIDI) messages, which then interface with a dedicated sound module to produce an extensive range of auditory tones and effects.

The primary objective of this endeavor is the conversion of a traditional acoustic accordion into a versatile digital musical instrument, capable of producing a wide array of sounds and effects.

The essential components employed in this project include:

1. Sensors: A total of 40 Hall effect sensors (specifically, 49E analog sensors) and 40 miniature magnets.
2. 1 X Arduino Uno microcontroller with a USB cable for interfacing and control.
3. 3 X 16-channel analog-to-digital multiplexers (CD74HC4067) to manage the sensor inputs efficiently.
4. An electronic matrix board to securely hold and interconnect the sensors.
5. A personal computer (PC) utilized as a sound module to process and produce the desired audio output.

## High level Architecture
The core of this system is centered around the Arduino Uno microcontroller, the sixteen-channel multiplexer, and the Hall effect sensors, as illustrated in the following Figure:
![image](https://github.com/AdamLauz/midi-accordion-arduino/assets/2620814/d865fcbd-199e-4894-ba15-5409a9702363)

## Detailed Diagram
The main components used are symbolized as follows:
![image](https://github.com/AdamLauz/midi-accordion-arduino/assets/2620814/e240dba7-c56c-4c04-926d-645c288f62e6)

The schematic diagram outlines the connections and interactions between the various components in the system. Notably, for clarity, the accordion itself is omitted from the diagram, focusing exclusively on the electrical circuitry.
![image](https://github.com/AdamLauz/midi-accordion-arduino/assets/2620814/bd13a7b5-b2c5-4770-aa57-bf71b5b0081e)

## How key press is captured?
Figure 2 serves to elucidate the method by which physical key presses on the accordion are captured and translated into digital signals. The linear-analog Hall effect sensors, such as the 49E model, produce signals of varying strength in response to the proximity of magnets.
![image](https://github.com/AdamLauz/midi-accordion-arduino/assets/2620814/ef4312c2-cfd3-40a3-ba5d-7bec7b19ba32)


## Pictures
Included below are visual representations of the project, providing a comprehensive view of the implemented system and its components.
![Screenshot 2023-09-24 232715](https://github.com/AdamLauz/midi-accordion-arduino/assets/2620814/6fc6f0f1-ede1-47ce-afe4-ec00670c1c9f)

![Screenshot 2023-09-24 232602](https://github.com/AdamLauz/midi-accordion-arduino/assets/2620814/3cbc1a69-f123-491e-ab92-fc65c2d3a5c7)

![Screenshot 2023-09-24 232444](https://github.com/AdamLauz/midi-accordion-arduino/assets/2620814/d6f23090-eaee-47b7-9529-96ccfbc28c76)


