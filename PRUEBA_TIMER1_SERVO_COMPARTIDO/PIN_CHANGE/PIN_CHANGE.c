/*
 * PIN_CHANGE.c
 *
 * Created: 16/05/2026 15:01:03
 *  Author: raulc
 */ 

#include "PIN_CHANGE.h"
void setup_PINCHANGE()
{
	// Configuración de entradas de botones// 
	DDRC &=		~(boton_modo);			// Configuramos como entrada PC0 donde estará el boton de cambio de modo
	DDRC &=		~(boton_guardar);		// Configuramos como entrada PC1 donde estará el boton de guardar posición en la memoria
	DDRC &=		~(boton_reproducir);	// Configuramos como entrada PC2 donde estará el boton de reproducir posiciones guardadas en memoria 
	// Activación de pull-ups //
	PORTC |=	(boton_modo); 			// Activamos pull-ups
	PORTC |=	(boton_guardar);		// Activamos pull-ups
	PORTC |=	(boton_reproducir);		// Activamos pull-ups
}

void initPINCHANGE()
{
	// Apagamos todos los registros de control y activamos el registro de control para PORTC (PCIE1)
	PCICR &= ~((1<<PCIE2) | (1<<PCIE1) | (1<<PCIE0));
	PCICR |= (1<<PCIE1);
	// Activamos registros de que pines del PORTC generaran la isr, en este caso solo PC0,PC1, PC2 y PC3
	//PCMSK1 |= (1<<PCINT8) | (1<<PCINT9) | (1<<PCINT10) | (1<<PCINT13);
	PCMSK1 |= (1<<PCINT8) | (1<<PCINT9) | (1<<PCINT10);
}