clear;clear;clear;
set +v
echo "========== Compilando udp_server =========="
cd source/
rawnetcc udp_server udp_server.c udp.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c
mv udp_server ../bins/udp_server
cd ..