#include "rip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>

/*
El cliente generar� mensajes RIP Request de toda la tabla,enviados en unicast a 
la direcci�n IPv4 especificada por l�nea de par�metros.
El cliente espera asimismo respuesta del servidor RIP, que puede ser tanto un 
servidor RIPv2 est�ndar como el servidor rip_server.c, e imprime por pantalla el
contenido m�s significativo de la respuesta recibida.
*/
int main(int argc, char *argv[])
{
	rip_open();
	
	if (argc != 2)
	{
		fprintf(stderr,"Bad input params: ./rip_client ip_addr\n");
		return 1;
	}
	
	ipv4_addr_t dst;
	ipv4_str_addr(argv[1], dst);
	
	int num_bytes = 0;
	int timeout = 3000;
//	rip_route_t *routes;

	struct rip_entry rip_route;
	
	memset(&rip_route,0,RIP_ROUTE_LEN);
	
	rip_route.metric = 16;
	
	srand(time(NULL));//Selecciona una semilla distinta para el rand dependiendo de la hora del sistema.
	int rand_port = rand()%2001 + 3000;//Genera un puerto aleatorio entre 2000 y 5000
	udp_port_t rip_port;
	udp_bin_port(rip_port, rand_port); //copia el puerto aleatorio en la variable rip_port
	
	do
	{
		puts("#DEBUG# Enviando paquete rip");
        num_bytes = rip_send(dst, rip_port, (unsigned char *)&rip_route, RIP_ROUTE_LEN, RIP_REQUEST);
	}
	while (!num_bytes);
	
    if (num_bytes < 0) 
    {
        puts("main(): ERROR en rip_send()\n");
        exit(-1);
    }
    
    unsigned char op_code;
    ipv4_addr_t ip_src;
    udp_port_t port_dst;
    udp_bin_port(port_dst, 520); //Copiamos en port_dst el valor del puerto rip al que se enviarán datagramas desde el cliente.
    udp_port_t port_src;
    unsigned char buffer[RIP_MAX_SIZE];


    int i=0;
    do
    {
		puts("#DEBUG# Recibiendo paquete rip");
        num_bytes = rip_recv(port_src, rip_port, ip_src, buffer, &op_code, timeout); //intenta recibir si no recibe intenta de nuevo hasta 3 veces.
    
        if (num_bytes > 0)
        {
            printf("%d bytes received from server: %x\n\n", num_bytes, *buffer);
            
            int n_entries = num_bytes/sizeof(rip_route_t); //Calculamos el número de entradas que hay en la lista de rutas.
            rip_route_t rip_entrys[n_entries];
            memcpy(rip_entrys,buffer,num_bytes);

            int j;
            for(j = 0;j<n_entries;j++)
            {
		rip_entrys[j].metric += 1;
		memcpy(&(rip_entrys[j].next_hop), ip_src, IPv4_ADDR_SIZE);//modificamos el next_hop y la metrica de las rutas 
                rip_route_print(&rip_entrys[j]);
            }
        }
        else
        {
            if (num_bytes == 0)
            {
                printf("Timeout: 0 bytes received.\nTrying again...\n\n");
            }
            else if (num_bytes < 0) 
            {
                puts("main(): ERROR en rip_recv()\n");
                exit(-1);
            }
        }
        i++;
    }while(num_bytes < 0 && i<3 );
    
    
    
    
	//Aca se queda recibiendo e imprime la respuesta

// 	rip_open();
// 	
// 	ipv4_addr_t ipv4_dst;
// 	ipv4_str_addr(argv[1], ipv4_dst);
// 	
// 	struct rip_entry rip_route;
// 	
// 	memset(&rip_route,0,RIP_ROUTE_LEN);
// 	
// 	rip_route.metric = htonl(16);
// 	
// 	udp_port_t rip_port;//Desde donde enviamos
// 	//udp_port_t rip_port1;//A donde envamos
// 	udp_bin_port(rip_port, RIP_PORT);
// 	
// 	rip_send(ipv4_dst, rip_port,(unsigned char *)&rip_route, RIP_ROUTE_LEN, RIP_REQUEST);
// 
// 	puts("enviado");
// 	
// 	rip_close();
// 	
 	return 0;
}
