clear;clear;clear;
set +v
echo "========== Compilando ip_client =========="
cd source/
rawnetcc ip_client ipv4_client.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c arp_cache_table.c
mv ip_client ../bins/ip_client
cd ..
