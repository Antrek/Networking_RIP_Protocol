#include "rip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 





/*
El servidor RIPv2 recibe mensajes Request enviados por el cliente y responde
con mensajes Response adecuado al tipo de consulta realizado. Adem�s recibe y
procesa mensajes Response generados por otros demonios RIP est�ndar, y a partir
de los mismos genera y mantiene actualizada una tabla de rutas de acuerdo a las
reglas del susodicho protocolo de encaminamiento de vector distancia RIPv2. Para
ello almacena en dicha tabla de rutas la ruta �ptima en cada momento (indicando
la m�trica de dicha ruta) para llegar a cualquiera los destinos aprendidos, as�
como borrar las entradas que no han sido actualizadas recientemente o tienen una
m�trica infinita. Por cada mensaje Response procesado que genere alg�n cambio en
la tabla de ruta imprime el estado final de la misma una vez aplicados todos los
cambios.
*/
int main()
{    
    // Null entry para peticiones de toda la tabla.
    rip_route_t null_entry;
    
    memset(&null_entry,0,RIP_ROUTE_LEN);
    null_entry.metric = 16;
    
    rip_open();
    
    rip_route_table_t *rip_table = NULL;
    unsigned char op_code = 0;
    ipv4_addr_t ip_src;
    udp_port_t port_dst;
    udp_bin_port(port_dst, 520);
    udp_port_t port_src;
    unsigned char buffer[RIP_MAX_SIZE];
	int bytes_sent = 0;
    int bytes_recv = 0;
	int timeout = INF_TIME*1000;
	
	timerms_t update_timer;
	timerms_t trigger_timer;
	int trigger_flag = 0;

	// Peticion de rutas por multicast al inicio
	while (bytes_sent <= 0)
	{
		bytes_sent = rip_send(RIP_MCAST_ADDR, port_dst, 
					(unsigned char *)&null_entry, RIP_ROUTE_LEN, RIP_REQUEST);
		if (bytes_sent < 0)
		{
			fprintf(stderr, "rip_recv(): ERROR en rip_send()\n");
			return -1;
		}
	
	}

	// Inicializa el temporizador de updates periodicos
	update_timer_reset(&update_timer);  

    
    //int rip_recv(udp_port_t src, udp_port_t dst, ipv4_addr_t from, unsigned char *buffer, long int timeout)
    while (1)
    {
		// Calculamos timeouts de updates periodicos y trigger updates
		int up_timeout = timerms_left(&update_timer);
		int tr_timeout = timerms_left(&trigger_timer);
		

		// Miramos primero el timeout del update periodico antes del triggered
		// para evitar un doble envio
		if (up_timeout == 0)
		{
			int table_size = rip_route_table_size(rip_table);
			if (table_size > 0)
			{
				rip_route_t send_routes[table_size];
				int num_routes = rip_table_get_routes(rip_table, send_routes, 0);				
				table_size = num_routes*sizeof(rip_route_t);
							
				// Update periodico manda por multicast
				bytes_sent = rip_send(RIP_MCAST_ADDR, port_dst, (unsigned char *)send_routes,
								table_size, RIP_RESPONSE);
				if (bytes_sent < 0)
                {
					fprintf(stderr, "rip_server(): ERROR en rip_send()\n");
                    return -1;
               	}
			}
			
			//Limpia flags y resetea el timer
			clean_flags(rip_table);
			trigger_flag = 0;
			update_timer_reset(&update_timer);  
		
		}
	
		// Mira si estaba activo el timer de trigger y si ha expirado
		if (tr_timeout == 0 && trigger_flag)
		{
			int table_size = rip_route_table_size(rip_table);
			if (table_size > 0)
			{
				rip_route_t send_routes[table_size];
				int num_routes = rip_table_get_routes(rip_table, send_routes, 0);
				
				if (num_routes > 0)
				{			
					table_size = num_routes*sizeof(rip_route_t);
					
					unsigned char routes[num_routes];
					memcpy(routes, send_routes, table_size);
					
					// manda updates por multicast
					bytes_sent = rip_send(RIP_MCAST_ADDR, port_dst, routes,
									table_size, RIP_RESPONSE);
					if (bytes_sent < 0)
					{
						fprintf(stderr, "rip_recv(): ERROR en rip_send()\n");
						return -1;
					}		
				}           
			}
			
			// limpia flags
			clean_flags(rip_table);
			trigger_flag = 0;
		}

		// Calculamos timeouts de rutas y nos quedamos con el menor
		timeout = get_min_time(rip_table);
		
		if (up_timeout < timeout)
			timeout = up_timeout;
		if (tr_timeout < timeout && trigger_flag)
			timeout = tr_timeout;
    
		// Si el timeout es 0 en este punto significa que no hay rutas en la 
		// tabla, y que get_min_time es 0. Luego, el siguiente evento de timer
		// sera obligatoriamente un update periodico.
		if (timeout == 0)
			timeout = up_timeout;

		// Recibimos con el menor de los timeouts para poder capturar el evento
        bytes_recv = rip_recv(port_src, port_dst, ip_src, buffer, &op_code, timeout);
			
        if (bytes_recv > 0)
        {
            // Crea estructura para las entradas recibidas
            rip_route_t rip_entrys[(bytes_recv)/sizeof(rip_route_t)];
            int n_entries = (sizeof(rip_entrys)/sizeof(struct rip_entry));
            
            memcpy(rip_entrys,buffer,bytes_recv);
			
            if (op_code == RIP_REQUEST)
            {   
                // Envia la tabla completa si recibimos una unica entrada con todo a 0 y metrica 16
                if ((sizeof(rip_entrys) == sizeof(rip_route_t)) && (memcmp(rip_entrys, &null_entry, sizeof(rip_route_t)) == 0))
                {

                    int table_size = rip_route_table_size(rip_table);
					
					// Calcula el numero de rutas a enviar (no envia rutas a 16)
					route_node *aux = rip_table;
					while (aux != NULL)
					{
						if (aux->data->route.metric == 16)
							table_size--;
						aux = aux->next;	
					}			
					
					// Calcula el numero de datagramas a enviar
                    int num_dgm = table_size <= 25 ? 1 : table_size/25 + 1;
                    rip_route_table_t *send_table = rip_table;

					printf("Tamaño de tabla: %d, Numero de datagramas: %d\n", table_size, num_dgm);
                    unsigned char *send_routes = NULL;
                    int i = 1;
                    while (i <= num_dgm)
                    {
                        int j = 0;
                        int num_routes;
                        
						// si es el ultimo datagrama calcula el numero de rutas
                        if (i == num_dgm)
                        {
                            num_routes = table_size - (num_dgm - 1)*25;
                        }
                        else
                        {
                            num_routes = 25;
                        }
                        
                        // estructura para enviar en el datagrama
                        rip_route_t routes[num_routes];
						
                        while (j < num_routes)
                        {
							// si la metrica es 16 no la envia
							if (routes[j].metric == 16)
								continue;
							
                            memcpy(&routes[j], &(send_table->data->route), sizeof(struct rip_entry));
                            send_table = send_table->next;
                            j++;
                        }
                        
                        send_routes = (unsigned char *)routes;

						// enviamos el response						
                        bytes_sent = rip_send(ip_src, port_src, send_routes, num_routes*sizeof(rip_route_t), RIP_RESPONSE);
                        if (bytes_sent < 0)
                        {
                            fprintf(stderr, "rip_recv(): ERROR en rip_send()\n");
                            return -1;
                        }
                        
                        i++;
                    }
                }else // si no nos llega una peticion de toda la tabla procesamos peticiones individuales
                {                   
		
                    int i;
                    int index;
                    for (i=0;i<n_entries;i++)
                    {				
						// busca la ruta en la tabla
                        index = rip_route_table_find (rip_table, rip_entrys[i].ip_addr, rip_entrys[i].mask);
                        if (index >= 0) // si esta en la tabla le guarda nuestra metrica
                        {
                            route_data *data;
                            data = rip_route_table_get(rip_table, index);
                            memcpy(&(rip_entrys[i].metric), &(data->route.metric),(sizeof(int)));
                        }
                        else // si no la pone a 16
                            rip_entrys[i].metric = 16;
                    }
                 
					// envia el response
                    bytes_sent = rip_send(ip_src, port_src, (unsigned char *)rip_entrys, sizeof(rip_entrys), RIP_RESPONSE);
                    if (bytes_sent < 0)
                    {
                        fprintf(stderr, "rip_recv(): ERROR en rip_send()\n");
                        return -1;
                    }
                }
                
            }else if (op_code == RIP_RESPONSE)
            {
             
                int f;
                for(f = 0; f < n_entries; f++)
				{
					// actualiza la tabla con las entradas recibidas
					rip_table = rip_route_update(rip_table, (rip_route_t *)(&rip_entrys[f]), ip_src);
                }
                
                // si hay cambios la imprime y pone en marcha el triggered updates
                if (has_changes(rip_table))
				{
					rip_route_table_print(rip_table);
					
					// Si hubieron cambios lanzamos un triggered update
					if (trigger_flag == 0)
					{
						trigger_flag = 1;
						trigger_timer_reset(&trigger_timer);						
					}

				}
            }
        }
        else
        {
			// si no recibe nada, ha ocurrido un evento por temporizador
            if (bytes_recv == 0)
            {
                // actualiza los timers de las rutas
                rip_table = update_timers(rip_table);
                
				// si hay cambios imprime
                if (has_changes(rip_table))
				{
					rip_route_table_print(rip_table);
					
					// Si hubieron cambios lanzamos un triggered update
					if (trigger_flag == 0)
					{
						trigger_flag = 1;
						trigger_timer_reset(&trigger_timer);						
					}
				}
            }
            else
            {
                fprintf(stderr,"main(): ERROR in rip_recv()\n");
                exit(-1);
            }
        }
    }

}
