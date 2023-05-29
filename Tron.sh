make clean
make game

# open server
./Server_Tron.out 0.0.0.0 2600 &
sleep 1.5

# open clients
./Client_Tron.out 127.0.0.1 2600 &