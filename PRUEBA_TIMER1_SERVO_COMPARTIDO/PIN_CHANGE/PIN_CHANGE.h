/*
 * PIN_CHANGE.h
 *
 * Created: 16/05/2026 15:01:14
 *  Author: raulc
 */ 


#ifndef PIN_CHANGE_H_
#define PIN_CHANGE_H_

// Encabezado de librerias // 
#include <avr/io.h>
// Definiciones de macros// 
#define boton_modo        (1<<PC0)	// Macro para boton de cambio de modo entre 0 y 1 
#define boton_guardar     (1<<PC1)	// Macro para boton de cambio de modo entre 0 y 1 
#define boton_reproducir  (1<<PC2)	// Macro para reproducir las posiciones guardadas en eprom
//#define boton_clear_eprom (1<<PC3)	// Macro para eliminar todo lo que esta en la eprom para evitar posiciones "basura"
/****************************************/
// Function prototypes
void setup_PINCHANGE();	//Declaramos los prototipos de funciones a usar para esta libreria; definición de entradas y salidas 
void initPINCHANGE();	//Declaramos los prototipos de funciones a usar para esta libreria; configuración de ISR

#endif /* PIN_CHANGE_H_ */