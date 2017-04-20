#ifndef _IPv4_H
#define _IPv4_H

#include "eth.h"

#include <stdint.h>

/* Tamaño en bytes de una dirección IPv4 */
#define IPv4_ADDR_SIZE 4

/* Tamaño máximo de payload de un datagrama IPv4 sin opciones.
 * Usada por ip_client/server
 */
#define IPv4_MAX_PAYLOAD (ETH_MTU - IPv4_HEAD_SIZE)

/* Tamaño de la cabecera de un datagrama IPv4 sin opciones.
 * Usada por ip_client/server
 */
#define IPv4_HEAD_SIZE 20

/* Tamaño máximo de una dirección IPv4 como cadena de caracteres 
 * Usada por ip_client/server
 */
#define IPv4_STR_MAX_LENGTH 16

/* Definición del tipo de la estructura que almacena las direcciones IPv4 */
typedef unsigned char ipv4_addr_t [IPv4_ADDR_SIZE];

/* 
 * Definición del tipo de la estructura que permite manejar parametros de 
 * configuración local.
 */
typedef struct ipv4_handler ipv4_handler_t;

/*
 * uint32_t ipv4_addr_bin(ipv4_addr_t ipv4_addr)
 *
 * DESCRIPCIÓN:
 *   Esta función local realiza la conversión de una dirección IPv4 ipv4_addr_t
 *   a binario.
 *
 * PARÁMETROS:
 *   'ipv4_addr': Dirección IPv4 a convertir.
 *
 * VALOR DEVUELTO:
 *   Devuelve la representación binaria de la dirección pasada por parámetros.
 */
uint32_t ipv4_addr_bin(ipv4_addr_t ipv4_addr);

void ipv4_bin_addr(unsigned char *ipv4_addr, uint32_t ipv4_bin_addr);
/*
 * int ipv4_open ()
 *
 * DESCRIPCIÓN:
 *   Esta función inicializa el protocolo IPv4 para poder enviar datos.
 *
 * PARÁMETROS:
 *   Ninguno.
 *
 * VALOR DEVUELTO:
 *   Devuelve 0 si se ha podido inicializar el protocolo.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error.
 */
int ipv4_open();

/*
 * int ipv4_close ()
 *
 * DESCRIPCIÓN:
 *   Esta función finaliza el protocolo IPv4.
 *
 * PARÁMETROS:
 *   Ninguno.
 *
 * VALOR DEVUELTO:
 *   Devuelve 0 si se ha podido finalizar la instancia del protocolo.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error.
 */
int ipv4_close();

/*
 * int ipv4_send(ipv4_addr_t ipv4_dest, uint16_t protocol, 
 * 				unsigned char *payload, int payload_len );
 *
 * DESCRIPCIÓN:
 *   Esta función permite enviar un datagrama IPv4 especificando el protocolo
 *   de la capa superior.
 *
 * PARÁMETROS:
 *   'ipv4_dest': Dirección IP destino.
 *    'protocol': Protocolo de la capa superior.
 *	   'payload': Payload a enviar.
 * 'payload_len': Longitud en bytes del payload.
 *
 * VALOR DEVUELTO:
 *   Devuelve el número de bytes enviados.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error.
 */
int ipv4_send(ipv4_addr_t ipv4_dest, uint16_t protocol, unsigned char * payload, int payload_len );

/*
 * int ipv4_recv(ipv4_addr_t ipv4_src, uint16_t protocol, 
 * 				unsigned char *buffer, long int timeout );
 *
 * DESCRIPCIÓN:
 *   Esta función permite recibir un datagrama IPv4 desde el nivel de enlace
 *	 especificando el protocolo de la capa superior.
 *
 * PARÁMETROS:
 * 'ipv4_src': Dirección IP origen.
 * 'protocol': Protocolo de la capa superior.
 *	 'buffer': Buffer para almacenar los datos recibidos.
 *  'timeout': Tiempo en milisegundos que debe esperarse a recibir un datagrama
 *             antes de retornar. Un número negativo indicará que debe
 *             esperarse indefinidamente, mientras que con un '0' la función
 *             retornará inmediatamente, se haya recibido o no un datagrama.
 *
 * VALOR DEVUELTO:
 *   Devuelve el número de bytes recibidos.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error.
 */
int ipv4_recv(ipv4_addr_t ipv4_src, uint16_t protocol, unsigned char *buffer, long int timeout );

/*
 * uint16_t ipv4_checksum ( unsigned char * data, int len )
 *
 * DESCRIPCIÓN:
 *   Esta función calcula el checksum IP de los datos especificados.
 *
 * PARÁMETROS:
 *   'data': Puntero a los datos sobre los que se calcula el checksum.
 *    'len': Longitud en bytes de los datos.
 *
 * VALOR DEVUELTO:
 *   El valor del checksum calculado.
 */
uint16_t ipv4_checksum ( unsigned char * data, int len );


/* void ipv4_addr_str ( ipv4_addr_t addr, char* str );
 *
 * DESCRIPCIÓN:
 *   Esta función genera una cadena de texto que representa la dirección IPv4
 *   indicada.
 *
 * PARÁMETROS:
 *   'addr': La dirección IP que se quiere representar textualente.
 *    'str': Memoria donde se desea almacenar la cadena de texto generada.
 *           Deben reservarse al menos 'IPv4_STR_MAX_LENGTH' bytes.
 */
void ipv4_addr_str ( ipv4_addr_t addr, char* str );


/* int ipv4_str_addr ( char* str, ipv4_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función analiza una cadena de texto en busca de una dirección IPv4.
 *
 * PARÁMETROS:
 *    'str': La cadena de texto que se desea procesar.
 *   'addr': Memoria donde se almacena la dirección IPv4 encontrada.
 *
 * VALOR DEVUELTO:
 *   Se devuelve 0 si la cadena de texto representaba una dirección IPv4.
 *
 * ERRORES:
 *   La función devuelve -1 si la cadena de texto no representaba una
 *   dirección IPv4.
 */
int ipv4_str_addr ( char* str, ipv4_addr_t addr );

/*mac_addr_t group_mapping_ip_mac
 *			(ipv4_addr_t ipv4)
 *
 * DESCRIPCION
 *	La funcion se encarga de mapear una direccion
 *	ip multicas, a una direcci�n mac multicast.
 *	
 * PARAMETROS
 *	ipv4 -> Direcci�n ip a transformar.
 *
 *VALOR DE SALIDA:
 *	Devuelve una direcci�n mac multicast.
 *
 *(Testado y funcionando.)
 *
 *ERRORES
 *	Si la direcci�n ip no es una direcci�n multicast devuelve -1.
 */
void group_mapping_ip_mac(ipv4_addr_t ipv4 , mac_addr_t mac_group);

#endif /* _IPv4_H */
