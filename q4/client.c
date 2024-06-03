#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

#define BACKLOG 10
#define PORT 9034

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server_address> <server_port> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_address = argv[1];
    int server_port = atoi(argv[2]);

    // Define server socket, client socket, struct sockaddr
    int server_socket, new_fd;
    struct sockaddr_in server_addr;
    int sin_size;

    // Create socket
    if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, server_address, &server_addr.sin_addr);

    // Connect to server
    if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    else printf("client connect to the server\n");




    printf("Enter 2 focis: (x1,y1), (x2,y2), and radius\n");
        
    // Send the focis of the ellipse
    double x1, y1, x2, y2, radius;

    scanf("%lf %lf %lf %lf %lf", &x1, &y1, &x2, &y2, &radius);
    double data[] = {x1, y1, x2, y2, radius};
    
    if (send(server_socket, data, sizeof(data), 0) == -1) {
        perror("send");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    double answer;
    printf("Data sent to the server:\n");
    if (recv(server_socket, &answer, sizeof(double), 0) < -1) {
        perror("recv");
        close(server_socket);
    }
    printf("Percents covered = %.2lf\n" ,answer);
    

    close(server_socket);

    return 0;
}
