# CC-CNC
Credit Card sized CNC Controller board

![PCB Picture](https://github.com/twatorowski/CC-CNC/blob/master/Hardware/IMG_1246.JPG)

Simple and Small CNC Controller. Features are:

### Hardware

- Small form factor: 52x85mm (just the same as standard credit/debit cards are)
- 4 independent axes driven by TI DRV8825
- Configurable Motor currents (set by DAC in software)
- Configurable stepping mode (set in software)
- Isolated (ADuM4160) USB interface (enumerates as virtual com port)
- 8 limit switches are supported
- PWM 5V output for spindle control
- Probe input

### Software

- 50kHz update interrupt frequency
- Supports Linear Feed, Arc Feed (in both formats), Dwelling, etc.
- 9 coordinate systems
- coordinate offsets
- plane selection
- feed rate and inverse feed rate, all compensated for diagonal motion
- simple acceleration/decceleration profile
- Bresenham algorithms for line and arc motion (no segmenting)
- all fixed point arithmetic, no floats, no 'libm' functions


### TODO
- S-curve acceleration model
- stepping mode change with G-code command
- current setting with G-code command
- position and status feedback over USB
