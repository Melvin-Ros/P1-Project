#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <iostream>
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_listener.h>
#include "p1_turtlebot_pkg/pointmsg.h"


using namespace std;

vector<geometry_msgs::Point> mypoints;
/** function declarations **/
bool moveToGoal(double xGoal, double yGoal);
void getpoints(const p1_turtlebot_pkg::pointmsg::ConstPtr& msg);
int main(int argc, char **argv)
{
	
    ros::init(argc, argv, "generate_route_node");
    ros::NodeHandle n("~");
	ros::Subscriber sub = n.subscribe("/generate_points_node/point_list", 100, getpoints);
	
	//create listener that gives us the coordinates of the robot from amcl
	tf::TransformListener listener;

    ros::Rate loop_rate(50);
	 
	int count = 0;
	while (ros::ok)
	{
		/* code */
	
	
    while (ros::ok()&& sub.getNumPublishers() >=0 && mypoints.size() >= 2)
    {
	

		double choiceX = mypoints[count].x;
		double choiceY = mypoints[count].y;
		
		//create a time stamped transform so that we can also get previous positions and possible future positions
		tf::StampedTransform transform;
	//Attempt to get the position of the robot
	try
	{
		/* code */
		listener.lookupTransform("/map","/base_link",ros::Time(0), transform);
		ROS_INFO("Got a transform! x = %f, y = %f",transform.getOrigin().x(),transform.getOrigin().y());
		cout << "yea" << endl;
	}
	catch(tf::TransformException ex)
	{
		//ROS_ERROR("no transform", ex.what());
		cout << "no transform send help" << endl;
	}



		if(moveToGoal(choiceX,choiceY)){
			std::cout << "reached location:" << std::endl;
			
			if(count <= 100){
				count++;
			}
		}
		else if(!moveToGoal(choiceX,choiceY)){
			std::cout << "location: not reached going to next goal" << std::endl;
			
			if(count <= 100){
				count++;
			}
		}
        ros::spinOnce();
        loop_rate.sleep();
    }

	ros::spinOnce();
    loop_rate.sleep();
	}
}

void getpoints(const p1_turtlebot_pkg::pointmsg::ConstPtr& msg) {
    //ROS_INFO("first point: x=%.2f, y=%.2f", msg->points[msg->another_field].x, msg->points[msg->another_field].y);
	if(mypoints.size()<= 2){
		mypoints = msg->points;
	}
	
}
bool moveToGoal(double xGoal, double yGoal){

	//define a client for to send goal requests to the move_base server through a SimpleActionClient
	actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> ac("move_base", true);

	//wait for the action server to come up
	while(!ac.waitForServer(ros::Duration(5.0))){
		ROS_INFO("Waiting for the move_base action server to come up");
	}

	move_base_msgs::MoveBaseGoal goal;
	
	//set up the frame parameters
	goal.target_pose.header.frame_id = "map";
	goal.target_pose.header.stamp = ros::Time::now();

	/* moving towards the goal*/
	cout << "xgoal: " << xGoal << endl;
	cout << "ygoal " << yGoal << endl;

	goal.target_pose.pose.position.x =  xGoal;
	goal.target_pose.pose.position.y =  yGoal;
	goal.target_pose.pose.position.z =  0.0;
	goal.target_pose.pose.orientation.x = 0.0;
	goal.target_pose.pose.orientation.y = 0.0;
	goal.target_pose.pose.orientation.z = 0.0;
	goal.target_pose.pose.orientation.w = 1.0;

	ROS_INFO("Sending goal location ...");
	ac.sendGoal(goal);
	
	ac.waitForResult();

	if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED){
		ROS_INFO("You have reached the destination");
		return true;
	}
	else{
		ROS_INFO("The robot failed to reach the destination");
		return false;
	}

}

