#ifndef UDP_H
#define UDP_H

#include "ipv4.h"

#include <timerms.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <netinet/in.h>

#define UDP_PROTOCOL 17
#define UDP_MAX_PAYLOAD (IPv4_MAX_PAYLOAD - UDP_HEAD_SIZE)
#define UDP_PORT_SIZE 2
#define UDP_HEAD_SIZE 8

typedef unsigned char udp_port_t [UDP_PORT_SIZE];

int udp_port_bin(udp_port_t port);

void udp_bin_port(udp_port_t port, int bin_port);

/*
 * int udp_open ()
 *
 * DESCRIPCIÓN:
 *   Esta función inicializa el protocolo udp para poder enviar datos.
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
int udp_open();

/*
 * int udp_close ()
 *
 * DESCRIPCIÓN:
 *   Esta función finaliza el protocolo udp.
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
int udp_close();

/*
 * int udp_send(udp_port_t port_src, udp_port_t port_dst, ipv4_addr_t ipv4_src,
 *				unsigned char *payload, int payload_len);
 *
 * DESCRIPCIÓN:
 *   Esta función permite enviar un datagrama udp especificando el protocolo
 *   de la capa superior.
 *
 * PARÁMETROS:
 *	 'port_src': Puerto origen.
 *   'port_dst': Puerto destino.
 *   'ipv4_dst': Dirección IP destino.
 *	   'payload': Payload a enviar.
 * 'payload_len': Longitud en bytes del payload.
 *
 * VALOR DEVUELTO:
 *   Devuelve el número de bytes enviados.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error.
 */
 int udp_send(udp_port_t port_src, udp_port_t port_dst, ipv4_addr_t ipv4_dst, unsigned char *payload, int payload_len);

/*
 * int udp_recv(udp_port_t *port_src, ipv4_addr_t ipv4_src, udp_port_t port_dst,
 *			 	unsigned char *buffer, long int timeout );
 *
 * DESCRIPCIÓN:
 *   Esta función permite recibir un datagrama udp desde el nivel de enlace
 *	 especificando el protocolo de la capa superior.
 *
 * PARÁMETROS:
 * 'port_src': Puntero donde se almacenara el puerto origen.
 * 'ipv4_src': Variable donde se almacenará la IP origen.
 * 'port_dst': Puerto destino.
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
int udp_recv(udp_port_t port_src, ipv4_addr_t ipv4_src, udp_port_t port_dst, unsigned char *buffer, long int timeout );
#endif
