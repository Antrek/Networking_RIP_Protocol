#include "arp_cache_table.h"

#include <stdio.h>
#include <string.h>

arp_entry_t arp_table[ARP_MAX_TABLE_SIZE];

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
void arp_new_table()
{
	memset(arp_table, 0, ARP_MAX_TABLE_SIZE*sizeof(arp_entry_t));
}

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
int arp_table_add(ipv4_addr_t ip_addr, mac_addr_t mac_addr, char *iface)
{

	int empty_pos = arp_get_empty_position();

	if (empty_pos >= 0)
	{
		memcpy(arp_table[empty_pos].ip_addr, ip_addr, IPv4_ADDR_SIZE);
		memcpy(arp_table[empty_pos].mac_addr, mac_addr, MAC_ADDR_SIZE);
		memcpy(arp_table[empty_pos].iface, iface, sizeof(iface));
		timerms_reset(&(arp_table[empty_pos].timer), ARP_TIMEOUT*1000);
	}
	else
	{
		arp_table_timeout();
		if (arp_get_empty_position() != -1)
		{
			arp_table_add(ip_addr, mac_addr, iface);
		}
	}

	return empty_pos;
}

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
int arp_table_remove(ipv4_addr_t ip_addr)
{

	arp_entry_t *entry = arp_table_search(ip_addr);
	
	if (entry != NULL)
	{
		memset(entry, 0, sizeof(arp_entry_t));
		return 0;
	}
		
	return -1;
}

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
int arp_table_update(ipv4_addr_t ip_addr, mac_addr_t mac_addr, char *iface)
{
	arp_entry_t *entry = arp_table_search(ip_addr);
	
	if (entry != NULL)
	{
		memcpy(entry->ip_addr, ip_addr, IPv4_ADDR_SIZE);
		memcpy(entry->mac_addr, mac_addr, MAC_ADDR_SIZE);
		memcpy(entry->iface, iface, sizeof(iface));
		timerms_reset(&(entry->timer), ARP_TIMEOUT*1000);
		return 0;
	}
	else
	{
		return arp_table_add(ip_addr, mac_addr, iface);
	}
}

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
arp_entry_t *arp_table_search(ipv4_addr_t ip_addr)
{
	int i;
	for (i = 0; i < ARP_MAX_TABLE_SIZE; i++)
	{
// 		char ip1[IPv4_STR_MAX_LENGTH];
// 		char ip2[IPv4_STR_MAX_LENGTH];
// 		
// 		ipv4_addr_str(arp_table[i].ip_addr, ip1);
// 		ipv4_addr_str(ip_addr, ip2);
// 		
// 		printf("###%s\t	%s\t\n", ip1, ip2);
		
		if (memcmp(arp_table[i].ip_addr, ip_addr, IPv4_ADDR_SIZE) == 0)
		{
			return &arp_table[i];
		}
	}
	
	return NULL;
}

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
int arp_table_size()
{
	int size = 0, i;
	for (i = 0; i < ARP_MAX_TABLE_SIZE; i++)
	{
		if (arp_is_empty(&arp_table[i]) == 0)
		{
			size++;
		}
	}
	
	return size;
}

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
void arp_table_timeout()
{
	int i;
	for (i = 0; i < ARP_MAX_TABLE_SIZE; i++)
	{
		if (arp_is_empty(&arp_table[i]) != 0)
		{
			if (timerms_left(&(arp_table[i].timer)) == 0)
			{
				arp_table_remove(arp_table[i].ip_addr);
			}			
		}
	}
}

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
int arp_get_empty_position()
{
	int i;
	for (i = 0; i < ARP_MAX_TABLE_SIZE; i++)
	{
		if (arp_is_empty(&arp_table[i]) == 0)
		{
			return i;
		}
	}
	
	return -1;
}

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
int arp_is_empty(arp_entry_t *entry)
{
	arp_entry_t null_entry;
	memset(&null_entry, 0, sizeof(arp_entry_t));
	
	return memcmp(entry, &null_entry, sizeof(arp_entry_t));

}

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
void arp_entry_print(arp_entry_t entry)
{
	char ip_addr[IPv4_STR_MAX_LENGTH];
	char mac_addr[MAC_STR_LENGTH];
	
	ipv4_addr_str(entry.ip_addr, ip_addr);
	mac_addr_str(entry.mac_addr, mac_addr);
	
	printf("%s\t	%s\t%s\t%d\n", ip_addr, mac_addr, entry.iface,
		   (int)(timerms_elapsed(&(entry.timer))/1000));
}

/* void arp_entry_print(arp_entry_t entry)
* 
* DESCRIPCIÓN:
*  Imprime la tabla completa de rutas
*
* PARÁMETROS:
* arp_entry_t *entry Principio de la tabla arp.   
*
* VALOR DEVUELTO:
* No devuelve nada.
*/
void arp_table_print()
{
	puts("IPv4 Address\t	MAC Address\t	Iface\tAge(seconds)");	
	int i;
	for (i = 0; i < ARP_MAX_TABLE_SIZE; i++)
	{
		if (arp_is_empty(&arp_table[i]) != 0)
		{
			arp_entry_print(arp_table[i]);
		}
	}
}


