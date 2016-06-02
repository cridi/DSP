# DSP project interpolated FIR Filter HSKa

## Goal

Interpolated FIR filter with 3 compontents H1, H2, H3. 
Fs = 8000 Hz
L1 = 14
L2 = 8

### H1

* Fpass = 10 Hz
* Fstop1 = 15 Hz
* Stop band attenuation >= 60 dB
* Fs1 = Fs/(L1 * L2)

### H2

* Fpass = 10 Hz
* Fstop2 = Fs/(L1 * L2)-Fstop1
* Stop band attenuation >= 60 dB
* Fs2 = Fs/L1

### H3

* Fpass = 10 Hz
* Fstop3 = Fs/L1 - Fstop2
* Stop band attenuation >= 60dB
* Fs3 = Fs
