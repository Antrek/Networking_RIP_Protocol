#ifndef _ARP_H
#define _ARP_H

#include "eth.h"
#include "ipv4.h"
#include "arp_cache_table.h"

#include <netinet/in.h>

/* ID protocolo ARP en Ethernet */
#define ARP_TYPE 0x0806

/* ID petición ARP */
#define REQUEST 0x0001

/* ID respuesta ARP */
#define REPLY 0x0002

/* Longitud en bytes de la cabecera ARP */
#define ARP_LEN 28

/* Typedef de la estructura frame de arp */
typedef struct arp_frame arp_frame_t;

/* Escructura de la cabecera ARP según RFC826 */
struct arp_frame
{
    unsigned short hrd_type;    
    unsigned short prot_type;
    unsigned char hrd_len;
    unsigned char prot_len;
    unsigned short opcode;
    mac_addr_t mac_src;
    ipv4_addr_t ip_src;
    mac_addr_t mac_dst;
    ipv4_addr_t ip_dst; 
};

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


