PORT=5676

make

pkill client
pkill server

build/server $PORT &
build/client < "client_1.txt" &
sleep .2
#build/client < "client_2.txt" &
