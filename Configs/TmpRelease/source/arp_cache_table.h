#ifndef _ARP_CACHE_TABLE_H
#define _ARP_CACHE_TABLE_H

#include "arp.h"
#include "ipv4.h"

#include <timerms.h>

#define ARP_MAX_TABLE_SIZE 100
#define ARP_TIMEOUT 60
#define ARP_MAX_TIMEOUT 180

/* Definicion de la estructura de entrada en tabla arp */
typedef struct arp_entry 
{
	ipv4_addr_t ip_addr;
	mac_addr_t mac_addr;
	char iface[6];
	timerms_t timer;
} arp_entry_t;

extern arp_entry_t arp_table[ARP_MAX_TABLE_SIZE];

/* void arp_new_table()
* 
* DESCRIPCIÓN:
*  Crea una nueva tabla arp.
*
* PARÁMETROS:
*   
* VALOR DEVUELTO:
*   No devuelve nada.
*/
void arp_new_table();

/* int arp_table_add(ipv4_addr_t ip_addr, mac_addr_t mac_addr, char *iface)
* 
* DESCRIPCIÓN:
*  Añade una entrada a la tabla arp.
*
* PARÁMETROS:
* ipv4_addr_t ip_addr - Direccion ip destino
* mac_addr_t mac_addr - Mac correspondiente a la ip destino
* char *iface         - Interfaz correspondiente.
*
* VALOR DEVUELTO:
*   Devuelve la posicion en la que ha incluido la ruta.
*/
int arp_table_add(ipv4_addr_t ip_addr, mac_addr_t mac_addr, char *iface);

/* int arp_table_remove(ipv4_addr_t ip_addr)
* 
* DESCRIPCIÓN:
*  Borra un elemento de la tabla arp.
*
* PARÁMETROS:
* ipv4_addr_t ip_addr - Direccion ip destino.
*
* VALOR DEVUELTO:
*   Devuelve 0 si la ha borrado
*    Devuelve -1 si no ha podido..
*/
int arp_table_remove(ipv4_addr_t ip_addr);

/* int arp_table_update(ipv4_addr_t ip_addr, mac_addr_t mac_addr, char *iface)
* 
* DESCRIPCIÓN:
*  Actualiza o añade la entrada de la tabla arp con los parametros proporcionados..
*
* PARÁMETROS:
* ipv4_addr_t ip_addr - Direccion ip destino
* mac_addr_t mac_addr - Mac correspondiente a la ip destino
* char *iface         - Interfaz correspondiente.
*
* VALOR DEVUELTO:
*   Devuelve 0 si ha actualizado la tabla
*   Devuelve el numero de la posicion en la que ha añadido la tabla..
*/
int arp_table_update(ipv4_addr_t ip_addr, mac_addr_t mac_addr, char *iface);

/* arp_entry_t *arp_table_search(ipv4_addr_t ip_addr)
* 
* DESCRIPCIÓN:
*  Busca una entrada asociada a la ip indicada.
*
* PARÁMETROS:
* ipv4_addr_t ip_addr - Direccion ip destino
*
* VALOR DEVUELTO:
*   arp_entry_t * - Puntero a la entrada en contrada.
*   Null si no existe ninguna entrada para esa ip.
*/
arp_entry_t *arp_table_search(ipv4_addr_t ip_addr);

/* int arp_table_size()
* 
* DESCRIPCIÓN:
*  Calcula el numero de entradas de la tabla.
*
* PARÁMETROS:
* No tiene parametros.
*
* VALOR DEVUELTO:
* Devuelve el numero de entradas de la tabla.
*/
int arp_table_size();

/* void arp_table_timeout()
* 
* DESCRIPCIÓN:
* Comprueba las entradas de la tabla que han expirado y las elimina.
*
* PARÁMETROS:
* No tiene parametros.
*
* VALOR DEVUELTO:
* No devuelve nada.
*/
void arp_table_timeout();

/*
* int arp_get_empty_position()
* 
* DESCRIPCIÓN:
* Busca la primera posicion vacia.
*
* PARÁMETROS:
* No tiene parametros.
*
* VALOR DEVUELTO:
* -1 si no ha encontrado hueco. El primer hueco libre.
*/
int arp_get_empty_position();

/* int arp_is_empty(arp_entry_t *entry)
* 
* DESCRIPCIÓN:
* Comprueba si la tabla esta vacia.
*
* PARÁMETROS:
* arp_entry_t *entry Principio de la tabla arp.
*
* VALOR DEVUELTO:
* -1 si no ha encontrado hueco. El primer hueco libre.
*/
int arp_is_empty(arp_entry_t *entry);

/* void arp_entry_print(arp_entry_t entry)
* 
* DESCRIPCIÓN:
*  Imprime la ruta marcada
*
* PARÁMETROS:
* arp_entry_t *entry Imprime una entrada tabla arp.   
*
* VALOR DEVUELTO:
* No devuelve nada.
*/
void arp_table_print();

#endif /* _ARP_CACHE_TABLE_H */
