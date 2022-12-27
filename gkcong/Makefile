all: udp_client_1 tcp_client_report tcp_client server 
udp_client_1: udp_client_1.c
	gcc udp_client_1.c -o udp_client_1
tcp_client_report: tcp_client_report.c
	gcc tcp_client_report.c -o tcp_client_report
tcp_client: tcp_client.c
	gcc tcp_client.c -o tcp_client
server: server.c
	gcc server.c -o server
clean:
	rm udp_client_1 tcp_client_report tcp_client server
