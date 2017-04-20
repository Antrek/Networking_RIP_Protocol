#include "rip_route_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* rip_route_t * rip_route_create
 * ( ipv4_addr_t subnet, ipv4_addr_t mask, ipv4_addr_t next_hop, uint32_t metric)
 * 
 * DESCRIPCIÓN: 
 *   Esta función crea una ruta RIP con los parámetros especificados:
 *   dirección de subred, máscara, direcci�n IP del siguiente salto y la 
 *   m�trica de la ruta.
 *
 *   Esta función reserva memoria para la estructura creada. Debe utilizar la
 *   función 'rip_route_free()' para liberar dicha memoria.
 *
 * PARÁMETROS:
 *   'subnet': Dirección IP de la subred destino de la nueva ruta.
 *     'mask': Máscara de la subred destino de la nueva ruta.
 * 'next_hop': Dirección IP del encaminador empleado para llegar a la subred
 *             destino de la nueva ruta
 *   'metric': M�trica a la subred espec�ficada.
 *
 * VALOR DEVUELTO:
 *   La función devuelve un puntero a la ruta creada.
 * 
 * ERRORES:
 *   La función devuelve 'NULL' si no ha sido posible reservar memoria para
 *   crear la ruta.
 */
rip_route_t * rip_route_create
( ipv4_addr_t subnet, ipv4_addr_t mask, ipv4_addr_t next_hop, uint32_t metric)
{	
	// Reserva memoria para la ruta creada
	rip_route_t * route = (rip_route_t *) malloc(sizeof(struct rip_entry));

	if (route != NULL) 
	{
		// Asigna valores a los campos de la ruta.
		route->famid = FAM_ID_IP;
		route->route_tag = ROUTE_TAG;
		memcpy(route->ip_addr, subnet, IPv4_ADDR_SIZE);
		memcpy(route->mask, mask, IPv4_ADDR_SIZE);
		memcpy(route->next_hop, next_hop, IPv4_ADDR_SIZE);
		route->metric = metric;
	}

	return route;
}


/* int rip_route_lookup ( rip_route_t * route, ipv4_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función indica si la dirección IP especificada pertence a la
 *   subred indicada. En ese caso devuelve la longitud de la máscara de la
 *   subred.
 * 
 * PARÁMETROS:
 *   'route': Ruta a la subred que se quiere comprobar.
 *    'addr': Dirección RIP destino.
 *
 * VALOR DEVUELTO:
 *   Si la dirección IP pertenece a la subred de la ruta especificada, debe
 *   devolver un número positivo que indica la longitud del prefijo de
 *   subred. Esto es, el número de bits a uno de la máscara de subred.
 *   La función devuelve '-1' si la dirección IP no pertenece a la subred
 *   apuntada por la ruta especificada.
 */
int rip_route_lookup ( rip_route_t * route, ipv4_addr_t addr )
{
	int prefix_length = -1;
	
	uint32_t ip = ipv4_addr_bin(addr);
	uint32_t netmask = ipv4_addr_bin(route->mask);
	uint32_t prefix = ipv4_addr_bin(route->ip_addr);
	
	// "Filtra" la ip y el prefijo con la mascara para obtener las subredes.
	ip = netmask&ip;
	prefix = netmask&prefix;
	
	// Operacion XOR -> si es 0 significa que la ip y el prefijo filtrados son
	// iguales y por lo tanto de la misma subred.
	if ((ip^prefix) == 0)
	{
		// Realiza un AND lógico bit a bit hacia la derecha comenzando por el 31
		// para calcular la longitud del prefijo. 
		int i = 31;
		while ((netmask&(1 << i)) != 0) i--; 
		
		prefix_length = 32 - (i + 1);
	}

	return prefix_length;
}


/* void rip_route_print ( rip_route_t * route );
 *
 * DESCRIPCIÓN:
 *   Esta función imprime la ruta especificada por la salida estándar.
 *
 * PARÁMETROS:
 *   'route': Ruta que se desea imprimir.
 */
void rip_route_print ( rip_route_t * route )
{
	printf("# SubnetAddr  \tSubnetMask    \tNextHop  \tMetric\n");
	
	char subnet_str[IPv4_STR_MAX_LENGTH];
	char mask_str[IPv4_STR_MAX_LENGTH]; 
	char nexthop_str[IPv4_STR_MAX_LENGTH];

	if (route != NULL)
	{
		ipv4_addr_str(route->ip_addr, subnet_str);
		ipv4_addr_str(route->mask, mask_str);
		ipv4_addr_str(route->next_hop, nexthop_str);
		uint32_t metric = route->metric;
		//IMPRIME LA RUTA DESPUES DE HABER TRANSFORMADO DE INT A STRING
		printf("%-15s\t%-15s\t%-15s\t%d\n",
						subnet_str, mask_str, nexthop_str, metric);
	}
}

/* void rip_route_free ( rip_route_t * route );
 *
 * DESCRIPCIÓN:
 *   Esta función libera la memoria reservada para la ruta especificada, que
 *   ha sido creada con 'rip_route_create()'.
 *
 * PARÁMETROS:
 *   'route': Ruta que se desea liberar.
 */
void rip_route_free ( rip_route_t * route )
{
	if (route != NULL) 
	{
		free(route);
	}
}

/* int rip_route_table_add ( rip_route_table_t * table, 
 *                            rip_route_t * route );
 * DESCRIPCIÓN: 
 *   Esta función añade la ruta especificada en la primera posición libre de
 *   la tabla de rutas.
 *
 * PARÁMETROS:
 *   'table': Tabla donde añadir la ruta especificada.
 *   'route': Ruta a añadir en la tabla de rutas.
 * 
 * VALOR DEVUELTO:
 *   La función devuelve la nueva tabla.
 * 
 * ERRORES:
 *   La función devuelve NULL si no ha sido posible añadir la ruta
 *   especificada.
 */
rip_route_table_t *rip_route_table_add ( rip_route_table_t * table,
				 rip_route_t * route )
{
	// Puntero para guardar la nueva tabla
	rip_route_table_t *new_table = NULL;
	
	// Si la ruta a añadir no tiene una metrica válida se devuelve la tabla.
	if((route->metric < 1)  || (route->metric > 16))
	{
		return table;
	}
	
	//RESERVA MEMORIA PARA EL NODO DONDE SE ALMACENA LAS RUTA
	route_node *node = (route_node *)malloc(sizeof(route_node));
	
	if (node != NULL)
	{
		//RESERVA MEMORIA PARA LA ESTRUCTURA DE DATOS QUE IRA EN EL NODO
		node->data = (route_data *)malloc(sizeof(route_data));
		
		if (node->data != NULL)
		{
			// COPIA LA INFORMACION DE LA RUTA EN DATA, AÑADE POR EL PRINCIPIO
			// EL NODO Y PONE EL PUNTERO APUNTANDO AL NODO RECIEN ANADIDO.
			memcpy(&(node->data->route), route, sizeof(rip_route_t));
			node->next = table;
			new_table = node;
				
			// Calcula el timeout.Si la metrica es 16 lo pone a 120, si no a 180.
			int timeout = route->metric == 16? DEL_TIME : INF_TIME;
			timeout *= 1000; // Convierte a ms	
				
			// Inicia el temporizador	
			timerms_reset(&(node->data->timer), timeout);
		}		
	}
	
	//Devuelve new_table, como la tabla con la ruta que acaba de añadirse.
	return new_table;
}


/* rip_route_table_t * rip_route_table_remove ( rip_route_table_t * table, 
 *							rip_route_t *route )
 *
 * DESCRIPCIÓN:
 *   Esta función borra la ruta almacenada en la tabla de rutas especificada.
 *
 * PARÁMETROS:
 *   'table': Tabla de rutas de la que se desea borrar una ruta.
 *   'route': Ruta a eliminar de la tabla.
 * 
 * VALOR DEVUELTO:
 *   Esta función devuelve la nueva tabla.
 *
 */
rip_route_table_t * rip_route_table_remove ( rip_route_table_t * table, 
							rip_route_t *route )
{
	// Puntero para guardar la nueva tabla
	rip_route_table_t *aux_table = table;

	// Caso especial en el que se borra el primer nodo de la tabla
	if (table != NULL && memcmp(&(table->data->route), route, 
		sizeof(rip_route_t)) == 0)
	{
		aux_table = table->next;
		rip_route_node_free(table);
		return aux_table;
	}

	// Busca la ruta en la tabla y si la encuentra la borra y actualiza la tabla
	while (table->next != NULL) 
	{
		if (memcmp(&(table->next->data->route), route, sizeof(rip_route_t)) == 0)
		{
			// Se guarda el nodo a borrar
			route_node *aux = table->next;
			
			// Lo elimina de la tabla y lo libera
			table->next = table->next->next;
			rip_route_node_free(aux);	
			
			return aux_table;
		}
		
		table = table->next;
	}

	return aux_table;
}


/* route_data * rip_route_table_get ( rip_route_table_t * table, int index );
 * 
 * DESCRIPCIÓN:
 *   Esta función devuelve la estructura "data" almacenada en la posición de la
 *   tabla de rutas especificada.
 *
 * PARÁMETROS:
 *   'table': Tabla de rutas de la que se desea obtener una ruta.
 *   'index': Índice de la ruta consultada
 * 
 * VALOR DEVUELTO:
 *   Esta función devuelve la estructura "data" almacenada en la posición de la
 *   tabla de rutas especificada.
 *
 * ERRORES:
 *   Esta función devuelve 'NULL' si no ha sido posible consultar la tabla de
 *   rutas, o no existe ninguna ruta en dicha posición.
 */
route_data * rip_route_table_get ( rip_route_table_t * table, int index )
{
	// Comprueba limites de la tabla
	if(index < 0 || index > rip_route_table_size(table) )
		return NULL;
	
	// Puntero para guardar el "data"
	route_data * data = NULL;

	if (table != NULL)
	{
		// Recorre toda la tabla hasta llegar a la posición "index"
		while (index != 0)
		{
			table = table->next;
			index--;
		}
		data = table->data;
	}
	
	return data;
}


/* int rip_route_table_find ( rip_route_table_t * table, ipv4_addr_t subnet, 
 * 							ipv4_addr_t mask)
 *
 * DESCRIPCIÓN:
 *   Esta función devuelve el índice de la ruta para llegar a la subred
 *   especificada.
 *
 * PARÁMETROS:
 *   'subnet': Dirección IP de la subred a buscar.
 *     'mask': Máscara de la subred destino a buscar.
 * 
 * VALOR DEVUELTO:
 *   Esta función devuelve la posición de la tabla de rutas donde se encuentra
 *   la ruta que apunta a la subred especificada.
 *
 * ERRORES:
 *   La función devuelve '-1' si no se ha encontrado la ruta especificada o
 *   '-2' si no ha sido posible realizar la búsqueda.
 */
int rip_route_table_find ( rip_route_table_t * table, ipv4_addr_t subnet, 
						   ipv4_addr_t mask)
{
	// Variable donde se guardara el indice donde se encuentra la ruta
	int route_index = -1;
	
	// Variable de iteracion del bucle
	int i = 0;
	while (table != NULL)
	{
		// Captura la ruta en la posicion i de la tabla
		rip_route_t * route_i = &(table->data->route);
		
		if (route_i != NULL)
		{
			// Comprueba si se trata de la misma ruta(subred y mascara)
			int same_subnet = 
			(memcmp(route_i->ip_addr, subnet, IPv4_ADDR_SIZE) == 0);			
			int same_mask = (memcmp(route_i->mask, mask, IPv4_ADDR_SIZE) == 0);
			
			// Si es la misma ruta, guarda el indice y sale del bucle
			if (same_subnet && same_mask)
			{
				route_index = i;
				break;
			}
		}
		
		table = table->next;
		i++;
	}
	
	return route_index;
}

/* void rip_route_table_free ( rip_route_table_t * table );
 *
 * DESCRIPCIÓN:
 *   Esta función libera la memoria reservada para la tabla de rutas
 *   especificada, incluyendo todas las rutas almacenadas en la misma,
 *   mediante la función 'rip_route_free()'.
 *
 * PARÁMETROS:
 *   'table': Tabla de rutas a borrar.
 */
void rip_route_table_free ( rip_route_table_t * table )
{
	while (table != NULL)
	{
		rip_route_table_t *aux = table;
		table = table->next;
		rip_route_node_free(aux);
    }
}


/* void rip_route_table_output ( rip_route_table_t * table, FILE * out );
 *
 * DESCRIPCIÓN:
 *   Esta función imprime por la salida indicada la tabla de rutas RIP
 *   especificada.
 *
 * PARÁMETROS:
 *        'out': Salida por la que imprimir la tabla de rutas.
 *      'table': Tabla de rutas a imprimir.
 *
 * VALOR DEVUELTO:
 *   La función devuelve el número de rutas impresas por la salida indicada, o
 *   '0' si la tabla de rutas estaba vacia.
 *
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error al escribir por
 *   la salida indicada.
 */
int rip_route_table_output ( FILE * out, rip_route_table_t * table )
{
	int err = fprintf
		(out, "# SubnetAddr  \tSubnetMask    \tNextHop  \tMetric \tElapsedTime\n");
	if (err < 0) {
		return -1;
	}
	
	char subnet_str[IPv4_STR_MAX_LENGTH];
	char mask_str[IPv4_STR_MAX_LENGTH]; 
	char nexthop_str[IPv4_STR_MAX_LENGTH];

	while(table != NULL)
	{
		
		if (table->data != NULL)
		{
			ipv4_addr_str(table->data->route.ip_addr, subnet_str);
			ipv4_addr_str(table->data->route.mask, mask_str);
			ipv4_addr_str(table->data->route.next_hop, nexthop_str);
			uint32_t metric = table->data->route.metric;
			uint32_t time = timerms_elapsed(&(table->data->timer));

			err = fprintf(out, "%-15s\t%-15s\t%-15s\t%d\t%d\n",	subnet_str, 
						  mask_str, nexthop_str, metric, time/1000);
			if (err < 0)
			{
				return -1;
			}
		}

		table = table->next;
	}

	return 0;
}


/* void rip_route_table_print ( rip_route_table_t * table );
 *
 * DESCRIPCIÓN:
 *   Esta función imprime por la salida estándar la tabla de rutas RIP
 *   especificada.
 *
 * PARÁMETROS:
 *      'table': Tabla de rutas a imprimir.
 */
void rip_route_table_print ( rip_route_table_t * table )
{
	if (table != NULL)
	{
		rip_route_table_output (stdout, table);
	}
	else
	{
		printf("\nTabla vacia\n");
	}
}

void rip_route_node_free(route_node *node)
{
	if (node != NULL)
	{
		if (node->data != NULL) free(node->data);
			free(node);
	}
}

rip_route_table_t * rip_route_update(rip_route_table_t *table, 
									 rip_route_t *route, ipv4_addr_t from)
{
	//Comprueba que la ruta tenga parametros correctos, metrica entre 0-16
	if(route->metric < 1 || route->metric > 16)
	{
		return table;
	}
	
	// Comprueba si la ruta esta en la tabla
	int index = rip_route_table_find(table, route->ip_addr, route->mask);
	
	// Si encuentra la ruta la actualiza, si no la a�ade
	if (index >= 0)
	{
		// Captura el data de la tabla para trabajar con la ruta
		route_data *data = rip_route_table_get(table, index);
		
		// Si la informacion viene de nuestro next_hop, aunque la info diga que
		// tiene mas metrica hay que actualizarlo.
		if(memcmp(&(data->route.next_hop), from, IPv4_ADDR_SIZE) == 0)
		{
			data->route.metric = route->metric;
		    data->route.metric += HOP_METRIC;
		}
		// Si la metrica de la ruta de la tabla es mayor que la de la recibida
		// actualiza todo
		else if (data->route.metric > route->metric) 
		{
			memcpy(&(data->route), route, sizeof(struct rip_entry));
			memcpy(&(data->route.next_hop), from, sizeof(ipv4_addr_t));
			
			data->route.metric = route->metric;
		    data->route.metric += HOP_METRIC;
		}
		
		// Si se produce un cambio ponemos el flag a 1 para indicarlo, volvera
		// al cero con un trigger update o con un update periodico.
		data->flag = 1;
		
		// Calcula el timeout. Si la metrica es 16 lo pone a 120, si no a 180.
		int timeout = data->route.metric == 16? DEL_TIME : INF_TIME;
		timeout *= 1000; // Convierte a ms
		
		// Actualiza el timer
		timerms_reset(&(data->timer), timeout);
		
	}
	else if (index == -1) // A�ade la ruta
	{
		memcpy(route->next_hop, from, sizeof(ipv4_addr_t));
		
		// Si la métrica es infinita, sigue infinita, si no, añade 1
		if (route->metric < 16)
			route->metric += HOP_METRIC;
		
		table = rip_route_table_add(table, route);
	}
	
	return table;
}

int rip_route_table_size(rip_route_table_t *table)
{
	int size = 0;
	
	while (table != NULL)
	{
		table = table->next;
		size++;
	}
	
	return size;
}

long int get_min_time(rip_route_table_t *table)
{
	// Variable para guardar el tiempo menor. Se inicializa el valor maximo que
	// puede tomar el temporizador.
	long int min = INF_TIME*1000;
	
	// Variable auxiliar para obtener el tiempo restante de los temporizadores
	long int timeleft = 0;
	
	// Recorre la tabla
	while (table != NULL)
	{
		// Captura el tiempo restante del temporizador
		timeleft = timerms_left(&(table->data->timer));
		
		// Si es menor que el tiempo minimo, lo guarda
		if(timeleft < min) min = timeleft;
		
		table = table->next;
	}
	
	return min;
}

rip_route_table_t *update_timers(rip_route_table_t *table)
{
	puts("#DEBUG#Actualizando temporizadores\n");

	// Puntero auxiliar para recorrer la tabla
	rip_route_table_t *aux_table = table;
	
	// Recorre la tabla
	while (aux_table != NULL)
	{
		// Captura el data
		route_data *data = aux_table->data;
		
		// Comprueba si ha expirado el temporizador
		if (timerms_left(&(data->timer)) == 0)
		{
			// Si ha expirado y la metrica es 16 ya ha habido un timeout, luego
			// el temporizador actual era el de garbage collection; se elimina
			// la ruta
			if (data->route.metric == 16)
			{
				table = rip_route_table_remove(table, &(data->route));
				//return table;
			}
			// Si la metrica no es 16 es un timeout. La pone a 16 y pone a 
			// correr el nuevo temporizador
			else 
			{
				data->route.metric = 16;
				timerms_reset(&(data->timer), DEL_TIME*1000);
				//return table;
			}
		}
		else
		{
			aux_table = aux_table->next;
		}
	}
	
	return table;
}

int has_changes(rip_route_table_t *table)
{	
	while (table != NULL)
	{
		if (table->data->flag == 1) 
			return 1;
		
		table = table->next;
	}
		
	return 0;	
}

int is_valid_route(ipv4_addr_t subnet, ipv4_addr_t mask, uint32_t metric)
{
	// Variable auxiliar para comprobar si la ruta es valida
	int valid = 1;
    printf("#DEBUG#is_valid_route()-> metric: =%d\n",metric);
	
	// Comprueba que la metrica este dentro de los limites
	if (metric >= 1 && metric <= 16) 
	{
		// Comprueba campo a campo de la subred y de la mascara que sea una
		// direccion IPv4 valida
		int i;
		for (i = 0; i < 4; i++)
		{
			unsigned int subnet_i = ((unsigned char *)subnet)[i];
			unsigned int mask_i = ((unsigned char *)mask)[i];
            
			// Si es valida continua, si no, actualiza la variable de control
			// y sale del bucle
			if (subnet_i >= 0 && subnet_i <= 255 && mask_i >= 0 && mask_i <= 255)
				continue;
			else
			{
				valid = 0;
				break;
			}
		}
	}
	else 
		valid = 0;
	
	return valid;
}

void rip_route_hton(rip_route_t *route)
{
	route->famid = htons(route->famid);
	route->route_tag = htons(route->route_tag);
	
	uint32_t ip_addr = htonl(ipv4_addr_bin(route->ip_addr));	
	memcpy(route->ip_addr, &ip_addr, IPv4_ADDR_SIZE);
	
	uint32_t mask = htonl(ipv4_addr_bin(route->mask));	
	memcpy(route->mask, &mask, IPv4_ADDR_SIZE);
	
	uint32_t next_hop = htonl(ipv4_addr_bin(route->next_hop));	
	memcpy(route->next_hop, &next_hop, IPv4_ADDR_SIZE);
	
	route->metric = htonl(route->metric);
}

void rip_route_ntoh(rip_route_t *route)
{		
	route->famid = ntohs(route->famid);
	route->route_tag = ntohs(route->route_tag);
	
	uint32_t ip_addr = ntohl(ipv4_addr_bin(route->ip_addr));	
	memcpy(route->ip_addr, &ip_addr, IPv4_ADDR_SIZE);
	
	uint32_t mask = ntohl(ipv4_addr_bin(route->mask));	
	memcpy(route->mask, &mask, IPv4_ADDR_SIZE);
	
	uint32_t next_hop = ntohl(ipv4_addr_bin(route->next_hop));	
	memcpy(route->next_hop, &next_hop, IPv4_ADDR_SIZE);
	
	route->metric = ntohl(route->metric);
}
