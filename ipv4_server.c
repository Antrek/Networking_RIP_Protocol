#include "ipv4.h"
#include "ipv4_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{

	/* Dirección IPv4 fuente del datagrama */
	ipv4_addr_t ipv4_src;
	
	/* Buffer con tamaño máximo de payload para almacenarlo */
	unsigned char buffer[IPv4_MAX_PAYLOAD];

	/* Como se van a enviar los datos sobre IP para comprobar el funcionamiento
	 * se elige un tipo de protocolo no asignado
	 */ 
	uint16_t protocol = 200;

	if (argc != 2)
	{
		printf("Bad usage: ip_server src_ipaddr\n");
		exit(-1);
	}

	/* Convierte dirección IPv4 de cadena de caracteres a estructura ipv4_addr_t
	 */ 
	if (ipv4_str_addr(argv[1], ipv4_src) == -1)
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

	/* Mientras el servidor esté en ejecución estará a la escucha de datagramas
	 * IPv4 provenientes de la dirección y protocolo especificados
     */ 
	do
	{
		/* Recibe con un timeout de 3 segundos y comprueba el estado de la
		 * recepción.
		 */
		int check =  ipv4_recv (ipv4_src, protocol, buffer, 3000);

		switch (check)
		{
			case -1:
					fprintf(stderr,"main(): ERROR in ipv4_send()\n");
					exit(-1);
					break;
			case 0:
					printf("Timeout: 0 bytes received.\n");
					break;
			default:
					printf("%d bytes received: %s\n", check, buffer);
					break;
		}
	}
	while (1);
	
	/* "Cierra" IPv4 */
	if (ipv4_close() == -1)
	{
		fprintf(stderr,"main(): ERROR in ipv4_close()\n");
		exit(-1);
	}

	return 0;
}

