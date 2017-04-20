#include "arp.h"
#include "ipv4_config.h"

#include <string.h>
#include <timerms.h>
#include <stdlib.h>


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
 * int arp_resolve ( eth_iface_t * iface, ipv4_addr_t dest, mac_addr_t mac );
 *
 * DESCRIPCIÓN:
 *   Esta función permite realizar una resolución de dirección MAC dada la
 *   dirección IPv4.
 *
 * PARÁMETROS:
 *   'iface': Puntero al descriptor de la interfaz Ethernet utilizada.
 *    'dest': Dirección IPv4 del host del cual se desea hacer la resolución.
 *	   'mac': Variable donde se guardará la dirección MAC resuelta.
 *
 * VALOR DEVUELTO:
 *   Devuelve 1 si ha podido resolver la dirección y 0 en caso contrario.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error.
 */
int arp_resolve (eth_iface_t* iface, ipv4_addr_t dest, mac_addr_t mac)
{

	/* Crea y llena la cabecera de la petición ARP */
	struct arp_frame req;
	req.hrd_type = htons(0x0001);		// Ethernet
	req.prot_type = htons(0x0800);		// IP
	req.hrd_len = 6;					// MAC address length (bytes)
	req.prot_len = 4;					// IP address length (bytes)
	req.opcode = htons(REQUEST);		// ARP request
	
	/* Variables para guardar la configuración IP y sacar así la dirección IPv4
	 * de la interfaz a utilizar
	 */
	char ifname[5];
	ipv4_addr_t ipv4_src;
	ipv4_addr_t netmask;
	
	if (ipv4_config_read(IP_CONF, ifname, ipv4_src, netmask) == -1)
	{
		fprintf(stderr,"arp_resolve(): ERROR in ipv4_config_read()\n");
		exit(-1);
	}
	
	/* Rellena direcciones MAC e IPv4 de la petición ARP */
	eth_getaddr (iface, req.mac_src);	// Dirección MAC de la interfaz
	memcpy(req.ip_src, ipv4_src, IPv4_ADDR_SIZE);	// Source IP address
	memset(req.mac_dst, 0, MAC_ADDR_SIZE);			// MAC broadcast
	memcpy(req.ip_dst, dest, IPv4_ADDR_SIZE);		// Destination IP address

	/* Envía petición ARP mediante Ethernet*/
	eth_send (iface, MAC_BCAST_ADDR, ARP_TYPE, (unsigned char *)&req, ARP_LEN);

	/* Espera la respuesta */
	struct arp_frame *rep;				// Cabecera ARP de la respuesta
	unsigned char found = 0;			// Flag de control
	unsigned char buffer[ETH_MTU];		// Buffer para la trama Ethernet
	long int timeout = 1000;			// Tiempo de espera en milisegundos
  	timerms_t timer;					// Temporizador
	long int time_left;					// Tiempo restante
	
	/* Inicializa el temporizador */
	timerms_reset(&timer, timeout);
	
	/* Bucle hasta que se reciba la respuesta o expire el temporizador */
	do
	{
		// Calcula el tiempo restante
		time_left = timerms_left(&timer);	
		
		/* Espera a recibir una trama mediante Ethernet hasta que agote el
		 * temporizador o hasta recibir una trama ARP.
		 */
		char check = eth_recv (iface, req.mac_src, ARP_TYPE, buffer,time_left);

		//Si hay error en la recepción sale de la función
		if(check <= 0)
		{
			return check;
		}

		/* Captura la cabecera de la trama ARP */
		rep = (struct arp_frame *)buffer;
		
		/* Si la trama ARP es una respuesta y es la que esperamos guarda la
		 * dirección MAC resuelta y marca el flag
		 */
		if ((ntohs(rep->opcode) == REPLY) && (memcmp(rep->ip_src,
											req.ip_dst,IPv4_ADDR_SIZE) == 0))
		{
			memcpy(mac, rep->mac_src, MAC_ADDR_SIZE);
			found = 1;
		}

	}
	while (!found && time_left > 0);

	return found;
}


