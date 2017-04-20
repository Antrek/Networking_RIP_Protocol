clear;clear;clear;
set +v
echo "========== Compilando rip_server =========="
cd source/
rawnetcc rip_server rip_server.c rip.c udp.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c rip_route_table.c
mv rip_server ../bins/rip_server
cd ..