# DSP project interpolated FIR Filter HSKa

## Goal

Interpolated FIR filter with 3 compontents H1, H2, H3. 

![](http://i.imgur.com/Y9e9tyq.jpg)

* Fs = 8000 Hz
* L1 = 14
* L2 = 8

### H1

* Fpass = 10 Hz
* Fstop1 = 15 Hz
* Stop band attenuation >= 60 dB
* Fs1 = Fs/(L1 * L2) ≈ 72 Hz

### H2

* Fpass = 10 Hz
* Fstop2 = Fs/(L1 * L2)-Fstop1 ≈ 57 Hz  
* Stop band attenuation >= 60 dB
* Fs2 = Fs/L1 ≈ 572 Hz 

### H3

* Fpass = 10 Hz
* Fstop3 = Fs/L1 - Fstop2 ≈ 514 Hz 
* Stop band attenuation >= 60dB
* Fs3 = Fs = 8000 Hz
