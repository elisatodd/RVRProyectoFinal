make clean
make game

# open server
./TronServer.out 0.0.0.0 2600 &
sleep 1.0

# open clients with time delay to see how theyys are connected
./TronClient.out 127.0.0.1 2600 &
sleep 3.0
./TronClient.out 127.0.0.1 2600 &
