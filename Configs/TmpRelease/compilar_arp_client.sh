clear;clear;clear;
set +v
echo "========== Compilando arp_client =========="
cd source/
rawnetcc arp_client arp_client.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c arp_cache_table.c
mv arp_client ../bins/arp_client
cd ..


