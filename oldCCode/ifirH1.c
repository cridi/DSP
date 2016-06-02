#include<stdio.h>
#define nc = 29

int main()
{
	
	int i = 0, zeiger = 0;

	float new_sample, y, circular_buffer[nc];

// Filterkoeffizienten

	float b1[] = {  -2.5413679e-04,  -5.6653231e-03,  -1.4933878e-02,
	-2.2070978e-02,  -1.6652246e-02,   4.3202916e-03,   2.7256538e-02,   
	2.8062394e-02,  -5.0881050e-03,  -5.0905901e-02,  -6.0653001e-02,
	5.3770247e-03,   1.3765225e-01,   2.7149231e-01,   3.2789058e-01,
	2.7149231e-01,   1.3765225e-01,   5.3770247e-03,  -6.0653001e-02,
	-5.0905901e-02,  -5.0881050e-03,   2.8062394e-02,   2.7256538e-02,
	4.3202916e-03,  -1.6652246e-02,  -2.2070978e-02,  -1.4933878e-02,
   -5.6653231e-03,  -2.5413679e-04
}

	for (i = 0; i<nc; i++)
		circular_buffer[i] = 0;

// Der folgende Code wird jedes Mal ausgeführt, wenn ein neuer
// Eingangswert (new_sample) zur Verfügung steht

// Schreibe neuen Eingangswert in Buffer

	circular_buffer[zeiger] = new_sample;
// Inkrementiere Zeiger modulo nc

	zeiger = (zeiger + 1) % nc;

// Berechne neuen Ausgangswert

	y = 0;

	for (i = 0; i<nc; i++)
		y += (b1[i] * circular_buffer[(zeiger + 1) % nc]);

}