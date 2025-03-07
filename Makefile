all:
	g++ pollSwayer.cpp -o pollSwayer	-lpthread
	g++ poller.cpp -o poller	-lpthread
clean: 
	rm pollSwayer	
	rm poller