#include "ipv4.h"
#include "ipv4_config.h"
#include "ipv4_route_table.h"
#include "arp.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*
 struct ipv4_handler
    {
        ipv4_addr_t ipv4_src;
        ipv4_addr_t netmask;
        eth_iface_t *iface;
    };
*/
int main (int argc, char *argv[])
{   

   
    
    char ifname[5]; // Variable para guardar el nombre de la interfaz Ethernet
    
    struct ipv4_handler ipv4_handler;
    
    /* Lee el archivo de configuración IP */
    if (ipv4_config_read(IP_CONF, ifname, ipv4_handler.ipv4_src, ipv4_handler.netmask) == -1)
    {
        fprintf(stderr,"arp_server(): ERROR in ipv4_config_read()\n");
        exit(-1);
    }   
    
    /* Inicializa Ethernet */
    if((ipv4_handler.iface = eth_open (ifname)) == NULL)
    {
        fprintf(stderr, "arp_server(): ERROR en eth_open()\n");
        return -1;
    }
    
    
    int bytes_recv = 0;
    do
    {
        /* Recibe con un timeout de 3 segundos y comprueba el estado de la
        * recepción.
        */
        unsigned char arp_buffer[ETH_MTU];
        mac_addr_t mac_src;
        
        bytes_recv =  eth_recv(ipv4_handler.iface, mac_src, ARP_TYPE, (unsigned char *)arp_buffer, 3000);
        
        
        if (bytes_recv > 0)
        {
            arp_frame_t *arp_frame;
            
            memcpy(arp_frame, (struct arp_frame *)arp_buffer,sizeof(arp_buffer));
            if((arp_frame->opcode) == 1)
            {
                int send = 0;
                if(!memcmp(arp_frame->ip_dst, ipv4_handler.ipv4_src,IPv4_ADDR_SIZE))
                {
                    memcpy(arp_frame->mac_dst, (ipv4_handler.iface)->mac_address, MAC_ADDR_SIZE);
                    send = 1;
                }
                else 
                {
                    arp_entry_t *arp_entry;
                    arp_entry_t *null_arp_entry;
                    memset(null_arp_entry,0,sizeof(arp_entry_t));
                    
                    memcpy(arp_entry, arp_table_search(arp_frame->ip_dst),sizeof(arp_entry));
                    if(memcmp(arp_entry,null_arp_entry,sizeof(arp_entry)))
                    {
                        memcpy(arp_frame->mac_dst, arp_table_search(arp_frame->ip_dst)->mac_addr, MAC_ADDR_SIZE);
                        send = 1;
                    }
                }
                
                if(send)
                {
                    arp_frame->opcode = REPLY;
                    int err = eth_send(ipv4_handler.iface, arp_frame->mac_src, ARP_TYPE, (unsigned char*)arp_frame, sizeof(arp_frame));
                    if (err == -1) 
                    {
                        fprintf(stderr, "ERROR en eth_send()\n");
                        exit(-1);
                    }
                }
            }else if(arp_frame->opcode == REPLY)
            {
                arp_table_update(arp_frame->ip_dst, arp_frame->mac_dst, ipv4_handler.iface->mac_address);
            }
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
        
    }
    while (1);
    
    /* "Cierra" IPv4 */
    if (ipv4_close() == -1)
    {
        fprintf(stderr,"main(): ERROR in ipv4_close()\n");
        exit(-1);
    }
    
}
