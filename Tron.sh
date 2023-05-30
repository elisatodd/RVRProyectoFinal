make clean
make game

# open server
./TronServer.out 0.0.0.0 2600 &
sleep 1.5

# open clients
./TronClient.out 127.0.0.1 2600 &