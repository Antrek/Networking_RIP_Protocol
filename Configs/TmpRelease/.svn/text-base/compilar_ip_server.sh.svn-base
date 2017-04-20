clear;clear;clear;
set +v
echo "========== Compilando ip_server =========="
cd source/
rawnetcc ip_server ipv4_server.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c
mv ip_server ../bins/ip_server
cd ..
