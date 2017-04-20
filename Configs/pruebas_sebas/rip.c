#include "rip.h"
#include "rip_route_table.h"

extern struct ipv4_handler ipv4_handler;

/*
 * Estructura que define un datagrama RIPv2
 */
typedef struct rip_datagram
{
  unsigned char command;  //TIPO: REQUEST || RESPONSE
  unsigned char version;  //VERSION: RIPv2
  uint16_t zero;
  struct rip_entry rip_entry[MAX_ENTRIES]; //ARRAY EN EL QUE SE ALMACENAN HASTA 25 RUTAS PARA ENVIARLAS EN UN DATAGRAMA
} rip_datagram_t;

/*
*int rip_recv(udp_port_t src, udp_port_t dst, ipv4_addr_t from, 
			 unsigned char *buffer, unsigned char *opcode, long int timeout)
*
* DESCRIPCION:
* Esta función se encarga de recibir los mensajes rip de la red, analizarlos y
* validarlos. Si el mensaje es válido guarda en el buffer las rutas recibidas,
* si no, descarta el paquete.
* 
* PARAMETROS:
* "src":	Puntero donde se guarda el puerto desde el que se envía el datagrama.
* "dst":	Puerto al que se envió el datagrama.
* "from":	Puntero donde se guarda la IP origen del datagrama.
* "buffer":	Puntero donde se guardan las rutas del datagrama RIPv2 recibido.
* "opcode":	Puntero donde se guarda el tipo de mensaje recibido (RESPONSE o REQUEST)
* "timeout":Tiempo que estará esperando hasta recibir algo.
* 
* VALOR DEVUELTO:
* Devuelve el número de bytes que ocupan las rutas validas recibidas guardadas
* en el buffer.
*
* ERRORES:
* Si ha habido errores al recibir devuelve -1
*
*/
int rip_recv(udp_port_t src, udp_port_t dst, ipv4_addr_t from, 
			 unsigned char *buffer, unsigned char *opcode, long int timeout)
{

	// Estructura donde se guardara el datagrama recibido
	struct rip_datagram *rip_dgm;
	
	// Variable donde se guardara el numero de bytes recibidos
	int bytes_read = 0;
  
	// Recibimos desde UDP un tiempo "timeout" en ms
	bytes_read = udp_recv(src, from, dst, buffer, timeout);
	
	// Transformamos el puerto udp del que se recibe desde orden de red, a orden
	// de host, y lo guardamos en el puntero que nos pasan por parametros. 
	uint16_t src_bin = ntohs(udp_port_bin(src));
	udp_bin_port(src, src_bin);
	
	// Transformamos la dirección de la que se recibe desde orden de red, a 
	// orden de host, y lo copiamos al puntero pasado por parametros.
	uint32_t ip_addr = ntohl(ipv4_addr_bin(from));	
	memcpy(from, &ip_addr, IPv4_ADDR_SIZE);
	
	if (bytes_read < 0)
	{
		fprintf(stderr, "rip_recv(): ERROR en udp_recv()\n");
		return -1;
	}
	else if (!bytes_read)
	{
		return 0;
	}
	else
	{
		// Comprueba que la IP que nos envía el paquete RIPv2 es un vecino, o lo
		// que es lo mismo, que esta en la misma subred.
 		if (ipv4_route_lookup((ipv4_route_t *)&ipv4_handler, from) == -1)
			return 0;
		
		// Capturamos el datagrama desde el buffer.
		rip_dgm = (struct rip_datagram *)buffer;
		
		// Conviertimos a orden de host todas las rutas recibidas.
		int k;
		for (k = 0; k < sizeof(rip_dgm->rip_entry)/sizeof(rip_route_t); k++)
		{
			rip_route_ntoh(&(rip_dgm->rip_entry[k]));
		}
		
		// Si es un RESPONSE comprueba que venga del puerto 520, ya que estos
		// siempre vienen de ese puerto.
		if (rip_dgm->command == RIP_RESPONSE)
		{
			puts("#DEBUG# Recibido RIP_RESPONSE");
			udp_port_t aux;
			udp_bin_port(aux, 520);
			
			if (memcmp(src, aux, sizeof(udp_port_t)) != 0) 
				return 0;
		}
		//No toco, pero el de arriba o el de abajo debería compararse con un RIP_REQUEST, no sé cual en concreto.
		else if (rip_dgm->command == RIP_RESPONSE)
		{
			puts("#DEBUG# Recibido RIP_REQUEST");
		}
		
		// Calculamos el numero de rutas recibidas para facilitar la validacion
		int num_entries = ( (bytes_read - RIP_HEADER_LEN) /sizeof(rip_route_t) );
        
        //Mensajes de depuracion
        printf("#DEBUG#Paquete RIP: La informacion aqui mostrada puede ser erronea solo sirve para depurar\n");
        char ip_src[IPv4_STR_MAX_LENGTH];
        ipv4_addr_str(from,ip_src);
        printf("#DEBUG#Origen: IP:%s,Puerto:%d\n",ip_src, udp_port_bin(src));
        printf("#DEBUG#Destino: IP:La ip del equipo...,Puerto:%d\n", udp_port_bin(dst));    
        printf("#DEBUG#Command:%d, Version:%d Num_entrys:%d \n",rip_dgm->command, rip_dgm->version, num_entries);
        //Mensajes de depuracion
		
		if (num_entries < 0) return 0;

        puts("\n\n#DEBUG#hasta aca llega :D \n\n");
        
		// Auxiliar para guardar las rutas validas
		rip_route_t aux[num_entries];
		
		// Variable con todas las rutas recibidas
		rip_route_t routes[num_entries];		
        memcpy(routes, rip_dgm->rip_entry, (bytes_read - RIP_HEADER_LEN) );	
		
		// Validamos las rutas y guardamos el numero de rutas validas en
		// valid_routes
		int i, valid_routes = 0;
		for (i = 0; i < num_entries; i++)
		{
			if (is_valid_route(routes[i].ip_addr, routes[i].mask, routes[i].metric))
			{
				memcpy(&aux[valid_routes], &routes[i], sizeof(rip_route_t));
				valid_routes++;
			}
		}
		
		// Guardamos el tamaño que ocupan las rutas validas
		int valid_routes_size = sizeof(rip_route_t)*valid_routes;
		
        puts("#DEBUG#rip_rcv()-Imprime Ruta\n");
        rip_route_print(&rip_dgm->rip_entry[0]);
		
		// Guardamos el opcode en la variable que nos pasan por parametros y
		// copiamos las rutas validas al buffer
        *opcode = rip_dgm->command;
		memcpy(buffer, aux, valid_routes_size);
        
        //Mensajes de depuracion
        printf("#DEBUG#Rutas validas:%d Opcode:%d\n",valid_routes, (int)(*opcode));
        printf("#DEBUG#Valid_routes:%d routes_size:%d\nrip_rcv()-Imprime Ruta\n",valid_routes,valid_routes_size);
        rip_route_print((rip_route_t *)buffer);
        //Mensajes de depuracion
        
        // Devolvemos el tamaño de las rutas validas guardadas en el buffer
		return valid_routes_size;
	}
}

/*
* int rip_send(ipv4_addr_t dst,udp_port_t port_dst, unsigned char* table, 
*			 int len, int op_code) 
* 
* DESCRIPCION:
* Esta función se encarga de enviar la tabla de rutas pasada por parametros.
* 
* PARAMETROS:
* 
* 'dst': 	  IP a la que le va dirigido el paquete.
* 'port_dst': Puerto al que va dirigido el paquete a la maquina destino.
* 'table':	  Tabla en la cual están almacenadas las rutas a enviar.
* 'len':	  Longitud de la tabla de rutas.
* 'op_code':  Indica si el paquete es un response o un request
* 
* VALOR DEVUELTO:
* Devuelve el número de bytes enviados si no se ha producido error
* 
* ERROR
* Devuelve '-1' si no ha podido enviarse el datagrama.
*
*/
int rip_send(ipv4_addr_t dst, udp_port_t port_dst, unsigned char* table, 
			 int len, int op_code) 
{
	// Variable para guardar el numero de bytes enviados
	int bytes_send = 0;
	
	// Calculamos la longitud total del datagrama y lo declaramos
	int total_len = RIP_HEADER_LEN + len;
	unsigned char rip_datagram[total_len];
	
	udp_port_t rip_port;//Desde donde enviamos
	udp_bin_port(rip_port, RIP_PORT);
	
	// Rellenamos el datagrama con datos comunes a todo tipo de envios
	((struct rip_datagram *)rip_datagram)->version = VERSION;
	((struct rip_datagram *)rip_datagram)->zero = 0;
	
	// Copia la tabla de rutas al array de rip_entry en el datagrama rip			
	memcpy(((struct rip_datagram *)rip_datagram)->rip_entry, table, len);
		
	// Convierte a orden de red todas las rutas del datagrama
	int i;
	for (i = 0; i < len/sizeof(rip_route_t); i++)
	{
		rip_route_hton(&(((struct rip_datagram *)rip_datagram)->rip_entry[i]));
	}

	// Miramos el command del paquete (request o response)

	if(op_code == RIP_REQUEST)
	{
		//Mensajes de depuracion
        printf("#DEBUG#Envio paquete RIP REQUEST\n");
        char ip_dst[IPv4_STR_MAX_LENGTH];
        ipv4_addr_str(dst,ip_dst);
        printf("#DEBUG#Origen: IP:La ip del equipo...,Puerto:%d\n", udp_port_bin(port_dst));
        printf("#DEBUG#Destino: IP:%s,Puerto:%d\n",ip_dst,udp_port_bin(rip_port));    
        printf("#DEBUG#Command:%d, Num_entrys:%d \n",RIP_REQUEST, (int)(len/sizeof(rip_route_t)));
		
		((struct rip_datagram *)rip_datagram)->command = RIP_REQUEST;
		
		if((bytes_send = udp_send(port_dst, rip_port, dst, rip_datagram,
			total_len)) < 0)
		{
			fprintf(stderr, "send_updates(): ERROR en udp_send()\n");
				return -1; 
		}else return bytes_send;
	}else if(op_code == RIP_RESPONSE)
	{
		//Mensajes de depuracion
        printf("#DEBUG#Envio paquete RIP RESOPNSE\n");
        char ip_dst[IPv4_STR_MAX_LENGTH];
        ipv4_addr_str(dst,ip_dst);
        printf("#DEBUG#Origen: IP:La ip del equipo...,Puerto:%d\n", udp_port_bin(rip_port));
        printf("#DEBUG#Destino: IP:%s,Puerto:%d\n",ip_dst,udp_port_bin(port_dst));     
        printf("#DEBUG#Command:%d, Num_entrys:%d \n",RIP_REQUEST, (int)(len/sizeof(rip_route_t)));
		
		((struct rip_datagram *)rip_datagram)->command = RIP_RESPONSE;

		if((bytes_send = udp_send(rip_port, port_dst, dst, rip_datagram,
			total_len)) < 0)
		{
			fprintf(stderr, "send_updates(): ERROR en udp_send()\n");
				return -1; 
		}else return bytes_send;
	}else return -1;
	/*if(op_code == RIP_REQUEST || op_code == RIP_RESPONSE)
	{	
		// Ponemos el opcode del paquete
		((struct rip_datagram *)rip_datagram)->command = op_code;

		// Enviamos el datagrama rip
		if ((bytes_send = udp_send(port_dst, rip_port, dst, rip_datagram, 
			total_len)) < 0)
		{
			fprintf(stderr, "send_updates(): ERROR en udp_send()\n");
				return -1; 
		}
		else
		{
			return bytes_send;
		}		
	}
	else
	{
		// Si el op_code no es request o response es un error
		return -1;
	}*/
	
	return bytes_send;
}

/*
* int rip_open ()
* DESCRIPCION
* Esta funcion se encarga de inicializar el protocolo rip para poder enviar datos
*
* PAR�METROS:
*   Ninguno.
*
* VALOR DEVUELTO:
*   Devuelve 0 si se ha podido inicializar el protocolo.
*
* ERRORES:
*   La funci�n devuelve '-1' si se ha producido alg�n error.
*/
int rip_open()
{
    if (udp_open() == -1)
    {
        fprintf(stderr,"rip_open(): ERROR in udp_open()\n");
        exit(-1);
    }
    
    return 0;	
}

/*
 * int rip_close ()
 *
 * DESCRIPCI�N:
 *   Esta funci�n finaliza el protocolo rip.
 *
 * PAR�METROS:
 *   Ninguno.
 *
 * VALOR DEVUELTO:
 *   Devuelve 0 si se ha podido finalizar la instancia del protocolo.
 *
 * ERRORES:
 *   La funcion devuelve '-1' si se ha producido algun error.
 */
int rip_close()
{
    if (udp_close() == -1)
    {
        fprintf(stderr,"rip_close(): ERROR in udp_close()\n");
        exit(-1);
    }
    
    return 0;
}

