//H1 Filter für DSP interpolierter FIT Filter, Implementation in C
//Boris Kuznetsov
//fpass=10Hz -- fstop = 15Hz -- stop band attenuation >= 60dB

#include<stdio.h>
#define NC 29
#define L1 14
#define L2 8
#define FS 72 //Samplerate, 8e3/(L1*L2) 

int main(){
    int i = 0, z = 0, nsh = 0;
    float ns, out, buf[NC*L1*L2];

//Koeffizienten

float b1[] = {  -2.5413679e-04,  -5.6653231e-03,  -1.4933878e-02,
-2.2070978e-02,  -1.6652246e-02,   4.3202916e-03,   2.7256538e-02,   
2.8062394e-02,  -5.0881050e-03,  -5.0905901e-02,  -6.0653001e-02,
5.3770247e-03,   1.3765225e-01,   2.7149231e-01,   3.2789058e-01,
2.7149231e-01,   1.3765225e-01,   5.3770247e-03,  -6.0653001e-02,
-5.0905901e-02,  -5.0881050e-03,   2.8062394e-02,   2.7256538e-02,
4.3202916e-03,  -1.6652246e-02,  -2.2070978e-02,  -1.4933878e-02,
-5.6653231e-03,  -2.5413679e-04
};

for (i=0;i<NC*L1*L2;i++){ //Buffer initialisieren
    buf[i]=0;
}

//Neuer Wert

buf[z] = ns; //Neuen Eingangswert in den Buffer schreiben

z = (z+1) % (NC*L1*L2); //Zeiger inkrementieren und auf Null setzen wenn Ende

//ns++; //Nullstellen-Zähler inkrementieren 
//Neuen Ausgangswert berechnen

out=0;

for(i = 0; i < NC*L1*L2; i++){
    if(nsh == 0) out += (b1[i] * buf[(z+i)%NC]); //nur jeden 112ten Wert verrechnen
    nsh=(nsh+1)%(L1*L2); //ns inkrementieren und auf 0 setzen wenn 112 erreicht wurde. 
    }
}
