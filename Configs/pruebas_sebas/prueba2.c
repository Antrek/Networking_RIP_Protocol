#include "rip.h"
#include <stdio.h>


#define NUM 11




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
		fprintf(stderr,"Bad input params: rip_client ip_addr\n");
		return 1;
	}
	
	ipv4_addr_t dst;
	ipv4_str_addr(argv[1], dst);
	
	rip_route_table_t *rip_table;

  
  rip_table = NULL;
  rip_route_t *ruta[25];
  
  ipv4_addr_t puta_ip_de_prueba;

  puta_ip_de_prueba[0] = 10;
  puta_ip_de_prueba[1] = 11;
  puta_ip_de_prueba[2] = 12;

  int i;
  for (i = 0; i < 22; i++)
  {
	ruta[i] = crea_ruta(192,168,1,i,192,168,1,i,5);
	rip_table = rip_route_update(rip_table,ruta[i],puta_ip_de_prueba);
	puta_ip_de_prueba[3] = (i+5);
	//sleep(1);
  }
 
  udp_port_t rip_port;
  udp_bin_port(rip_port, RIP_PORT);


  
//    for (i = 0; i < 25; i++)
//   {
// 	ruta[i] = crea_ruta(192,168,1,i,192,168,1,i+1,4);
// 	rip_table = rip_route_update(rip_table,ruta[i]);
//   }
  
	


  int num_bytes;
  int num_bytes2;

 if((num_bytes = rip_send(dst, rip_port, (unsigned char *) &(rip_table->data->route), RIP_ROUTE_LEN, RIP_REQUEST))>0)
	puts("====REQUEST====REQUEST===REQUEST=====\n=====SENT"); 

 if( (num_bytes2 = rip_send(dst, rip_port, (unsigned char *) ruta, sizeof(ruta), RIP_RESPONSE))>0)

  puts("====RESPONSE====RESPONSE=====RESPONSE \n =====SENT");
 
 printf("%d\t%d\n", num_bytes, num_bytes2);
 
  rip_route_table_output(stdout,rip_table);
  
  return 0; 
}
