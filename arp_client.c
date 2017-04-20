#include "arp.h"
#include <stdio.h>

int main (int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Bad usage: arp_client iface ipaddr\n");
		return (-1);
	}
	
	char *ifname = argv[1];
	
	ipv4_addr_t ipv4_dst;
	if (ipv4_str_addr (argv[2], ipv4_dst) == -1)
	{
		printf("Invalid IP address\n");
		return (-1);
	}	

	eth_iface_t *iface = eth_open(ifname);
	
	if (iface == NULL)
	{
		printf("Could not open interface %s\n", ifname);
		return (-1);
	}	
	
	mac_addr_t mac_rep;
	char check = arp_resolve(iface, ipv4_dst, mac_rep);
	switch (check)
	{
		case -1:
			printf("Error resolve ARP for host %s\n", argv[2]);
			return (-1);
		case 0:
			printf("Timeout to resolve ARP for host %s\n", argv[2]);
			return (0);
	}
	
	char mac_str[MAC_STR_LENGTH];

	mac_addr_str(mac_rep, mac_str);
	
	
	printf("\n%s -> %s\n\n", argv[2], mac_str);
	
	eth_close(iface);
	
	return (0);
}
