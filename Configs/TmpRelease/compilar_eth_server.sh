clear;clear;clear;
set +v
echo "========== Compilando eth_server =========="
cd source/;
rawnetcc eth_server eth_server.c eth.c
mv eth_server ../bins/eth_server
cd ..
