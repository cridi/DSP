#include<stdio.h>
#define nc = 25

int main()
{
	
	int i = 0, zeiger = 0;

	float new_sample, y, circular_buffer[nc];

// Filterkoeffizienten

		float b1[] = {  1.7648976e-03,   3.9491542e-03,   7.8143952e-03,   1.3485435e-02,   2.1083438e-02,   3.0493677e-02,   4.1324118e-02,  5.2912672e-02,  6.4379824e-02,   7.4731119e-02,   8.2983173e-02,   8.8306333e-02,   9.0146552e-02,   8.8306333e-02,   8.2983173e-02,   7.4731119e-02,   6.4379824e-02,   5.2912672e-02,   4.1324118e-02,   3.0493677e-02,   2.1083438e-02,   1.3485435e-02,   7.8143952e-03,   3.9491542e-03,   1.7648976e-03}

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