#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

#define M_PI 3.14159265358979323846
#define BACKLOG 10
#define PORT 8080
#define BUFFER_SIZE


int isInsideEllipse(double x, double y, double x1, double y1, double x2, double y2, double radius){
    double semi_major_axis = radius/2;
    double focis_distance = (sqrt(pow(x1-x2,2) - pow(y1-y2,2)));
    double semi_minor_axis =  (sqrt(pow(semi_major_axis,2) + pow(focis_distance,2)));
    double ans = (pow(x,2) / pow(semi_major_axis,2)) + (pow(y,2) / pow(semi_minor_axis,2));
    if(ans <= 1) return 1;
    return 0;
}

double percentageCovered(double x1, double y1, double x2, double y2, double radius){

    if(sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)) > radius) {
        printf("distance of (x1,y1) = (%f,%f) (x2,y2) = (%f,%f) cant be greater then radius = %f !\n",x1,y1,x2,y2,radius);
        exit(EXIT_FAILURE);
    }

    int canvasSize = 100;
    int canvasMinX = -50;
    int canvasMinY = -50;

    int totalPoints = 1000;

    int points_inside_ellipse = 0;

    for (int i = 0; i < totalPoints; i++) {

        double x = ((double)rand() / RAND_MAX) * canvasSize + canvasMinX; // Random x-coordinate
        double y = ((double)rand() / RAND_MAX) * canvasSize + canvasMinY; // Random y-coordinate

        if (isInsideEllipse(x, y, x1, y1, x2, y2, radius)) {
            points_inside_ellipse++;
        }
    }

    double canvas_area = canvasSize * canvasSize;
    double estimated_ellipse_area = (double)points_inside_ellipse / totalPoints * canvas_area;

    double semi_major_axis = radius/2;
    double focis_distance = (sqrt(pow(x1-x2,2) - pow(y1-y2,2)));
    double semi_minor_axis =  (sqrt(pow(semi_major_axis,2) + pow(focis_distance,2)));
    double percentage_covered = (estimated_ellipse_area / canvas_area) * 100;
    return percentage_covered;
}

int main(){

    // define server socket, client socket , struct sockaddr 

    int server_socket,client_socket;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    int sin_size;

    // create socket 

    if((server_socket = socket(PF_INET,SOCK_STREAM,0)) == -1 ){
        perror("socket");
        exit(1);
    }

    
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&(my_addr.sin_zero),8);
    
    // binding 

    if(bind(server_socket,(struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1 ){
        perror("bind");
        exit(1);        
    }
    
    // listening 

    if(listen(server_socket,BACKLOG) == -1){
        perror("listen");
        exit(1);         
    }
     
    printf("Server start running \n");
    // accept

    while(1){
        sin_size = sizeof(struct sockaddr_in);
        if((client_socket = accept(server_socket,(struct sockaddr *)&their_addr,&sin_size)) == -1 ){
        perror("accept");
        exit(1);             
        }
        
        printf("server : Got connection from %s\n", inet_ntoa(their_addr.sin_addr));
        char response[] = "Connect to the server successfully";
        if(send(client_socket, response, sizeof(response),0) == -1){
            perror("send");
            close(client_socket);
            continue;
        }  
        while(1){      
            int stay_allive = 0;
            
            char response[] = "Enter 2 focis: (x1,y1), (x2,y2), and radius";
            if(send(client_socket, response, sizeof(response),0) == -1){
                perror("send");
                close(client_socket);
                break;
            } 

            double x1,y1,x2,y2,radius;

            // Receive all data from client
            double data[5];
            if (recv(client_socket, data, sizeof(data), 0) == -1) {
                perror("recv");
                close(client_socket);
                continue;
            }

            x1 = data[0];
            y1 = data[1];
            x2 = data[2];
            y2 = data[3];
            radius = data[4];

            printf("server : receive data :\nx1 = %lf, y1 = %lf, x2 = %lf, y2 = %lf, radius = %lf\n",x1,y1,x2,y2,radius);

            double ans = percentageCovered(x1,y1,x2,y2,radius);
            if(send(client_socket,&ans,sizeof(double),0) == -1){
                perror("recv");
                close(client_socket);
                continue;
            }
            if (recv(client_socket, &stay_allive, sizeof(int), 0) == -1) {
                perror("recv");
                close(client_socket);
                continue;
            }
            if (stay_allive == 0)
            {
                close(client_socket);
                break;
            }
            

        }
        
    }
    return 0;

    
}