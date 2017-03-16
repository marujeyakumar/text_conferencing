all: .client .server

.client: 
	gcc -std=gnu11 -o build/client *.c client/*.c
	
.server: 
	gcc -std=gnu11 -o build/server *.c server/*.c
