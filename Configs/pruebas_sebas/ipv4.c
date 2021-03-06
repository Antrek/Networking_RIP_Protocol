#include "ipv4.h"
#include "ipv4_config.h"
#include "ipv4_route_table.h"
#include "arp.h"

#include <timerms.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Identificador de protocolo IPv4 para Ethernet */
#define IP_TYPE 0x0800



/*
 * Estructura que define el datagrama IPv4, teniendo 20 bytes estáticos de
 * cabecera y 1480 de payload (MTU de Ethernet - cabecera) * 
 */
struct ipv4_datagram
{
	/******************************************/
	/* IPv4 datagram header (20 bytes header) */	
	
	/* version (4 bits) + header length (4 bits) */
	uint8_t ver_hlen;
	uint8_t tos;
	uint16_t total_len;
	uint16_t id;
	/* flags (3 bits) + offset (13 bits) */
	uint16_t flags_off;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t checksum;
	ipv4_addr_t ipv4_src;
	ipv4_addr_t ipv4_dst;
	/*****************************************/
	
	unsigned char payload[IPv4_MAX_PAYLOAD];
};

/*
 * Estructura que permite manejar con facilidad los parametros de configuración
 * local leídos del fichero de configuración de IPv4.
 */
struct ipv4_handler
{
	ipv4_addr_t ipv4_src;
	ipv4_addr_t netmask;
	eth_iface_t *iface;
};

/* Variable global para guardar la configuración IPv4 leída */
struct ipv4_handler ipv4_handler;

/* Variable global que contiene la tabla de rutas */
ipv4_route_table_t *ipv4_route;

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
uint32_t ipv4_addr_bin(ipv4_addr_t ipv4_addr)
{
	// Dirección a devolver.
	uint32_t bin_addr = 0;
	
	int i, j;
	for (i = 0, j = 3; i < 4; i++, j--)
	{
		/* Lee un byte del array, lo desplaza tantos bits a la izquierda como 
		 * sea necesario y lo suma a la dirección binaria.
		 */
		bin_addr += (ipv4_addr[i] << 8*j);	
		
	}	
	
	return bin_addr;
}	

void ipv4_bin_addr(unsigned char *ipv4_addr, uint32_t ipv4_bin_addr)
{
	ipv4_addr_t aux;

	aux[0] = ipv4_bin_addr&0xFF000000 >> 24;
	aux[1] = ipv4_bin_addr&0x00FF0000 >> 16;
	aux[2] = ipv4_bin_addr&0x0000FF00 >> 8;
	aux[3] = ipv4_bin_addr&0x000000FF;

	memcpy(ipv4_addr, aux, 4);
}

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
int ipv4_open ()
{	

	char ifname[5];	// Variable para guardar el nombre de la interfaz Ethernet

	/* Lee el archivo de configuración IP */
	if (ipv4_config_read(IP_CONF, ifname, ipv4_handler.ipv4_src, 
						ipv4_handler.netmask) == -1)
	{
		fprintf(stderr,"ipv4_open(): ERROR in ipv4_config_read()\n");
		exit(-1);
	}	

	/* Crea tabla de rutas */
	ipv4_route = ipv4_route_table_create();
	 
	/* Lee tabla de rutas del archivo de configuración y lo almacena */
	if (ipv4_route_table_read (IP_ROUTE_TABLE, ipv4_route) == -1)
	{
		fprintf(stderr,"ipv4_open(): ERROR in ipv4_route_table_read()\n");
		exit(-1);
	}	

	/* Inicializa Ethernet */
	if((ipv4_handler.iface = eth_open (ifname)) == NULL)
	{
		fprintf(stderr, "ipv4_open(): ERROR en eth_open()\n");
		return -1;
	}

	return 0;
}

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
int ipv4_close ()
{
	/* Cierra Ethernet */
	if(eth_close (ipv4_handler.iface) == -1)
	{
		fprintf(stderr, "ipv4_close(): ERROR en eth_close()\n");
		return -1;
	}

	/* Libera tabla de rutas */
	ipv4_route_table_free(ipv4_route);

	return 0;
}

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
int ipv4_send (ipv4_addr_t ipv4_dst, uint16_t protocol, unsigned char * payload,
				int payload_len )
{
	int bytes_sent;

	struct ipv4_datagram ipv4_dgram;	// Datagrama a enviar
	mac_addr_t mac_dst;
	
	/* Rellena la cabecera IPv4 */
	memset(&ipv4_dgram, 0, sizeof(struct ipv4_datagram));
	ipv4_dgram.ver_hlen = 0x45;
	ipv4_dgram.total_len = htons(20 + payload_len);
	/* ID fijo para identificar correcto funcionamiento de cliente-servidor */
	ipv4_dgram.id = htons(1234); 
	ipv4_dgram.ttl = 64;
	ipv4_dgram.protocol = protocol;
	
	/* Convierte direcciones a orden de red */
	uint32_t src = htonl(ipv4_addr_bin(ipv4_handler.ipv4_src));
	uint32_t dst = htonl(ipv4_addr_bin(ipv4_dst));	
	
	/* Guarda direcciones en la cabecera del datagrama */
	memcpy(ipv4_dgram.ipv4_src, &src, IPv4_ADDR_SIZE);
	memcpy(ipv4_dgram.ipv4_dst, &dst, IPv4_ADDR_SIZE);

	ipv4_dgram.checksum = htons( ipv4_checksum((unsigned char *)(&ipv4_dgram),
												IPv4_HEAD_SIZE));
		
	memcpy(ipv4_dgram.payload, payload, payload_len);

	ipv4_route_t *dst_route;
	if ((dst_route = ipv4_route_table_lookup(ipv4_route, ipv4_dst)) == NULL)
	{
		fprintf(stderr, "ipv4_send(): ERROR en ipv4_table_lookup()\n");
		return -1;
	}

	ipv4_addr_t arp_addr;
	
	if (ipv4_addr_bin(dst_route->subnet_mask) == 0)
	{
		memcpy(arp_addr, dst_route->gateway_addr, IPv4_ADDR_SIZE);
	}
	else
	{
		memcpy(arp_addr, ipv4_dst, IPv4_ADDR_SIZE);
	}

	int status = arp_resolve(ipv4_handler.iface,ipv4_handler.ipv4_src, arp_addr,
						 mac_dst);

	if (status < 0)
	{
		fprintf(stderr, "ipv4_send(): ERROR en arp_resolve()\n");
		return -1;
	}
	else
	{
		if (status == 0)
		{			
			fprintf(stderr, "ipv4_send(): arp_resolve(), didn't get reply\n");
				return -1;
		}
	}

	if ((bytes_sent = eth_send (ipv4_handler.iface, mac_dst, IP_TYPE,
		(unsigned char *)&ipv4_dgram, IPv4_HEAD_SIZE + payload_len)) < 0)
	{
		fprintf(stderr, "ipv4_send(): ERROR en eth_send()\n");
		return -1;
	}

	return (bytes_sent - IPv4_HEAD_SIZE);
}

/*
 * int ipv4_recv(ipv4_addr_t ipv4_src, uint16_t protocol, unsigned char *buffer,
 *				 long int timeout );
 *
 * DESCRIPCIÓN:
 *   Esta función permite recibir un datagrama IPv4 desde el nivel de enlace
 *	 especificando el protocolo de la capa superior.
 *
 * PARÁMETROS:
 * 'ipv4_src': Variable para guardar la dirección IP origen.
 * 'protocol': Protocolo de la capa superior.
 *   'buffer': Buffer para almacenar los datos recibidos.
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
int ipv4_recv (ipv4_addr_t ipv4_src, uint16_t protocol, unsigned char *buffer,
			   long int timeout )
{
	timerms_t timer;
	timerms_reset(&timer, timeout);

	int bytes_read, recv = 0;
	unsigned char ip_buffer[ETH_MTU];
	struct ipv4_datagram *ip_dgm;
	mac_addr_t mac_src;
	
	
	do
	{
		long int time_left = timerms_left(&timer);
		if ((bytes_read = eth_recv (ipv4_handler.iface, mac_src, IP_TYPE,
									ip_buffer, time_left)) < 0 )
		{
			fprintf(stderr, "ipv4_recv(): ERROR en eth_recv()\n");
			return -1;
		}
		else
		{
			if (!bytes_read) return 0;
		}

		ip_dgm = (struct ipv4_datagram *)ip_buffer;
		
		if (ip_dgm->protocol == protocol) recv = 1;
			
		memcpy(ipv4_src, ip_dgm->ipv4_src, IPv4_ADDR_SIZE);
	}
	while(!recv);
	
	uint32_t payload_len = ntohs(ip_dgm->total_len) - IPv4_HEAD_SIZE;

	memcpy(buffer, ip_dgm->payload, payload_len);

	return payload_len;
}

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
uint16_t ipv4_checksum ( unsigned char * data, int len )
{
  int i;
  uint16_t word16;
  unsigned int sum = 0;
    
  /* Make 16 bit words out of every two adjacent 8 bit words in the packet
   * and add them up */
  for (i=0; i<len; i=i+2) {
    word16 = ((data[i] << 8) & 0xFF00) + (data[i+1] & 0x00FF);
    sum = sum + (unsigned int) word16;	
  }

  /* Take only 16 bits out of the 32 bit sum and add up the carries */
  while (sum >> 16) {
    sum = (sum & 0xFFFF) + (sum >> 16);
  }

  /* One's complement the result */
  sum = ~sum;

  return (uint16_t) sum;
}


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
void ipv4_addr_str ( ipv4_addr_t addr, char* str )
{
  if (str != NULL) {
    sprintf(str, "%d.%d.%d.%d",
            addr[0], addr[1], addr[2], addr[3]);
  }
}

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
int ipv4_str_addr ( char* str, ipv4_addr_t addr )
{
  int err = -1;

  if (str != NULL) {
    unsigned int addr_int[IPv4_ADDR_SIZE];
    int len = sscanf(str, "%d.%d.%d.%d", 
                     &addr_int[0], &addr_int[1], 
                     &addr_int[2], &addr_int[3]);

    if (len == IPv4_ADDR_SIZE) {
      int i;
      for (i=0; i<IPv4_ADDR_SIZE; i++) {
        addr[i] = (unsigned char) addr_int[i];
      }
      
      err = 0;
    }
  }
  
  return err;
}

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
void group_mapping_ip_mac(ipv4_addr_t ipv4,mac_addr_t mac)
{
  if(ipv4[0] < 224)
  {
    fprintf(stderr,"The IP address is not a Multicast IP address \n");
    return;
  }
  int i;
  mac[0] = 0x01;
  mac[1] = 0x00;
  mac[2] = 0x5E;
  for(i = 1; i<4; i++)
  {
    if( i == 1)
      if(ipv4[i] > 128)
	ipv4[i] = ipv4[i] - 128;
    mac[i+2] = ipv4[i];
  }
  return;
}
/*
int is_broadcast(ipv4_addr_t ipv4)
{
  ipv4_handler
}
*/