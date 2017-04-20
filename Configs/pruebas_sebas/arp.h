#ifndef _ARP_H
#define _ARP_H

#include "eth.h"
#include "ipv4.h"
#include <netinet/in.h>


/*
 * int arp_resolve(eth_iface_t * iface, ipv4_addr_t src, ipv4_addr_t dest,
 *	           mac_addr_t mac);
 * DESCRIPCIÓN:
 *   Esta función permite realizar una resolución de dirección MAC dada la
 *   dirección IPv4.
 *
 * PARÁMETROS:
 *   'iface': Puntero al descriptor de la interfaz Ethernet utilizada.
 *      'src: Dirección IPv4 del host que hace la consulta.
 *    'dest': Dirección IPv4 del host del cual se desea hacer la resolución.
 *     'mac': Variable donde se guardará la dirección MAC resuelta.
 *
 * VALOR DEVUELTO:
 *   Devuelve 1 si ha podido resolver la dirección y 0 en caso contrario.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error.
 */
int arp_resolve(eth_iface_t * iface, ipv4_addr_t src, ipv4_addr_t dest,
		mac_addr_t mac);

#endif /* _ARP_H */


