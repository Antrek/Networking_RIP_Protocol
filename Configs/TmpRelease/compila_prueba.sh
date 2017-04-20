clear;clear;clear;
set +v
echo "========== Compilando prueba =========="
cd source/
rawnetcc prueba2 prueba2.c rip.c rip_route_table.c udp.c ipv4.c eth.c ipv4_route_table.c ipv4_config.c arp.c
mv prueba2 ../bins/prueba2
cd ..
