clear;clear;clear;
set +v
echo "========== Compilando udp_client =========="
cd source/
rawnetcc udp_client udp_client.c udp.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c
mv udp_client ../bins/udp_client
cd ..