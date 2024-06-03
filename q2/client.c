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
#define PORT 8080

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

    char response[35];
    
    if (recv(server_socket, response, sizeof(response), 0) == -1) {
        perror("recv");
        close(server_socket);
    }    
    printf("%s\n",response);

    int stay_allive = 1;
    while(stay_allive){
        char is_allive = '\0';
        
        // Send the focis of the ellipse
        double x1, y1, x2, y2, radius;
        char response[70];

        if (recv(server_socket, response, sizeof(response), 0) == -1) {
        perror("recv");
        close(server_socket);
        }    
        printf("%s\n\n",response);

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

        printf("Percents covered = %f\n" ,answer);
        
        while(is_allive != 'Y' && is_allive != 'N'){
            printf("Continue? press Y else press N\n");

            scanf(" %c",&is_allive);

            if (is_allive != 'Y' && is_allive != 'N')
            {
                printf("Wrong input, only Y / N\n");
            }
            else if(is_allive == 'N'){
                printf("Disconnect from the server\n");
                
                break;
            }
        }
        if(is_allive == 'N'){
            stay_allive = 0;
        }
        if (send(server_socket, &stay_allive, sizeof(int), 0) == -1) {
            perror("send");
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        
    }

    close(server_socket);

    return 0;
}
