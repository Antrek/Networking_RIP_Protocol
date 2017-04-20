clear;clear;clear;
set +v
echo "========== Compilando rip_client =========="
cd source/
rawnetcc rip_client rip_client.c rip.c udp.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c rip_route_table.c
mv rip_client ../bins/rip_client
cd ..