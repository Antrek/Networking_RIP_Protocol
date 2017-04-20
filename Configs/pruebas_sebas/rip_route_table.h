#ifndef _RIP_ROUTE_TABLE_H
#define _RIP_ROUTE_TABLE_H

#include "rip.h"

#include <stdio.h>
#include <timerms.h>

#define RIP_ROUTE_TABLE_SIZE 25

/* Esta estructura almacena la información básica sobre la ruta a una subred. 
 * Incluye la dirección y máscara de la subred destino, el nombre del interfaz
 * de salida, y la dirección IP del siguiente salto.
 *
 * Utilice los métodos 'rip_route_create()' e 'rip_route_free()' para crear
 * y liberar esta estrucutra. Adicionalmente debe completar la implementación
 * del método 'rip_route_lookup()'.
 * 
 * Probablemente para construir una tabla de rutas de un protocolo de
 * encaminamiento sea necesario añadir más campos a esta estructura, así como
 * modificar las funciones asociadas.
 */
typedef struct rip_entry
{
	uint16_t famid;
	uint16_t route_tag;
	ipv4_addr_t ip_addr;
	ipv4_addr_t mask;
	ipv4_addr_t next_hop;
	uint32_t metric;
} rip_route_t;

typedef struct route_data
{
	rip_route_t route;
	timerms_t timer;
	unsigned char flag;
	
} route_data;

typedef struct rip_route_node {
  route_data *data;
  struct rip_route_node *next;
}route_node;

typedef struct rip_route_node rip_route_table_t;


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
( ipv4_addr_t subnet, ipv4_addr_t mask, ipv4_addr_t next_hop, uint32_t metric);


/* int rip_route_lookup ( rip_route_t * route, rip_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función indica si la dirección RIP especificada pertence a la
 *   subred indicada. En ese caso devuelve la longitud de la máscara de la
 *   subred.
 *
 *   Esta función NO está implementada, debe implementarla usted para que
 *   funcione correctamente la función 'rip_route_table_lookup()'.
 * 
 * PARÁMETROS:
 *   'route': Ruta a la subred que se quiere comprobar.
 *    'addr': Dirección RIP destino.
 *
 * VALOR DEVUELTO:
 *   Si la dirección RIP pertenece a la subred de la ruta especificada, debe
 *   devolver un número positivo que indica la longitud del prefijo de
 *   subred. Esto es, el número de bits a uno de la máscara de subred.
 *   La función devuelve '-1' si la dirección RIP no pertenece a la subred
 *   apuntada por la ruta especificada.
 */
// int rip_route_lookup ( rip_route_t * route, rip_addr_t addr );


/* void rip_route_print ( rip_route_t * route );
 *
 * DESCRIPCIÓN:
 *   Esta función imprime la ruta especificada por la salida estándar.
 *
 * PARÁMETROS:
 *   'route': Ruta que se desea imprimir.
 */
void rip_route_print ( rip_route_t * route );


/* void rip_route_free ( rip_route_t * route );
 *
 * DESCRIPCIÓN:
 *   Esta función libera la memoria reservada para la ruta especificada, que
 *   ha sido creada con 'rip_route_create()'.
 *
 * PARÁMETROS:
 *   'route': Ruta que se desea liberar.
 */
// void rip_route_free ( rip_route_t * route );



/* Número de entradas máximo de la tabla de rutas RIP */


/* Definción de la estructura opaca que modela una tabla de rutas RIP.
 * Las entradas de la tabla de rutas están indexadas, y dicho índice puede
 * tener un valor entre 0 y 'RIP_ROUTE_TABLE_SIZE - 1'.
 *
 * Esta estructura nunca debe crearse directamente. En su lugar debe emplear
 * las funciones 'rip_route_table_create()' e 'rip_route_table_free()' para
 * crear y liberar dicha estructura, respectivamente.
 *
 * Una vez creada la tabla de rutas, utilice 'rip_route_table_get()' para
 * acceder a la ruta en una posición determinada. Además es posible añadir
 * ['rip_route_table_add()'] y borrar rutas ['rip_route_table_remove()'],
 * así como buscar una subred en particular ['rip_route_table_find()']. 
 * 'rip_route_table_lookup()' es la función más importante de la tabla de
 * rutas ya que devuelve la ruta para llegar a la dirección RIP destino
 * esecificada.
 * 
 * Adicionalmente, las funciones 'rip_route_table_read()',
 * 'rip_route_table_write()' y 'rip_route_table_print()' permiten,
 * respectivamente, leer/escribir la tabla de rutas de/a un fichero, e
 * imprimirla por la salida estándar.
 */



/* rip_route_table_t * rip_route_table_create();
 * 
 * DESCRIPCIÓN: 
 *   Esta función crea una tabla de rutas RIP vacía.
 *
 *   Esta función reserva memoria para la tabla de rutas creada, para
 *   liberarla es necesario llamar a la función 'rip_route_table_free()'.
 *
 * VALOR DEVUELTO:
 *   La función devuelve un puntero a la tabla de rutas creada.
 *
 * ERRORES:
 *   La función devuelve 'NULL' si no ha sido posible reservar memoria para
 *   crear la tabla de rutas.
 */
//rip_route_table_t * rip_route_table_create();


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
rip_route_table_t *rip_route_table_add ( rip_route_table_t * table, rip_route_t * route );

/* rip_route_t * rip_route_table_remove ( rip_route_table_t * table, 
 *                                          int index );
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
rip_route_table_t * rip_route_table_remove ( rip_route_table_t * table, rip_route_t *route );


/* route_data * rip_route_table_get ( rip_route_table_t * table, int index );
 * 
 * DESCRIPCIÓN:
 *   Esta función devuelve la ruta almacenada en la posición de la tabla de
 *   rutas especificada.
 *
 * PARÁMETROS:
 *   'table': Tabla de rutas de la que se desea obtener una ruta.
 *   'index': Índice de la ruta consultada
 * 
 * VALOR DEVUELTO:
 *   Esta función devuelve la ruta almacenada en la posición de la tabla de
 *   rutas indicada.
 *
 * ERRORES:
 *   Esta función devuelve 'NULL' si no ha sido posible consultar la tabla de
 *   rutas, o no existe ninguna ruta en dicha posición.
 */
route_data *rip_route_table_get ( rip_route_table_t * table, int index );


/* int rip_route_table_find ( rip_route_table_t * table, ipv4_addr_t subnet, 
 * 							ipv4_addr_t mask, ipv4_addr_t next_hop )
 *
 * DESCRIPCIÓN:
 *   Esta función devuelve el índice de la ruta para llegar a la subred
 *   especificada.
 *
 * PARÁMETROS:
 *   'subnet': Dirección IP de la subred a buscar.
 *     'mask': Máscara de la subred destino a buscar.
 * 'next_hop': Dirección IP del encaminador empleado para llegar a la subred
 *             destino a buscar
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
						   ipv4_addr_t mask);

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
void rip_route_table_free ( rip_route_table_t * table );

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
int rip_route_table_output ( FILE * out, rip_route_table_t * table );

/* void rip_route_table_print ( rip_route_table_t * table );
 *
 * DESCRIPCIÓN:
 *   Esta función imprime por la salida estándar la tabla de rutas RIP
 *   especificada.
 *
 * PARÁMETROS:
 *      'table': Tabla de rutas a imprimir.
 */
void rip_route_table_print ( rip_route_table_t * table );

/*Compara ruta a ruta y actualiza los timers */
rip_route_table_t * rip_route_update(rip_route_table_t *table, rip_route_t *route, ipv4_addr_t from);

void rip_route_node_free(route_node *node);
//void rip_data_free(route_data *data);

int rip_route_table_size(rip_route_table_t *table);

long int get_min_time(rip_route_table_t *table);

rip_route_table_t *update_timers(rip_route_table_t *table);

int has_changes(rip_route_table_t *table);

int is_valid_route(ipv4_addr_t subnet, ipv4_addr_t mask, uint32_t metric);

void rip_route_hton(rip_route_t *route);

void rip_route_ntoh(rip_route_t *route);

#endif /* _RIP_ROUTE_TABLE_H */
