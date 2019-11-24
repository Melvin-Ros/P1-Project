#include <ros/ros.h>
#include <iostream>
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <visualization_msgs/Marker.h>
using namespace std;
//struct Coordinates { double X;double Y; };  
/** function declarations **/

void generatePoints(vector<vector<double>> &points,double sizeInSquareMeters, double minXvalue, double minYvalue, double maxXvalue, double distancebetweenpoints); 
int main(int argc, char **argv)
{
	double sizeInSquareMeters, distancebetweenpoints,minXvalue, minYvalue, maxXvalue;

    ros::init(argc, argv, "example_node");
    ros::NodeHandle n("~");
	ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 10);
    ros::Rate loop_rate(50);
	vector<vector<double>> Points;
	cout << "time to generate points input the following information: " << endl;
	cout << "size of map in square Meters: ";
	cin >> sizeInSquareMeters;
	cout << endl;
	cout << "minimum X value: ";
	cin >> minXvalue;
	cout << endl;
	cout << "minimum y value: ";
	cin >> minYvalue;
	cout << endl;
	cout << "maximum X value: ";
	cin >> maxXvalue;
	cout << endl;
	cout << "distance between the points: ";
	cin >> distancebetweenpoints;
	cout << endl;

	generatePoints(Points,sizeInSquareMeters,minXvalue,minYvalue,maxXvalue,distancebetweenpoints);

	int count = 0;
	//Draw points in Rviz
    while (ros::ok()&& count <= Points.size())
    {
		double choiceX = Points[count][0];
		double choiceY = Points[count][1];

		//cout << count << " " << Points[count][0] << "  " << Points[count][1] <<endl;
		//create a marker point
		visualization_msgs::Marker points;
		points.header.frame_id = "map";
		points.header.stamp = ros::Time::now();
		points.action = visualization_msgs::Marker::ADD;
		points.pose.orientation.w = 1.0;
		points.id = 0;
		points.type = visualization_msgs::Marker::POINTS;
		points.ns = "generate_points_node";
		// POINTS markers use x and y scale for width/height respectively
      points.scale.x = 0.3;
      points.scale.y = 0.3;
	  // Points are green
       points.color.g = 1.0f;
       points.color.a = 1.0;
	   //set point equal to generated points
	   geometry_msgs::Point p;
		p.x = choiceX;
        p.y = choiceY;
        p.z = 1.0;
		points.points.push_back(p);
		count++;

		marker_pub.publish(points);
        ros::spinOnce();
        loop_rate.sleep();
    }
}


void generatePoints(vector<vector<double>> &points,double sizeInSquareMeters, double minXvalue, double minYvalue, double maxXvalue, double distancebetweenpoints){

	int n = (int)sizeInSquareMeters/(int)distancebetweenpoints;
	int m = 1;
	vector<vector<double>> vec(n, vector<double> (m));
	points = vec;
	int count=0;
	for (int i = 0; i <= ((n/(int)maxXvalue-(int)minXvalue) + 1); i++)
	{ 
		
			for (int j = 0; j < (int)maxXvalue-(int)minXvalue; j++)
			{
				
				points[count+i][0] = minXvalue + j;
				points[count+i][1] = minYvalue + i;
				cout << count << " ";
				cout << points[count+i][0];
				cout << " " << points[count+i][1] << endl;
				count++;
			}
			
			
	}
	
	//for (int k = 0; k < n; k++)
//	{
	//	cout << points[k][0] << " " << points[k][1]<<endl;
		/* code */
	////}
	
	
	
}


