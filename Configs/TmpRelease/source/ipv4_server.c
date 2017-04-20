#include "ipv4.h"
#include "ipv4_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{

	/* Dirección IPv4 fuente del datagrama */
	ipv4_addr_t ipv4_src;
	char ipv4_src_str[IPv4_STR_MAX_LENGTH];
	
	/* Buffer con tamaño máximo de payload para almacenarlo */
	unsigned char buffer[IPv4_MAX_PAYLOAD];

	/* Como se van a enviar los datos sobre IP para comprobar el funcionamiento
	 * se elige un tipo de protocolo no asignado
	 */ 
	uint16_t protocol = 200;

	/* Inicializa el protocolo IPv4 */
	if (ipv4_open() == -1)
	{
		fprintf(stderr,"main(): ERROR in ipv4_open()\n");
		exit(-1);
	}

	/* Mientras el servidor esté en ejecución estará a la escucha de datagramas
	 * IPv4 provenientes de la dirección y protocolo especificados
     */ 
	int bytes_recv = 0;
	int n_time_out = 0;
	do
	{
		/* Recibe con un timeout de 3 segundos y comprueba el estado de la
		 * recepción.
		 */
		bytes_recv =  ipv4_recv (ipv4_src, protocol, buffer, 3000);
		
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
				fprintf(stderr,"main(): ERROR in ipv4_send()\n");
				exit(-1);
			}
		}

		n_time_out++;
	}
	while ( (!bytes_recv)|| (n_time_out < 3) );
	
	/* Se envían los datos de vuelta al cliente y se comprueba si ha habido 
	 * algún error.
	 */
	int bytes_sent =  ipv4_send (ipv4_src, protocol, (unsigned char *)buffer, bytes_recv);
	
	if (bytes_sent >= 0)
	{
		printf("\n%d bytes sent back to host %s\n\n", bytes_sent, ipv4_src_str);
	}
	else
	{
		fprintf(stderr,"main(): ERROR in ipv4_send()\n");
		exit(-1);
	}
	
	/* "Cierra" IPv4 */
	if (ipv4_close() == -1)
	{
		fprintf(stderr,"main(): ERROR in ipv4_close()\n");
		exit(-1);
	}

	return 0;
}

