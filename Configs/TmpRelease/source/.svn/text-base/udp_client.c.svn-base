#include "udp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLIENT_PORT 12345
#define SERV_PORT 54321

int main (int argc, char *argv[])
{	
	ipv4_addr_t ipv4_dst;				// Dirección IPv4 destino
	char *datos = "Prueba de envio UDP";		// Datos a enviar (19 bytes)
	
	/* Buffer con tamaño máximo de payload para almacenarlo */
	unsigned char buffer[IPv4_MAX_PAYLOAD];
	
	udp_port_t port_src;				// Puntero para guardar puerto origen
	ipv4_addr_t ipv4_src;				// Puntero para guardar IP origen

	udp_port_t client_port;
	udp_bin_port(client_port, CLIENT_PORT);
	udp_port_t server_port;
	udp_bin_port(server_port, SERV_PORT);


	if (argc != 2)
	{
		printf("Bad usage: udp_client dst_ipaddr\n");
		exit(-1);
	}
	
	/* Convierte dirección IPv4 de cadena de caracteres a estructura ipv4_addr_t
	 */ 
	if (ipv4_str_addr(argv[1], ipv4_dst) == -1)
	{
		printf("Invalid IP address: %s\n", argv[1]);
		exit(-1);
	}
	
	/* Inicializa el protocolo UDP */
	if (udp_open() == -1)
	{
		fprintf(stderr,"main(): ERROR in udp_open()\n");
		exit(-1);
	}

	/* Se envían los datos (7 bytes) al servidor y se comprueba si ha habido
	 * algún error.
	 */
	int check =  0;

	/* Después de enviar el cliente estará a la escucha de datagramas UDP 
	 * provenientes del servidor.
     */ 
	do
	{
		//*datos = htonl(*datos);
		check =  udp_send (client_port, server_port, ipv4_dst, (unsigned char*)datos, 20);

		if (check >= 0)
		{
			printf("\n%d bytes sent to host %s: %s\n\n", check, argv[1], datos);
		}
		else
		{
			fprintf(stderr,"main(): ERROR in udp_send()\n");
			exit(-1);
		}
		
		/* Recibe con un timeout de 3 segundos y comprueba el estado de la
		 * recepción.
		 */
		check =  udp_recv (port_src, ipv4_src, client_port, buffer, 3000);

		if (check > 0)
		{
			printf("%d bytes received from server: %s\n\n", check, buffer);
		}
		else
		{
			if (check == 0)
			{
				printf("Timeout: 0 bytes received.\nTrying again...\n\n");
			}
			else
			{
				fprintf(stderr,"main(): ERROR in udp_send()\n");
				exit(-1);
			}
		}

	}
	while (!check);
	
	/* "Cierra" UDP */
	if (udp_close() == -1)
	{
		fprintf(stderr,"main(): ERROR in udp_close()\n");
		exit(-1);
	}

	return 0;
}
