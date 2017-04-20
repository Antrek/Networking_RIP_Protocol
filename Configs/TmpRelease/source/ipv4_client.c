#include "ipv4.h"
#include "ipv4_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{
	ipv4_addr_t ipv4_dst;				// Dirección IPv4 destino
	char *datos = "Prueba de envio IP"; // Datos a enviar (19 bytes)
	
	/* Buffer con tamaño máximo de payload para almacenarlo */
	unsigned char buffer[IPv4_MAX_PAYLOAD];
	
	/* Como se van a enviar los datos sobre IP para comprobar el funcionamiento
	 * se elige un tipo de protocolo no asignado
	 */ 
	uint16_t protocol = 200;

	if (argc != 2)
	{
		printf("Bad usage: ip_client dst_ipaddr\n");
		exit(-1);
	}

	/* Convierte dirección IPv4 de cadena de caracteres a estructura ipv4_addr_t
	 */ 
	if (ipv4_str_addr(argv[1], ipv4_dst) == -1)
	{
		printf("Invalid IP address: %s\n", argv[1]);
		exit(-1);
	}
	
	/* Inicializa el protocolo IPv4 */
	if (ipv4_open() == -1)
	{
		fprintf(stderr,"main(): ERROR in ipv4_open()\n");
		exit(-1);
	}

	/* Se envían los datos (19 bytes) al servidor y se comprueba si ha habido
	 * algún error.
	 */
	int check =  0;

	/* Después de enviar el cliente estará a la escucha de datagramas IPv4 
	 * provenientes del servidor.
     */ 
	do
	{
		check =  ipv4_send (ipv4_dst, protocol, (unsigned char *)datos, 19);

		if (check >= 0)
		{
			printf("\n%d bytes sent to host %s: %s\n\n", check, argv[1], datos);
		}
		else
		{
			fprintf(stderr,"main(): ERROR in ipv4_send()\n");
			exit(-1);
		}

		/* Recibe con un timeout de 3 segundos y comprueba el estado de la
		 * recepción.
		 */
		check =  ipv4_recv (ipv4_dst, protocol, buffer, 3000);

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
				fprintf(stderr,"main(): ERROR in ipv4_send()\n");
				exit(-1);
			}
		}

	}
	while (!check);
	
	/* "Cierra" IPv4 */
	if (ipv4_close() == -1)
	{
		fprintf(stderr,"main(): ERROR in ipv4_close()\n");
		exit(-1);
	}

	return 0;
}
