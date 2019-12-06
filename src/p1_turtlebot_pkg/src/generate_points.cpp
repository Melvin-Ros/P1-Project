#include <ros/ros.h>
#include <iostream>
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <visualization_msgs/Marker.h>
#include "geometry_msgs/Point.h"
#include <nav_msgs/MapMetaData.h>
#include <nav_msgs/OccupancyGrid.h>
#include "std_msgs/Header.h"
#include "nav_msgs/MapMetaData.h"
#include "p1_turtlebot_pkg/pointmsg.h"

using namespace std;
nav_msgs::OccupancyGrid occupancygrid;
bool isoccupied = false;
//struct Coordinates { double X;double Y; };  
/** function declarations **/
struct coordinatepoint{
	double x;
	double y;
};
vector<coordinatepoint> availablepoints;
void generatePoints(vector<coordinatepoint> &points,double distancebetweenpoints); 
void mapCallback(const nav_msgs::OccupancyGrid::ConstPtr& msg){
  occupancygrid.header = msg->header;


  occupancygrid.info = msg->info;
  occupancygrid.data = msg->data;
  isoccupied = true;
   
  
 
     
}
int main(int argc, char **argv)
{
	double  distancebetweenpoints;

    ros::init(argc, argv, "generate_points_node");
    ros::NodeHandle n("~");
	ros::Publisher pub = n.advertise<p1_turtlebot_pkg::pointmsg>("point_list", 100);
	ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("point_visualization_marker", 10);
     ros::Subscriber map_sub = n.subscribe("/map",2000,mapCallback);
	ros::Rate loop_rate(50);
	visualization_msgs::Marker points;
	p1_turtlebot_pkg::pointmsg msg;
	vector<coordinatepoint> Points;
	cout << "time to generate points input the following information: " << endl;

	cout << "distance between the points: ";
	cin >> distancebetweenpoints;
	cout << endl;
	while(marker_pub.getNumSubscribers() <= 0 || pub.getNumSubscribers() <= 0 || map_sub.getNumPublishers() <= 0){
		ROS_INFO("waiting for subscribers to come online");
		ros::spinOnce();
	}	
	while(isoccupied == false){
		ros::spinOnce();
		loop_rate.sleep();
	}
	
	generatePoints(Points, distancebetweenpoints);

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

	
	while (ros::ok)
	{
		marker_pub.publish(points);
		pub.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
	}
	
		
		
    
		
}


void generatePoints(vector<coordinatepoint> &points,double distancebetweenpoints){
	
for (unsigned int x = occupancygrid.info.origin.position.x; x <= occupancygrid.info.width; x+=(int)(distancebetweenpoints/occupancygrid.info.resolution))
{
	
	coordinatepoint temp;
    for (unsigned int y = occupancygrid.info.origin.position.y; y <= occupancygrid.info.height; y+=(int)(distancebetweenpoints/occupancygrid.info.resolution))
      {
		  
	  temp.y = y;
	  temp.x = x;

       
	  if(occupancygrid.data[(int)(temp.y *(occupancygrid.info.width)+temp.x)] != -1 && occupancygrid.data[int(temp.y  *(occupancygrid.info.width)+temp.x)] == 0)
	  {
		  temp.y *= occupancygrid.info.resolution;
		  temp.x *= occupancygrid.info.resolution;
		  cout << temp.x <<  " " << temp.y << endl;
		  points.push_back(temp);
	  }

	  }
}

	
}


