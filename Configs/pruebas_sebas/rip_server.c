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
    /*typedef struct rip_entry
    {
        uint16_t famid;
        uint16_t route_tag;
        ipv4_addr_t ip_addr; 
        ipv4_addr_t mask;
        ipv4_addr_t next_hop; 
        uint32_t metric;
    } rip_route_t;*/
    
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
    int bytes_recv = 0;
	int timeout = INF_TIME*1000;

    
    //int rip_recv(udp_port_t src, udp_port_t dst, ipv4_addr_t from, unsigned char *buffer, long int timeout)
    while (1)
    {
		if ((timeout = get_min_time(rip_table)) == 0)
			timeout = INF_TIME*1000;
        bytes_recv = rip_recv(port_src, port_dst, ip_src, buffer, &op_code, timeout);
        //Mensajes de depuracion
        printf("#DEBUG#RIP_serv/client: La informacion aqui mostrada puede ser erronea solo sirve para depurar\n");
        printf("#DEBUG#Bytes recibidos:%d\n",bytes_recv);
        char ip_src_char[IPv4_STR_MAX_LENGTH];
        ipv4_addr_str(ip_src, ip_src_char);
        printf("#DEBUG#Origen: IP:%s,Puerto:%d\n",ip_src_char, udp_port_bin(port_src));
        printf("#DEBUG#Destino: IP:La ip del equipo...,Puerto:%d\n",udp_port_bin(port_dst));    
        printf("#DEBUG#Command:%d, Num_entrys:%d \n",op_code, (int)(bytes_recv/sizeof(rip_route_t)));
        //Mensajes de depuracion
        
        
        
        
        if (bytes_recv > 0)
        {
            //procesa el mensaje y env�a lo que sea. si hay cambios imprime
            
            rip_route_t rip_entrys[(bytes_recv)/sizeof(rip_route_t)];
            int n_entries = (sizeof(rip_entrys)/sizeof(struct rip_entry));
            
            memcpy(rip_entrys,buffer,bytes_recv);
            puts("#DEBUG#rip_server()-Imprime Ruta\n");
            rip_route_print((rip_route_t *)buffer);
            
            if (op_code == RIP_REQUEST)
            {
                puts("#DEBUG#Server procesa Request\n");
                                
                rip_route_print(&rip_entrys[0]);
                rip_route_print(&null_entry);
                printf("#DEBUG#Comparacion %d\n",memcmp(rip_entrys, &null_entry, sizeof(rip_route_t)));
				printf("#DEBUG#Tamaños: %d (entradas), %d (ruta)\n",(int)sizeof(rip_entrys), (int)sizeof(rip_route_t));
                // Envia la tabla completa (FUNCION A PARTE???)
                if ((sizeof(rip_entrys) == sizeof(rip_route_t)) && (memcmp(rip_entrys, &null_entry, sizeof(rip_route_t)) == 0))
                {
                    puts("#DEBUG#Request de tipo 1.\n");
                    int table_size = rip_route_table_size(rip_table);
                    int num_dgm = table_size <= 25 ? 1 : table_size/25 + 1;
                    rip_route_table_t *send_table = rip_table;
                    
					printf("Tamaño de tabla: %d, Numero de datagramas: %d\n", table_size, num_dgm);
                    unsigned char *send_routes = NULL;
                    int i = 1;
                    while (i <= num_dgm)
                    {
                        int j = 0;
                        int num_routes;
                        
                        if (i == num_dgm)
                        {
                            num_routes = table_size - (num_dgm - 1)*25;
                        }
                        else
                        {
                            num_routes = 25;
                        }
                        rip_route_t routes[num_routes];
                        
                        while (j < num_routes)
                        {
                            memcpy(&routes[j], &(send_table->data->route), sizeof(struct rip_entry));
                            send_table = send_table->next;
                            j++;
                        }
                        
                        send_routes = (unsigned char *)routes;

						puts("#DEBUG#Enviando respuesta de tipo 1.\n");

                        int bytes_sent = rip_send(ip_src, port_src, send_routes, num_routes*sizeof(rip_route_t), RIP_RESPONSE);
                        if (bytes_sent < 0)
                        {
                            fprintf(stderr, "rip_recv(): ERROR en rip_send()\n");
                            return -1;
                        }
                        
                        i++;
                    }
                }else
                {
                    puts("#DEBUG#Request de tipo 2.\n");
					// Por aca da un seg fault, no se como lo vamos a probar pero hay que solucionarlo
					// puede ser por el rip_entrys[i] o por rip_table que esta a null
                    int i;
                    int index;
                    for (i=0;i<n_entries;i++)
                    {						
                        index = rip_route_table_find (rip_table, rip_entrys[i].ip_addr, rip_entrys[i].mask);
                        if (index < 0)
                        {
                            route_data *data;
                            data = rip_route_table_get(rip_table, index);
                            memcpy(&(rip_entrys[i].metric), &(data->route.metric),(sizeof(int)));
                        }
                        else
                            rip_entrys[i].metric = 16;
                    }
                    //int rip_send(ipv4_addr_t dst,udp_port_t port_dst, unsigned char* table, int len, int op_code) 
                    int bytes_sent = rip_send(ip_src, port_src, (unsigned char *)rip_entrys, sizeof(rip_entrys), RIP_RESPONSE);
                    if (bytes_sent < 0)
                    {
                        fprintf(stderr, "rip_recv(): ERROR en rip_send()\n");
                        return -1;
                    }
                }
                
            }else if (op_code == RIP_RESPONSE)
            {
                puts("#DEBUG#Response\n");
                int f;
                for(f = 0; f < n_entries; f++)
                {
					printf("#DEBUG#Metrica: %d\n",rip_entrys[f].metric);

                    rip_table = rip_route_update(rip_table, (rip_route_t *)(&rip_entrys[f]), ip_src);
                    //rip_table = rip_route_update(rip_table, rip_entrys[f], ip_src);
                }
                puts("#DEBUG#Imprimiendo tabla actualizada...\n");
                rip_route_table_output(stdout,rip_table);
                
                if (has_changes(rip_table)) rip_route_table_print(rip_table);
            }
        }
        else
        {
            if (bytes_recv == 0)
            {
                puts("#DEBUG#6\n");
                //Expira el timer, actualiza las rutas
                
                rip_table = update_timers(rip_table);
                
                if (has_changes(rip_table)) rip_route_table_print(rip_table);
            }
            else
            {
                fprintf(stderr,"main(): ERROR in rip_recv()\n");
                exit(-1);
            }
        }
    }
    //  rip_open();
    //  
    //  int bytes_recv = 0;
    //  ipv4_addr_t src;
    //  unsigned char buffer[RIP_MAX_SIZE];
    //  long int timeout = 3000;
    // 
    //  while ((bytes_recv = rip_recv(src, buffer, timeout) <= 0))
    //  {
        //      if (!bytes_recv) puts("Error ¬¬");
        //      else puts("Timeout v.vU");
        //  }   
        // 
        //  puts ("Llegó! Fuck yeah!");
        //  
        //  rip_close();
        //  
        //  return 0;
}
