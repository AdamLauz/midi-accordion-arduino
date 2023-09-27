# Add midi support to acoustic accordion

Accordions are usually made as acoustical instruments. They generate air pressure which makes internal metal reeds to vaibrate and produce sound. Some companies produce digital accordions or add electronics to an acoustic accordion in order to produce digital sounds similarly to digital keyboards. A common way to do this is to use physical sensors connected to a digital converter to convert signals from the sensors to MIDI messages that are in turn fed into a sound module to produce sounds. 
The goal of this project is to transform an acoustic accordion to produce digital sounds. 

To achieve this I used the following components:

1. Sensors: 40 X hall effects (49E analog) and 40 tiny magnets
2. 1X Arduino uno + Usb cable
3. 3 X 16 channel analogdigital multiplexers (CD74HC4067)  
4. Electronic Matrix board (hold and connect the sensors).
5. PC (operated as a sound module)

## Diagram
![image](https://github.com/AdamLauz/midi-accordion-arduino/assets/2620814/d865fcbd-199e-4894-ba15-5409a9702363)


![Screenshot 2023-09-24 232715](https://github.com/AdamLauz/midi-accordion-arduino/assets/2620814/6fc6f0f1-ede1-47ce-afe4-ec00670c1c9f)

![Screenshot 2023-09-24 232602](https://github.com/AdamLauz/midi-accordion-arduino/assets/2620814/3cbc1a69-f123-491e-ab92-fc65c2d3a5c7)

![Screenshot 2023-09-24 232444](https://github.com/AdamLauz/midi-accordion-arduino/assets/2620814/d6f23090-eaee-47b7-9529-96ccfbc28c76)


