#include <ros/ros.h>
#include <iostream>
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <visualization_msgs/Marker.h>
#include "geometry_msgs/Point.h"
#include "p1_turtlebot_pkg/pointmsg.h"

using namespace std;
//struct Coordinates { double X;double Y; };  
/** function declarations **/
struct coordinatepoint{
	double x;
	double y;
};

void generatePoints(vector<coordinatepoint> &points,double sizeOnX, double sizeOnY, double minXvalue, double minYvalue, double distancebetweenpoints); 
int main(int argc, char **argv)
{
	double sizeOnX, sizeOnY, distancebetweenpoints,minXvalue, minYvalue;

    ros::init(argc, argv, "generate_points_node");
    ros::NodeHandle n("~");
	ros::Publisher pub = n.advertise<p1_turtlebot_pkg::pointmsg>("point_list", 100);
	ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("point_visualization_marker", 10);
    ros::Rate loop_rate(50);
	visualization_msgs::Marker points;
	p1_turtlebot_pkg::pointmsg msg;
	vector<coordinatepoint> Points;
	cout << "time to generate points input the following information: " << endl;
	cout << "size of map on x axis : ";
	cin >> sizeOnX;
	cout << "size of map on y axis : ";
	cin >> sizeOnY;
	cout << endl;
	cout << "minimum X value: ";
	cin >> minXvalue;
	cout << endl;
	cout << "minimum y value: ";
	cin >> minYvalue;
	cout << endl;
	cout << "distance between the points: ";
	cin >> distancebetweenpoints;
	cout << endl;

	generatePoints(Points, sizeOnX,sizeOnY, minXvalue, minYvalue, distancebetweenpoints);

	int count = 0;

	
	//Draw points in Rviz
    
	while (count <= Points.size()&& ros::ok())
	{
		/* code */
	
	
		double choiceX = Points[count].x;
		double choiceY = Points[count].y;

		//cout << choiceX << " " << choiceY << endl;

		//cout << count << " " << Points[count][0] << "  " << Points[count][1] <<endl;
		//create a marker point
		
		points.header.frame_id = "map";
		points.header.stamp = ros::Time::now();
		points.action = visualization_msgs::Marker::ADD;
		points.pose.orientation.w = 1.0;
		points.id = count;
		points.type = visualization_msgs::Marker::POINTS;
		points.ns = "generate_points_node";
		// POINTS markers use x and y scale for width/height respectively
      points.scale.x = 0.3;
      points.scale.y = 0.3;
	  // Points are green
       points.color.g = 1.0f;
       points.color.a = 1.0f;
	   //set point equal to generated points
	   geometry_msgs::Point p;
	   
		p.x = choiceX;
        p.y = choiceY;
        p.z = 0;
		points.points.push_back(p);
		msg.another_field = count;
		msg.points.push_back(p);
		count++;
	}	

	while(marker_pub.getNumSubscribers() <= 0 || pub.getNumSubscribers() <= 0){
		ROS_INFO("waiting for subscribers to come online");
		ros::spinOnce();
	}	
		marker_pub.publish(points);
		pub.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
		
    
		
}


void generatePoints(vector<coordinatepoint> &points,double sizeonX, double sizeonY, double minXvalue, double minYvalue, double distancebetweenpoints){

for (int i = 0; i <= sizeonY/distancebetweenpoints; i++)
{
	coordinatepoint temp;
	for (int j = 0; j <= sizeonX/distancebetweenpoints; j++)
	{
		temp.x= minXvalue +(distancebetweenpoints*j);
		temp.y = minYvalue + (distancebetweenpoints*i);
		//cout << temp.x << " " << temp.y << endl;
		points.push_back(temp);
		//cout <<  minXvalue +(distancebetweenpoints*j) << " " << minYvalue + (distancebetweenpoints*i) << endl;
			}
	
}

	
}


