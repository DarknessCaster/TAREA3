all:
	g++ -Wall serial.cpp -c
	g++ -Wall slip.cpp -c
	g++ -Wall nodo.cpp -c
	g++ -Wall funcionesIP.cpp -c
	g++ -Wall slip.o serial.o funcionesIP.o nodo.o -o nodo
mk_socket:
	g++ -Wall virtualSocket.cpp -o virtualSocket
red: 
	./virtualSocket ../tmp/p1 ../tmp/p2 &
	./virtualSocket ../tmp/p3 ../tmp/p4 &
	./virtualSocket ../tmp/p5 ../tmp/p6 &
	./virtualSocket ../tmp/p7 ../tmp/p8 &
	./virtualSocket ../tmp/p9 ../tmp/p10 &
run_nodo_A:
	./nodo 10.10.10.10 ../tmp/p1 ../tmp/p10
run_nodo_B:
	./nodo 11.11.11.11 ../tmp/p3 ../tmp/p2
run_nodo_C:
	./nodo 12.12.12.12 ../tmp/p5 ../tmp/p4
run_nodo_D:
	./nodo 13.13.13.13 ../tmp/p7 ../tmp/p6
run_nodo_E:
	./nodo 14.14.14.14 ../tmp/p9 ../tmp/p8
clean:
	rm *.o