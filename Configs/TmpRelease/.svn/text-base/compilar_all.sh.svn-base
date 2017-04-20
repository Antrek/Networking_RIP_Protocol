clear;clear;clear;
set +v
echo "========== Compilando TODO =========="
cd source/
echo "========== Compilando eth_client =========="
rawnetcc eth_client eth_client.c eth.c
mv eth_client ../bins/eth_client -f
echo "========== Compilando eth_server =========="
rawnetcc eth_server eth_server.c eth.c
mv eth_server ../bins/eth_server -f
echo "========== Compilando arp_client =========="
rawnetcc arp_client arp_client.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c arp_cache_table.c
mv arp_client ../bins/arp_client -f
echo "========== Compilando ip_client =========="
rawnetcc ip_client ipv4_client.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c arp_cache_table.c
mv ip_client ../bins/ip_client -f
echo "========== Compilando ip_server =========="
rawnetcc ip_server ipv4_server.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c arp_cache_table.c
mv ip_server ../bins/ip_server -f
echo "========== Compilando udp_client =========="
rawnetcc udp_client udp_client.c udp.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c arp_cache_table.c
mv udp_client ../bins/udp_client -f
echo "========== Compilando udp_server =========="
rawnetcc udp_server udp_server.c udp.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c arp_cache_table.c
mv udp_server ../bins/udp_server -f
echo "========== Compilando rip_client =========="
rawnetcc rip_client rip_client.c rip.c udp.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c rip_route_table.c arp_cache_table.c
mv rip_client ../bins/rip_client -f
echo "========== Compilando rip_server =========="
rawnetcc rip_server rip_server.c rip.c udp.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c rip_route_table.c arp_cache_table.c
mv rip_server ../bins/rip_server -f
echo "========== Compilando rip_send_reply =========="
rawnetcc rip_send_reply rip_send_reply.c rip.c udp.c arp.c eth.c ipv4.c ipv4_config.c ipv4_route_table.c rip_route_table.c arp_cache_table.c
mv rip_send_reply ../bins/rip_send_reply -f
cd ..
