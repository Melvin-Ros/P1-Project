#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <math.h>


using namespace std;

struct point
{
double x;
double y;
bool inRoute=false;
};


void arrayPrinter(vector<point> &pointArray)
{
    cout << "[";

    for(int i=0; i<10; i++)
    {
        cout << pointArray[i].x << "," << pointArray[i].y << "; ";
    }

    cout << "]" << endl;
}


void pointGen(vector<point> &pointArray)
{
    srand(time(NULL));
    for(int i = 0; i < 10; i++)
    {
        pointArray[i].x=rand() % 10 + 1;
        cout << "yes" << endl;
        pointArray[i].y=rand() % 10 + 1;

        cout << "Point " << i << " is: " << pointArray[i].x << "," << pointArray[i].y << endl;
    }
}


void greedyRouteGen(vector<point> &pointArray, vector<point> &routeOrder)
{
    double shortestDist, currentDist;
    point currentPosition;
    int bestPoint;

    currentPosition.x=0;
    currentPosition.y=0;


    shortestDist = hypot((currentPosition.x - pointArray[0].x), (currentPosition.y - pointArray[0].y));
    bestPoint=0;
    routeOrder[0] = pointArray[0];

    for (int n=0; n < 11; n++)
    {
        if (n!=10)
        {
            int i = -1;
            do
            {
                i++;
                if (pointArray[i].inRoute=false)
                {
                    shortestDist = hypot((routeOrder[i].x - pointArray[i].x), (routeOrder[n].y - pointArray[i].y));
                    bestPoint=i;
                    routeOrder[n] = pointArray[i];
                }
            }
            while (pointArray[i].inRoute=true);


            for (int i=0; i < 10; i++)
            {
                if (pointArray[i].inRoute=false)
                {
                    if (n==0)
                    {
                        currentDist = hypot((currentPosition.x - pointArray[i].x), (currentPosition.y - pointArray[i].y));
                    }
                    else
                    {
                        currentDist = hypot((routeOrder[n].x - pointArray[i].x), (routeOrder[n].y - pointArray[i].y));
                    }

                    if (currentDist < shortestDist){
                        shortestDist=currentDist;
                        routeOrder[n]=pointArray[i];
                        bestPoint = i;
                    }
                }
            }
            pointArray[bestPoint].inRoute = true;
        }
        else
        {
            routeOrder[n]=currentPosition;
        }
    }
}


/*void routeModifier(vector<point> &routeArray, vector<point> &routeOrder)
{



}


void routeLengthCalc(vector<point> &routeOrder)
{

}*/


int main()
{
    vector<point> pointArray;
    vector<point> routeOrder;


    pointGen(pointArray);
    //arrayPrinter(pointArray);
    //greedyRouteGen(pointArray,routeOrder);
    //arrayPrinter(routeOrder);

    /*pointArray=routeOrder;

    routeModifier(pointArray, routeOrder);
    routeLengthCalc(routeOrder);*/

    return 0;
}
