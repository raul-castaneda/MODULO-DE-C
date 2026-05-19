/*
 * PRUEBA_TIMER1_SERVO_COMPARTIDO.c
 *
 * Created: 28/04/2026 15:20:58 
 * Author:  raulc
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)]
#define F_CPU 16000000UL				// Definimos la frecuencia del clock principal del micro para usar la función _delay_ms //
#include <avr/io.h>		
#include <avr/interrupt.h>				
#include <stdlib.h>						// Importamos esta libreria para poder utilizar el comando atoi para convertir un string a un numero //
#include <avr/eeprom.h>					// Importamos esta libreria para hacer posible guardar posiciones en la eprom //
#include <util/delay.h>					// Importamos esta libreria para nuevamente usar delays // 
#include <string.h>						// Importamos esta libreria para poder realizar operaciones con cadenas de caracteres recibidas de adafruit // 
/****************************************/
// Incluimos todas las librerias a utilizar en el "MAIN"//
#include "PWM1/PWM1.h"					// Librerias PWM para la ejecución de movimiento de los servos // 
#include "PWM_2/PWM_2.h"				// Librerias PWM para la ejecución de movimiento de los servos // 
#include "ADC/ADC.h"					// Libreria ADC para el control de servos por medio de potenciómetros // 
#include "PIN_CHANGE/PIN_CHANGE.h"		// Libreria PINCHANGE para uso de botones de cambio de modo, grabar posiciones y reproducción de las mismas // 
#include "UART/UART.h"					// Configuración UART para recibir datos desde la computadora y poder controlar los servos por medio de la computadora // 
/****************************************/
// Definición de macros // 
#define led_estado_0 (1<<PB0)			// Macros para leds que indican estado 0 "modo manual" (PB0) //
#define led_estado_1 (1<<PB4)			// Macros para leds que indican estado 1 "reproducción de movimientos guardados en eprom" (PB4) //
#define led_guardado (1<<PD7)			// Macros para led que indica que se guardo los movimientos en la eprom (PD7) //
/****************************************/
// Definiciones de variables globales // 
volatile uint8_t modo = 0;				// 0 = potenciómetros, 1 = Adafruit IO //
volatile uint8_t ADC_valor = 0; 		// ADC_valor, guarda el valor del ADCH (0->255) //
volatile uint8_t reproduciendo = 0; 	// Bandera que indica que la isr de timer0 esta reproduciendo las posiciones desactivando los potenciómetros // 
/****************************************/
// Tanto bandera_escribir, idx_escribir y modo_escritura, trabajan juntas para la escritura de la EPROM // 

volatile uint8_t bandera_escribir = 0;	// If bandera_escribir = 1, activa la escritura en la eprom // 
volatile uint8_t idx_escribir = 0;		// Indica que posición de la arreglo se debe escribir en la eprom // 
volatile uint8_t modo_escritura = 0; 	// Bandera que indica en que posiciones escribir si en manuales o en adafruit // 

/****************************************/
volatile uint8_t total_manual = 0; 		// Límite de posiciones para cada modo // 
volatile uint8_t total_adafruit = 0; 	// Límite de posiciones para cada modo // 
/****************************************/
// Estructura y posición de arreglos // 
// Agrupamos los 4 valores de los servos en una sola unidad, para los OCR1X del timer1 valores de 16 bits y para OCR2X del timer 2 valores de 8 bits //
typedef struct{
	uint16_t ocr1a;						// Valor del OCR1A en microsegundos //
	uint16_t ocr1b; 					// Valor del OCR1B en microsegundos // 
	uint8_t ocr2a; 						// Valor del OCR2A en microsegundos // 
	uint8_t ocr2b; 						// Valor del OCR2B en microsegundos // 
} Posicion; 
Posicion posiciones_manual[4];			// Arreglo de la ram para guardar posiciones realizadas manualmente //
Posicion posiciones_adafruit[4];		// Arreglo de la ram para guardar posiciones realizdas por medio de adafruit // 
/****************************************/
volatile uint8_t	pos_idx_manual		= 0; 	// Indice de escritura en modo manual // 
volatile uint8_t	pos_idx_adafruit    = 0; 	// Indice de escritura en modo adafruit // 
volatile uint8_t	rep_idx				= 0; 	// Indice de reproducción // 
volatile uint8_t	flag_guardado		= 0; 	// Bandera que indica si se guardaron las posiciones haciendo que se active el toggle de la led_guardado //
volatile uint16_t	conteo_overflow		= 0;	// Contador de ovf a utilizar en ISR de timer0 // 
/****************************************/
// Function prototypes

void setup_main();								// Configuración de salidas para las leds // 
/****************************************/
// Main Function
int main(void)
{
	cli();										// Desactivamos las ISRS // 
	setup();									// Setup para libreria de PWM usando timer 1 (PWM1) // 
	setup_2();									// Setup para libreria de PWM usando timer 2 (PWM2) // 
	setup_main();								// Setup del main donde se configuran como salidas los pines donde van las leds de indicación // 
	setup_PINCHANGE();							// Setup de libreria PINCHANGE configurando como entradas los botones a utilizar // 
	init_timer1();								// Configuración del timer1 para PWM // 
	init_TIMER2();								// Configuración del timer2 para pwm // 
	init_ADC();									// Configuración de ADC para uso de la misma para ISR // 
	initUART();									// Configruación de UART para comunicación computadora->arduino nano //
	initPINCHANGE();							// Configuración de ISR pinchange //
	
	/****************************************/ 
	// Cargar posiciones guardadas desde eeprom al inciar el programa // 
	// Leemos los 24 bytes de la dirección 0 y copiamos al arreglo posciones_manual en RAM asi sí se apaga el micro se recuperan las posciones automaticamente // 
	eeprom_read_block(posiciones_manual,
					  (void*)0,
					   4*sizeof(Posicion));
	// Misma lógica para las poiciones guardadas cuando se uso dashboard de ADAFRUIT // 
	eeprom_read_block(posiciones_adafruit,
					  (void*)(4*sizeof(Posicion)),
					   4*sizeof(Posicion));
	/****************************************/			   
	// Leemos cuantas posiciones había guardadas antes de que se apague el micro // 
	total_manual  = eeprom_read_byte((uint8_t*)40);	
	total_adafruit = eeprom_read_byte((uint8_t*)41);
	/****************************************/
	// Arrancamos la secuencia de canales con el ADC6 para evitar errores //
	ADMUX = (ADMUX & 0xF0) | 0x06;
	ADCSRA |= (1<<ADSC);
	/****************************************/
	sei();										 // Activamos ISRS globales //
	/****************************************/
	// Arrancamos desde el modo 0 (modo manual) //
	modo = 0; 
	PORTB |= (led_estado_0);					// Encendemos led que indica en que modo estamos // 
	/****************************************/
	while (1)
	{
		// Cuando la ISR pinchange activa la bandera de guardado hace un pequeño toggle la led indicando que se guardo correctamente las posiciones // 
		if(flag_guardado)
		{
			// Toggle de led usando _delay_ms // 
			PORTD |= (led_guardado);
			_delay_ms(200);
			PORTD &= ~(led_guardado);
			_delay_ms(200);
			PORTD |= (led_guardado);
			_delay_ms(200);
			PORTD &= ~(led_guardado);
			flag_guardado = 0; 
		}
		if(bandera_escribir)
		{
			// Cuando se apacha boton guardar temporalmente se guardan las posiciones en RAM y luego activa bandera_ escribir para ya escribir las posiciones en eprom // 
			// Se hizo en el while para no hacer más larga la ISR y evitar que se tarde la ISR //
			// Para una escritura correcta se usa idx_escribir*sizeof(Posicion) para calcular la dirección correcta de eprom  tanto para posiciones manuales y adafruit // 
			if (modo_escritura == 0)
			{
				eeprom_write_block(&posiciones_manual[idx_escribir],
				(void*)(idx_escribir * sizeof(Posicion)),
				sizeof(Posicion));
				eeprom_write_byte((uint8_t*)40, total_manual);
			}
			else if (modo_escritura == 1)
			{
				eeprom_write_block(&posiciones_adafruit[idx_escribir],
				(void*)((4 * sizeof(Posicion)) + (idx_escribir * sizeof(Posicion))),
				sizeof(Posicion));
				eeprom_write_byte((uint8_t*)41, total_adafruit);
			}
			bandera_escribir = 0; // Una vez finalizada la escritura, se desactiva la bandera //
		}
	
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup_main()
{
	// Configuración de leds para estados 
	DDRB |= (led_estado_0); // Configuración como salida PB0 (modo manual)
	DDRB |= (led_estado_1); // Configuración como salida PB4 (modo adafruit) 
	DDRD |= (led_guardado); // Configuración como salida PD7 (señal de que ya se guardo posición en eprom) 
}
/****************************************/
// Interrupt routines
ISR (PCINT1_vect)
{
	// Accionamiento de boton de modo // 
	if (!(PINC & boton_modo))			// if boton_modo = 0, se presiono cambia de modo // 
	{
		modo++;
		if(modo > 1)
		{ 
			modo = 0;					// Si se presiona el boton_modo en modo 1, hace overflow volviendo a modo manual //
		}
		if (modo == 0) 
		{	
			PORTB |= led_estado_0;     // Si estamos en modo manual, se enciende led_estado_0 //
			PORTB &= ~led_estado_1;    // Mantenemos led_estado_1 apagada // 
		}
		if (modo == 1)				   // Si estamos en modo = 0 y se preciona boton_modo, se cambia a modo 1 (adafruit) // 
		{  
			PORTB |= led_estado_1;	   // Se activa led_estrado_1, para indicar que se esta en modo adafruit // 
			PORTB &= ~led_estado_0;	   // Mantenemos led_estado_0 apagada // 
		}
	}
	// Accionamiento de boton de guardar // 
	if (!(PINC & boton_guardar))	   // Si se presiona el boton de guardar se hacen dos verificaciones, si estamos en modo manual o adafruit // 
	{
		/****************************************/
		// Si estamos en modo manual // 
		if (modo == 0)
		{
			// Si modo+escritura = 0, guardamos valores de CORNX modificados por los potenciómetros // 
			// Guardamos los valores actuales de los OCRNX en microsegundos para solo usarlos en las funciones PWM_set de los timers1/2 // 
			modo_escritura = 0; 
			posiciones_manual[pos_idx_manual].ocr1a = OCR1A/2;
			posiciones_manual[pos_idx_manual].ocr1b = OCR1B/2;
			posiciones_manual[pos_idx_manual].ocr2a = OCR2A;
			posiciones_manual[pos_idx_manual].ocr2b = OCR2B;
			idx_escribir = pos_idx_manual; 
			pos_idx_manual++;
			if(pos_idx_manual > 4) 
			{	
				// Si el indice de posiciones manuales es mayor a 4 se reinicia el indice de posicines manuales // 
				pos_idx_manual = 0; 
			}
		
			if(total_manual < 4)
			{
				// Si no sigue incrementando hasta llegar a 4 // 
				total_manual ++; 
			}
		}
			// Si modo_escritura = 1, guardamos valroes de OCRNX modificados por los potenciometros // 
		else if (modo == 1)
		{
			// Mismo procedimiento para guardar valores de los OCRNX pero en este caso son valores modificados por ADAFRUIT // 
			modo_escritura = 1; 
			posiciones_adafruit[pos_idx_adafruit].ocr1a = OCR1A/2;
			posiciones_adafruit[pos_idx_adafruit].ocr1b = OCR1B/2;
			posiciones_adafruit[pos_idx_adafruit].ocr2a = OCR2A;
			posiciones_adafruit[pos_idx_adafruit].ocr2b = OCR2B;
			idx_escribir = pos_idx_adafruit;
			pos_idx_adafruit++;
			if(pos_idx_adafruit > 4) 
			{
				// Misma verificación de posiciones pero esta vez son posiciones realizadas por ada fruit // 
				pos_idx_adafruit = 0;
			}
		
			if(total_adafruit < 4)
			{
				total_adafruit ++; 
			}
		
		}
		flag_guardado   = 1; // Activamos toggle de led indicando el guardado de las posiciones // 
		bandera_escribir = 1; // Activamos escritura en eprom // 
	}
	// Accionamiento de boton de reproducir // 
	if (!(PINC & boton_reproducir))
	{
		// Se realiza esta configuración antes de iniciar timer0 para evitar reproducir si no hay posiciones guardadas // 
		if ((modo == 0 && total_manual > 0) || (modo == 1 && total_adafruit	 > 0))
		{
			// Si sí hay posiciones guardadas // 
			reproduciendo = 1;			// Se activa la bandera de reproduciendo así mismo bloqueando los potenciómetros // 
			rep_idx = 0;				// Hacemos que el indice inicie desde la posición 0 // 
			conteo_overflow = 0;		// Iniciamos el conteo de overflow en 0 // 
			/****************************************/
			// Configuración de timer0 para tener más o menos 5 segundos entre posiciones // 
			TCNT0  = 0;					// Inicializamos el TCNT0 en 0 // 
			TCCR0A = 0x00;				// Configuración en modo normal en timer0 // 
			TCCR0B &= ~((1<<CS02) | (1<<CS01) | (1<<CS00));	
			TCCR0B |= (1<<CS02) | (1<<CS00);	// Configuramos un prescaler de 1024
			TIMSK0 |= (1<<TOIE0);		// Activamos ISRS de timer0 por overflow // 
			/****************************************/
		}
	}
}
ISR (ADC_vect)
{
	// Asiganmos ADCH a ADC_valor // 
	ADC_valor = ADCH;
	uint8_t canal  = ADMUX & 0X0F; // Aplicamos una mascara para solo tener los primeros 8 bits del ADMUX // 

	if(canal == 7)
	{
		// Verificamos si estamos en modo 0 para poder controlar la garra manualmente por medio de los potenciómetros y si no se esta reproduciendo posiciones (esto se hace para cada potenciómetro) // 
		// Notese que no se volvera a explicar esta lógica ya que es la misma para mapeo de timer1B	//
		if (modo == 0 && reproduciendo == 0)
		{
			// Mapeo de valor OCR1A para PWM_set y que servo se mueva de 0 a 180 //
			uint16_t tiempo_us;
			tiempo_us = 500 + ((uint32_t)ADC_valor*2000)/255;
			PWM_set(tiempo_us);
		}
			ADMUX = (ADMUX & 0xF0) | 0x06;
	}

	else if(canal == 6)
	{
		if (modo == 0 && reproduciendo == 0)
		{
			// Mapeo de valor OCR1B para PWM_set_B y que servo se mueva de 0 a 180 // 
			uint16_t tiempo_us_B;
			tiempo_us_B = 500 + ((uint32_t)ADC_valor*2000)/255;
			PWM_set_B(tiempo_us_B);
		}
		ADMUX = (ADMUX & 0xF0) | 0x04;
	}

	else if(canal ==  5)
	{
		if (modo == 0 && reproduciendo == 0)
		{
			// Mapeo de valor COR2A para PWM_set2 y que servo se mueva de 0 a 180 // 
			uint8_t OCR2A_valor = OCR2A_valormin + ((uint16_t) ADC_valor * 32) / 255 ;  
			PWM_set2(OCR2A_valor);
		}
	
		ADMUX = (ADMUX & 0xF0) | 0x07;
	}

	else if (canal == 4)
	{
		if (modo == 0 && reproduciendo == 0)
		{
			// Mapeo del valor OCR2B para PWM_set2_B y que servo se mueva aproximadamente unicamente 10 grados ya que este OCRNx controla la pinza de la garra //
			uint8_t OCR2B_valor = OCR2B_valormin + ((uint16_t) ADC_valor *(OCR2B_valormax - OCR2B_valormin))/255; 
			PWM_set2_B(OCR2B_valor);
		
		}
		ADMUX = (ADMUX & 0xF0) | 0x05;
	}

	ADCSRA |= (1<<ADSC); // Realizamos una nuvea conversión al final de todo el multiplexeo de canales ADC //
}

ISR (USART_RX_vect)
{
	// Declaramos como staticos el buffer y el indice para que su valor sea el mismo y no se pierda dentro de a ISR //
	static char buffer[16];		// Acumulamos los bytes recibidos por UART // 
	static uint8_t idx = 0;		// Indice del buffer // 
	// Leemos el byte de información recibido //
	char C = UDR0;	

	// Verificamos si ya se recibio el caracter nulo del byte de información 
	if (C != '\n')
	{	
		// Realizamos esta comparación como programación defensiva y evitamos que el buffer desborde si llega un byte muy largo //
		if (idx < 15)
		{
			buffer[idx++] = C;		// Guardamos la información recibida e increenta el indice // 
		}
		else 
		{
			idx = 0;				// Si hay desbordamiento reseteamos el indice // 
		}
	}
	else
	{
		buffer[idx] = '\0';			// Verficiamos si ya se resivio el caracter nulo //
		idx = 0;					// Reiniciamos el indice // 
	
		if (buffer[0] == 'S' && buffer[2] == ':')	// Verificamos si el mensaje enviado por adafruit empieza con S y si : estaa en la posición correcta // 
		{
			uint8_t servo = buffer[1] - '0';		// Convertimos el caracter a número por medio de ASCII // 
			uint16_t valor = atoi(&buffer[3]);		// Ahora solo tomamos el valor numerico es decir, solo agarramos "90\0" de S1:90\0 //
			
			// Realizamos una condición defensiva para evitar valores erroneos // 
			if (valor > 180) 
			{
				valor = 180;		// Si los valores enviados por la computadora son mayores a 180 solo se recibe el valor 180 //
			}
			
			if(valor < 0) 
			{
				valor = 0;			// Si los valores enviados son menores a 0 por la computadora solo se recibe el valor mínimo que es 0 nada más // 
			}
			
			// Mapeamos 0-180, es decir mapeamos valores recibidos a us y usamos parametro t/t para utilizarlos en los PWM_set/set_B del timer 1 unicamente // 
			uint16_t t = 500 + ((uint32_t)valor*2000)/180;
			// Para que se desarrolle el correcto funcionamiento del control de los servos por medio de adafruit se verifica si estamos en modo = 1 y el numero del servo // 
			if(servo == 1 && modo == 1)
			{
			
				PORTB |= (led_estado_1); // Sí estamos en modo 1, encendemos led_estado_1 // 
				PORTB &= ~(led_estado_0); // Mantenemos apagado el led_estado_0 // 
				PWM_set(t); // Iniciamos la función PWM_set con su respectivo parámetro "t" usando el valor de OCR1A // 
			}
			else if (servo == 2 && modo == 1)
			{
				// Sí estamos en modo 1, y recibimos valores del servo No. 2 // 
				PWM_set_B(t); // Se inicializa el segundo PWM_set_B del timer 1 usando OCR1B // 
			}
			else if (servo == 3 && modo == 1)
			{
				// Sí estamos en modo 1, y recibimos valores del servo No. 3 // 
				uint8_t t_2A = OCR2A_valormin + ((uint16_t)valor * 32)/180; // Realizamos un nuevo mapeo para los parámetros del timer 2 "t_2A/_2B"// 
				PWM_set2(t_2A); // Inciamos la función PWM_set2 con su respectivo parámetro "t_2A" usando el valor OCR2A // 
			}
			else if (servo == 4 && modo == 1)
			{
				// Si estamos en modo 1, y recibimos valores del servo No. 4 // 
				uint8_t t_2B =	OCR2B_valormin + ((uint16_t)valor * (OCR2B_valormax - OCR2B_valormin))/180; // Realizamos un distinto mapeo al anterior debido a que este pertenece a la pinza y solo necesitamos que se mueva aproximadamente 10 grados // 
				PWM_set2_B(t_2B); // Iniciamos la función PWM_set2_B con su respectivo parámetro "t_2B" usando el valor de OCR2B // 
			}
		}
	}
}

ISR (TIMER0_OVF_vect)
	{
		conteo_overflow++; // Inicializamos el conteo de overflow // 
		if(conteo_overflow >= 350) // Verificamos si ya se cumplió los 5 segundos para reproducir las siguientes posiciones // 
		{
			conteo_overflow = 0; // Sí ya pasaron 5 segundos, se renicia el contador de ovf // 
			
			// Verificamos si ya se terminaron de reproducir las posiciones guardadas // 
			if (modo == 0 && rep_idx >= total_manual) 
			{
				// Sí ya se terminaron de reproducir las posiciones guardadas // 
				rep_idx = 0;			// Se reinicia el indice //
				TCCR0B  = 0x00;			// Deshabilitamos timer0 //
				TIMSK0 &= ~(1<<TOIE0);	// Deshabilitamos isr de timer0 por overflow //
				reproduciendo = 0;		// Desbloqueamos potenciómetros apagando la bandera de reproduciendo //
				return; 				// Salimos de la condición //
			}
			// De igual verificamos si ya terminaron de reproducir las posiciones guardadas en modo ADAFRUIT //
			else if (modo == 1 && rep_idx >= total_adafruit)
			{
				// Sí ya se terminar de reproducir las posiciones guardadas, misma lógica que la anterior//
				rep_idx = 0;
				TCCR0B  = 0x00;
				TIMSK0 &= ~(1<<TOIE0);
				reproduciendo = 0;
				return; 
			}

			// Leer del arreglo RAM directamente — sin EEPROM
			Posicion p;
			if (modo == 0)     
			{
				p = posiciones_manual[rep_idx];		// Volvemos a la posición en la que estabamos antes de guardar las posiciones // 
			}
			 
			else if (modo == 1) 
			{
				p = posiciones_adafruit[rep_idx];	// Volvemos a la posición en la que estabamos antes de guardar las posiciones // 
			}

			// Asignamos valores a los OCRNXs en ticks ya que las funciones PWM_set se encargan de pasarlo a micro segundos  //
			OCR1A = p.ocr1a * 2;	//  valor de OCR1A en ticks //
			OCR1B = p.ocr1b * 2;	//	Valor de OCR1B en ticks //
			OCR2A = p.ocr2a;		//	OCR2A sin conversión //
			OCR2B = p.ocr2b;		//	OCR2B sin conversión // 

			rep_idx++;				// Incrementamos el indice a la siguiente posición // 
		}
	}