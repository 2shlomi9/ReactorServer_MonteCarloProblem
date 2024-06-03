#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define M_PI 3.14159265358979323846

int isInsideEllipse(double x, double y, double x1, double y1, double x2, double y2, double radius){
    double semi_major_axis = radius/2;
    double focis_distance = (sqrt(pow(x1-x2,2) - pow(y1-y2,2)));
    double semi_minor_axis =  (sqrt(pow(semi_major_axis,2) + pow(focis_distance,2)));
    double ans = (pow(x,2) / pow(semi_major_axis,2)) + (pow(y,2) / pow(semi_minor_axis,2));
    if(ans <= 1) return 1;
    return 0;
}


int main(int argc, char* argv[]){

    if(argc < 6){
        printf("Usage: %s x1 y1 x2 y2 radius\n", argv[0]);
        return 1;
    }
    double x1 = atof(argv[1]);
    double y1 = atof(argv[2]);
    double x2 = atof(argv[3]);
    double y2 = atof(argv[4]);
    double radius = atof(argv[5]);
    if(sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)) > radius) {
        printf("distance of (x1,y1) = (%f,%f) (x2,y2) = (%f,%f) cant be greater then radius = %f !\n",x1,y1,x2,y2,radius);
        exit(EXIT_FAILURE);
    }

    int canvasSize = 100;
    int canvasMinX = -50;
    int canvasMinY = -50;

    int totalPoints = 1000;

    int pointsInsideEllipse = 0;

    for (int i = 0; i < totalPoints; i++) {

        double x = ((double)rand() / RAND_MAX) * canvasSize + canvasMinX; // Random x-coordinate
        double y = ((double)rand() / RAND_MAX) * canvasSize + canvasMinY; // Random y-coordinate

        if (isInsideEllipse(x, y, x1, y1, x2, y2, radius)) {
            pointsInsideEllipse++;
        }
    }

    double canvasArea = canvasSize * canvasSize;
    double estimatedEllipseArea = (double)pointsInsideEllipse / totalPoints * canvasArea;

    double semi_major_axis = radius/2;
    double focis_distance = (sqrt(pow(x1-x2,2) + pow(y1-y2,2)));
    double semi_minor_axis =  (sqrt(pow(semi_major_axis,2) - pow(focis_distance,2)));
    double ellipseArea = semi_major_axis*semi_minor_axis* M_PI;
    double percentageCovered = (estimatedEllipseArea / canvasArea) * 100;

    printf("Ellipse Area = %f \nEstimated Ellipse Area = %f \nPercent covered = %f" ,ellipseArea,estimatedEllipseArea,percentageCovered);
    return 0;

}


