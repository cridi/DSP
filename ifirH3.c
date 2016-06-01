#include<stdio.h>
#define nc = 32

int main()
{
	
	int i = 0, zeiger = 0;

	float new_sample, y, b3[nc], circular_buffer[nc];

// Filterkoeffizienten

	b3[0] = 2.8439663e-03;
	b3[1] = 3.5574510e-03;
	b3[2] = 5.5673598e-03;
	b3[3] = 8.1087959e-03;
	b3[4] = 1.1187834e-02;
	b3[5] = 1.4768284e-02;
	b3[6] = 1.8780664e-02;
	b3[7] = 2.3117482e-02;
	b3[8] = 2.7637691e-02;
	b3[9] = 3.2177764e-02;
	b3[10] = 3.6550902e-02;
	b3[11] = 4.0568597e-02;
	b3[12] = 4.4044476e-02;
	b3[13] = 4.6815281e-02;
	b3[14] = 4.8743771e-02;
	b3[15] = 4.9733752e-02;
	b3[16] = 4.9733752e-02;
	b3[17] = 4.8743771e-02;
	b3[18] = 4.6815281e-02;
	b3[19] = 4.4044476e-02;
	b3[20] = 4.0568597e-02;
	b3[21] = 3.6550902e-02;
	b3[22] = 3.2177764e-02;
	b3[23] = 2.7637691e-02;
	b3[24] = 2.3117482e-02;
	b3[25] = 1.8780664e-02;
	b3[26] = 1.4768284e-02;
	b3[27] = 1.1187834e-02;
	b3[28] = 8.1087959e-03;
	b3[29] = 5.5673598e-03;
	b3[30] = 3.5574510e-03;
	b3[31] = 2.8439663e-03;

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
		y += (b3[i] * circular_buffer[(zeiger + 1) % nc]);

}