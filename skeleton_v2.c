#include <Micki_Mouscfg.h>
#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include <csl_edma.h>
#include <dsk6713_led.h>
#include "config_AIC23.h"
#include "skeleton.h"



void config_EDMA(void);
void config_interrupts(void);
void EDMA_interrupt_service(void);


float Anfangswert1[4] = {0, 0, 0, 0}; //Anfangswerte für die 1. Filterung
float Anfangswert2[4] = {0, 0, 0, 0}; //Anfangswerte für die 2. Filterung
float b0[6] = {1, 1, 1, 1, 1, 1};
float b1[6] = {-1.6788673534151195, -1.6346268720830199, -1.5201210004449062, -1.2479839907798393, -0.52221716526519524, 1.3373430894196379};
float b2[6] = {1, 1, 1, 1, 1, 1};
//float a0[6] = {1, 1, 1, 1, 1, 1}; //////////////////////////////////////////////////////////// nicht verwendet
float a1[6] = {-1.7362690969448493, -1.6002124824775663, -1.4419850104112946, -1.2580043723019878, -1.0706357084671256, -0.94212372090903207};
float a2[6] = {0.93096376999187525, 0.7962254843543578, 0.65225836243823876, 0.49316869993845475, 0.33542626753492216, 0.2285504938712844};
float V[6]  = {0.60627493067904281, 0.53647350311249387, 0.43817994165590263, 0.31271186351515579, 0.17918096816662138, 0.085824790945321042};
float Verstaerkung = 0.000685;
float Abtastfrequenz = 48000; //muss noch geändert werden auf die echte Abtastfrequenz
float Abtastzeit; // Nicht sicher ob das benötigt wird.

int fg = 3000;
#define PI 3.14159265359

float Speicher[BUFFER_LEN];
float Speicher2[BUFFER_LEN];

main()
{
	CSL_init();  /* init CSL */
	
	DSK6713_init();

	/* Konfiguration des AIC23 über McBSP0 */
	Config_DSK6713_AIC23();
	
	/* Konfiguration des McBSP1 - Datenschnittstelle */
	hMcbsp = MCBSP_open(MCBSP_DEV1, MCBSP_OPEN_RESET);
    MCBSP_config(hMcbsp, &datainterface_config);
    
    IRQ_globalDisable();       // globale Interrupt nicht zulassen

    config_EDMA();

    /* interrupts immer zuletzt */
    config_interrupts();

    MCBSP_start(hMcbsp,  MCBSP_XMIT_START | MCBSP_RCV_START, 0xffffffff);
    MCBSP_write(hMcbsp, 0x0); /* einmal schießen */



} /* und fertig */



void config_EDMA(void)
{
	/* Konfiguration der EDMA zum Lesen*/
	hEdmaRcv = EDMA_open(EDMA_CHA_REVT1, EDMA_OPEN_RESET);  // EDMA Kanal für das Event REVT1
	hEdmaReloadRcvPing = EDMA_allocTable(-1);               // hEdmaReloadRcvPing handle
	hEdmaReloadRcvPong = EDMA_allocTable(-1);               // hEdmaReloadRcvPong handle

	configEDMARcv.src = MCBSP_getRcvAddr(hMcbsp);          //  Quell-Adresse zum Lesen

	tccRcvPing = EDMA_intAlloc(-1);                        // nächsten freien Transfer-Complete-Code
	configEDMARcv.opt |= EDMA_FMK(OPT,TCC,tccRcvPing);     // dann der Grundkonfiguration des EDMA Empfangskanals zuweisen
	
	/* ersten Transfer und Reload-Ping mit ConfigPing konfigurieren */
	EDMA_config(hEdmaRcv, &configEDMARcv);
	EDMA_config(hEdmaReloadRcvPing, &configEDMARcv);
	/* braucht man auch noch andere EDMA-Konfigurationen fuer das Lesen? */
	
	configEDMARcv.dst = EDMA_DST_OF(Buffer_in_pong);       // wechsle Struktur um Ziel ping zu haben
	tccRcvPong = EDMA_intAlloc(-1);                        // nächsten freien Transfer-Complete-Code
	configEDMARcv.opt &= 0xfff0000ffff;  /// BITMASKE DER TCC-Bit anpassen!!!!!
	configEDMARcv.opt |= EDMA_FMK(OPT,TCC,tccRcvPong);     // dann der Grundkonfiguration des EDMA Empfangskanals zuweisen

	EDMA_config(hEdmaReloadRcvPong, &configEDMARcv);       // konfiguriere reload für pong

	/* Transfers verlinken ping -> pong -> ping */

	EDMA_link(hEdmaRcv,hEdmaReloadRcvPong);                 // link auf ping
	EDMA_link(hEdmaReloadRcvPing,hEdmaReloadRcvPong);       // link Ping auf Pongg
	EDMA_link(hEdmaReloadRcvPong,hEdmaReloadRcvPing);       // and link Pong zu Ping





	/*+++++++++++ Schreiben ++++++++++++++++ */

	hEdmaXmt = EDMA_open(EDMA_CHA_XEVT1, EDMA_OPEN_RESET);  // EDMA Kanal für das Event XEVT1
	hEdmaReloadXmtPing = EDMA_allocTable(-1);               // hEdmaReloadXmtPing handle
	hEdmaReloadXmtPong = EDMA_allocTable(-1);               // hEdmaReloadXmtPong handle

	configEDMAXmt.dst = MCBSP_getXmtAddr(hMcbsp);         // Ziel-Adresse von hMcbsp

	tccXmtPing = EDMA_intAlloc(-1); // nächster Transfer-Complete-Code
	configEDMAXmt.opt |= EDMA_FMK(OPT,TCC,tccXmtPing);       // Grundfunktion

	EDMA_config(hEdmaXmt, &configEDMAXmt);                 // then configure the registers
	EDMA_config(hEdmaReloadXmtPing, &configEDMAXmt);       // and the reload for Ping

	configEDMAXmt.src = EDMA_SRC_OF(Buffer_out_pong);       // change the structure to have a source of Pong
	tccXmtPong = EDMA_intAlloc(-1);                        // nächsten freien Transfer-Complete-Code
	configEDMARcv.opt &= 0xfff0000ffff;         // BITMASKE DER TCC-Bit anpassen, passt das so?!
	configEDMARcv.opt |= EDMA_FMK(OPT,TCC,tccXmtPong);     // dann der Grundkonfiguration des EDMA Empfangskanals zuweisen

	EDMA_config(hEdmaReloadXmtPong, &configEDMAXmt);       // and configure the reload for Pong

	EDMA_link(hEdmaXmt,hEdmaReloadXmtPong);                 // link the regs to Pong
	EDMA_link(hEdmaReloadXmtPing,hEdmaReloadXmtPong);       // link Pong to Ping
	EDMA_link(hEdmaReloadXmtPong,hEdmaReloadXmtPing);       // and link Ping to Pong


	/* EDMA TCC-Interrupts freigeben */
	EDMA_intClear(tccRcvPing);
	EDMA_intClear(tccRcvPong);
	EDMA_intClear(tccXmtPing);
	EDMA_intClear(tccXmtPong);

	EDMA_intEnable(tccRcvPing);
	EDMA_intEnable(tccRcvPong);
	EDMA_intEnable(tccXmtPing);
	EDMA_intEnable(tccXmtPong);
	/* sind das alle? */

	/* EDMA starten, wen alles? */
	EDMA_enableChannel(hEdmaRcv);
	EDMA_enableChannel(hEdmaXmt);
}


void config_interrupts(void)
{
	IRQ_map(IRQ_EVT_EDMAINT, 8);        // EDMA interrupt mapping auf externen interupt 8!!!
	IRQ_clear(IRQ_EVT_EDMAINT);
	IRQ_enable(IRQ_EVT_EDMAINT);
	IRQ_globalEnable();
}


void EDMA_interrupt_service(void)
{
	static int rcvPingDone=0;
	static int rcvPongDone=0;
	static int xmtPingDone=0;
	static int xmtPongDone=0;
	
	if(EDMA_intTest(tccRcvPing)) {
		EDMA_intClear(tccRcvPing); /* ohne clear geht es NICHT */
		rcvPingDone=1;
	}
	else if(EDMA_intTest(tccRcvPong)) {
		EDMA_intClear(tccRcvPong);
		rcvPongDone=1;
	}
	
	if(EDMA_intTest(tccXmtPing)) {
		EDMA_intClear(tccXmtPing);
		xmtPingDone=1;
	}
	else if(EDMA_intTest(tccXmtPong)) {
		EDMA_intClear(tccXmtPong);
		xmtPongDone=1;
	}	
	
	if(rcvPingDone && xmtPingDone) {
		rcvPingDone=0;
		xmtPingDone=0;
		// SW-Interrupt mit Verarbeitung
		SWI_post(&SWI_process_ping);
	}
	else if(rcvPongDone && xmtPongDone) {
		rcvPongDone=0;
		xmtPongDone=0;
		// SW-Interrupt mit Verarbeitung
		SWI_post(&SWI_process_pong);
	}
}

void process_ping_SWI(void)
{
	int i,j;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 1. Filterung    /////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////


	Anfangswert1[2] = Buffer_in_ping[0]; // Wert für PingPong_input[j=0];
	Anfangswert1[3] = Buffer_in_ping[1]; // Wert für PingPong_input[j=1];

	for(i=0; i<6; i++)
	    {
	        for(j=0; j<BUFFER_LEN; j++)
	        {
	            if(j < 3)
	            {
	                //                                                                        Anfangswert1[j] gleiche wie PingPong_input[j-2]
	                //                                                 Anfangswert1[j+1] gleiche wie PingPong_input[j-1]
	                Speicher[j] = b0[i]*Buffer_in_ping[j]+b1[i]*Anfangswert1[j+1]+b2[i]*Anfangswert1[j];
	                Speicher[j] = Speicher[j]-a1[i]*Speicher[j-1]-a2[i]*Speicher[j-2];
	            }
	            else
	            {
	                Speicher[j] = b0[i]*Buffer_in_ping[j]+b1[i]*Buffer_in_ping[j-1]+b2[i]*Buffer_in_ping[j-2];
	                Speicher[j] = Speicher[j]-a1[i]*Speicher[j-1]-a2[i]*Speicher[j-2];
	            }
	        }
	    }

	Anfangswert1[0] = Speicher[498];  //sind die Werte für die nächsten 500 Werte. Die Werte werden
	Anfangswert1[1] = Speicher[499];  //jetzt gespeichert und wenn die Funktion erneut aufgerufen
	                                            //wird sind diese Werte die Alten der vorherigen Funktion

	for(i=0; i<BUFFER_LEN; i++)
	    {
	        Speicher[i] = Speicher[i]*Verstaerkung;// Ausgangswerte mit Verstärkung Multiplizieren
	    }

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Hochmischen                                                                                    //
	Abtastzeit = 1/Abtastfrequenz;                                                                    //
	                                                                                                  //
	for(i=1; i<BUFFER_LEN; i++)                                                                       //
	{                                                                                                 //
	    Speicher2[i] = Speicher[i] * cos(2*PI*fg*i*Abtastzeit);                                        //
	}                                                                                                 //
	////////////////////////////////////////////////////////////////////////////////////////////////////

	 ////////////////////////////////////////////////////////////////////////////////////////////////////
	 // 2. Filterung    /////////////////////////////////////////////////////////////////////////////////
	 ////////////////////////////////////////////////////////////////////////////////////////////////////

	 //   Anfangswert2[2] = Speicher2[0]; // Wert für PingPong_output[j=0];
	 //   Anfangswert2[3] = Speicher2[1]; // Wert für PingPong_output[j=1];

	    for(i=0; i<6; i++)
	    {
	        for(j=0; j<BUFFER_LEN; j++)
	        {
	            if(j < 3)
	            {
	                //                    Anfangswert2[j] gleiche wie PingPong_output[j-2]
	                //                     Anfangswert2[j+1] gleiche wie PingPong_output[j-1]
	                Buffer_out_ping[j] = b0[i]*Speicher2[j]+b1[i]*Anfangswert2[j+1]+b2[i]*Anfangswert2[j];
	                Buffer_out_ping[j] = Buffer_out_ping[j]-a1[i]*Buffer_out_ping[j-1]-a2[i]*Buffer_out_ping[j-2];
	            }
	            else
	            {
	                Buffer_out_ping[j] = b0[i]*Speicher2[j]+b1[i]*Speicher2[j-1]+b2[i]*Speicher2[j-2];
	                Buffer_out_ping[j] = Buffer_out_ping[j]-a1[i]*Buffer_out_ping[j-1]-a2[i]*Buffer_out_ping[j-2];
	            }
	        }
	    }

	    Anfangswert2[0] = Speicher2[498];  //sind die Werte für die nächsten 500 Werte. Die Werte werden
	    Anfangswert2[1] = Speicher2[499];  //jetzt gespeichert und wenn die Funktion erneut aufgerufen
	                                             //wird sind diese Werte die Alten der vorherigen Funktion

	    for(i=0; i<BUFFER_LEN; i++)
	    {
	        Buffer_out_ping[i] = Buffer_out_ping[i]*Verstaerkung;// Ausgangswerte mit Verstärkung Multiplizieren
	    }



	//for(i=0; i<BUFFER_LEN; i++)
		//*(Buffer_out_ping+i) = *(Buffer_in_ping+i);
}

void process_pong_SWI(void)
{
	int i;
	for(i=0; i<BUFFER_LEN; i++)
		*(Buffer_out_pong+i) = *(Buffer_in_pong+i); 
}

//void SWI_LEDToggle(void)
//{
//	SEM_postBinary(&SEM_LEDToggle);
//}

//void tsk_led_toggle(void)
//{
	/* Initialisierung der Task */
	/* nix */
	
	/* Verarbeitung */
//	while(1) {
	//	SEM_pendBinary(&SEM_LEDToggle, SYS_FOREVER);
		
	//	DSK6713_LED_toggle(1);
	//}
	
	/* Aufräumen */
	/* nix */
//}
