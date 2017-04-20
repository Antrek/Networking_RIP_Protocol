#include "rip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#define NUM_ROUTES 5

rip_route_t *crea_ruta(int a,int b,int c, int d,int aa,int bb,int cc,int dd,int metrica)
{
  ipv4_addr_t ip;
  ip[0]= a;
  ip[1]= b;
  ip[2]= c;
  ip[3]= d;
  ipv4_addr_t mask;
  mask[0] = 255;
  mask[1] = 255;
  mask[2] = 255;
  mask[3] = 0;
  ipv4_addr_t n_hop;
  n_hop[0]= aa;
  n_hop[1]= bb;
  n_hop[2]= cc;
  n_hop[3]= dd;
  int metric = metrica;
  return rip_route_create(ip,mask,n_hop,metric);
}

int main(int argc, char *argv[])
{
	rip_open();
	
	if (argc != 2)
	{
		fprintf(stderr,"Bad input params: ./rip_client ip_addr\n");
		return 1;
	}
	
	ipv4_addr_t dst;
// 	char *dst_addr = "224.0.0.9";
	ipv4_str_addr(argv[1], dst);
	
	int num_bytes = 0;

	struct rip_entry rip_routes[NUM_ROUTES];
	
	int i;
	for (i = 0; i < NUM_ROUTES; i++)
	{
		rip_route_t *aux = crea_ruta(111,111,i*i,i+1,192,168,i,i+1,5);
		memcpy(&rip_routes[i], aux, RIP_ROUTE_LEN);
	}
	
	udp_port_t rip_port;
	udp_bin_port(rip_port, 520);
	
	do
	{
			num_bytes = rip_send(dst, rip_port, (unsigned char *)rip_routes, RIP_ROUTE_LEN*NUM_ROUTES, RIP_RESPONSE);
	}
	while (!num_bytes);
	
    if (num_bytes < 0) 
    {
        puts("main(): ERROR en rip_send()\n");
        exit(-1);
    }
	
	return 0;
}
