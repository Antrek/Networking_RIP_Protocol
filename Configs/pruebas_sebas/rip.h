#ifndef _RIP_H
#define _RIP_H

#include "ipv4.h"
#include "udp.h"
#include "ipv4_config.h"
#include "ipv4_route_table.h"
#include "rip_route_table.h"

#define ADMIN_DISTANCE 120
#define HOP_METRIC 1
#define INF_TIME 180
#define DEL_TIME 120
#define BASE_UPDATE_TIME 30
#define RIP_PORT 520
#define MAX_ENTRIES 25
#define RIP_REQUEST 1
#define RIP_RESPONSE 2
#define RIP_MAX_SIZE 1472
#define RIP_HEADER_LEN 4
#define VERSION 2
#define FAM_ID_IP 2
#define RIP_ROUTE_LEN 20
#define ROUTE_TAG 0

/*************** Estructuras ****************/


/*
*int rip_recv(udp_port_t src, udp_port_t dst, ipv4_addr_t from, 
			 unsigned char *buffer, unsigned char *opcode, long int timeout)
*
* DESCRIPCION:
* Esta función se encarga de recibir los mensajes rip de la red, analizarlos y
* validarlos. Si el mensaje es válido guarda en el buffer las rutas recibidas,
* si no, descarta el paquete.
* 
* PARAMETROS:
* "src":	Puntero donde se guarda el puerto desde el que se envía el datagrama.
* "dst":	Puerto al que se envió el datagrama.
* "from":	Puntero donde se guarda la IP origen del datagrama.
* "buffer":	Puntero donde se guardan las rutas del datagrama RIPv2 recibido.
* "opcode":	Puntero donde se guarda el tipo de mensaje recibido (RESPONSE o REQUEST)
* "timeout":Tiempo que estará esperando hasta recibir algo.
* 
* VALOR DEVUELTO:
* Devuelve el número de bytes recibidos.
*
* ERRORES:
* Si ha habido errores al recibir devuelve -1
*
*/
int rip_recv(udp_port_t src, udp_port_t dst, ipv4_addr_t from, 
			 unsigned char *buffer, unsigned char *opcode, long int timeout);

/*
* int rip_send(ipv4_addr_t dst,udp_port_t port_dst, unsigned char* table, 
*			 int len, int op_code) 
* 
* DESCRIPCION:
* Esta función se encarga de enviar la tabla de rutas pasada por parametros.
* 
* PARAMETROS:
* 
* 'dst': 	  IP a la que le va dirigido el paquete.
* 'port_dst': Puerto al que va dirigido el paquete a la maquina destino.
* 'table':	  Tabla en la cual están almacenadas las rutas a enviar.
* 'len':	  Longitud de la tabla de rutas.
* 'op_code':  Indica si el paquete es un response o un request
* 
* VALOR DEVUELTO:
* Devuelve el número de bytes enviados si no se ha producido error
* 
* ERROR
* Devuelve '-1' si no ha podido enviarse el datagrama.
*
*/
int rip_send(ipv4_addr_t dst,udp_port_t port_dst, unsigned char* table, 
			 int len, int op_code);

/*
* int rip_open ()
* DESCRIPCION
* Esta funcion se encarga de inicializar el protocolo rip para poder enviar datos
*
* PAR�METROS:
*   Ninguno.
*
* VALOR DEVUELTO:
*   Devuelve 0 si se ha podido inicializar el protocolo.
*
* ERRORES:
*   La funci�n devuelve '-1' si se ha producido alg�n error.
*/
int rip_open();

/*
 * int rip_close ()
 *
 * DESCRIPCI�N:
 *   Esta funci�n finaliza el protocolo rip.
 *
 * PAR�METROS:
 *   Ninguno.
 *
 * VALOR DEVUELTO:
 *   Devuelve 0 si se ha podido finalizar la instancia del protocolo.
 *
 * ERRORES:
 *   La funcion devuelve '-1' si se ha producido algun error.
 */
int rip_close();



#endif /* _RIP_H */
