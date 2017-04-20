#include "udp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLIENT_PORT 12345
#define SERV_PORT 54321

int main (int argc, char *argv[])
{

	/* Dirección IPv4 fuente del datagrama */
	ipv4_addr_t ipv4_src;
	char ipv4_src_str[IPv4_STR_MAX_LENGTH];
	
	/* Buffer con tamaño máximo de payload para almacenarlo */
	unsigned char buffer[UDP_MAX_PAYLOAD];
	
	udp_port_t port_src;

	udp_port_t client_port;
	udp_bin_port(client_port, CLIENT_PORT);
	udp_port_t server_port;
	udp_bin_port(server_port, SERV_PORT);


	/* Inicializa el protocolo UDP */
	if (udp_open() == -1)
	{
		fprintf(stderr,"main(): ERROR in udp_open()\n");
		exit(-1);
	}

	/* Mientras el servidor esté en ejecución estará a la escucha de datagramas
	 * IPv4 provenientes de la dirección y protocolo especificados
     */ 
	int bytes_recv = 0;
	do
	{
		/* Recibe con un timeout de 3 segundos y comprueba el estado de la
		 * recepción.
		 */
		bytes_recv =  udp_recv (port_src, ipv4_src, server_port, buffer, 3000);
		
		ipv4_addr_str(ipv4_src, ipv4_src_str);

		if (bytes_recv > 0)
		{
			printf("%d bytes received from host %s: %s\n\n", bytes_recv, ipv4_src_str, buffer);
		}
		else
		{
			if (bytes_recv == 0)
			{
				printf("Timeout: 0 bytes received.\nTrying again...\n\n");
			}
			else
			{
				fprintf(stderr,"main(): ERROR in udp_recv()\n");
				exit(-1);
			}
		}

	}
	while (!bytes_recv);
	
	/* Se envían los datos de vuelta al cliente y se comprueba si ha habido 
	 * algún error.
	 */
	int bytes_sent =  udp_send (server_port, port_src, ipv4_src, (unsigned char *)buffer, bytes_recv);
	
	if (bytes_sent >= 0)
	{
		printf("\n%d bytes sent back to host %s\n\n", bytes_sent, ipv4_src_str);
	}
	else
	{
		fprintf(stderr,"main(): ERROR in udp_send()\n");
		exit(-1);
	}
	
	/* "Cierra" UDP */
	if (udp_close() == -1)
	{
		fprintf(stderr,"main(): ERROR in udp_close()\n");
		exit(-1);
	}

	return 0;
}

