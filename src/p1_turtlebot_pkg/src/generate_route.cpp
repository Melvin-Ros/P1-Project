#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <iostream>
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_listener.h>
using namespace std;
//struct Coordinates { double X;double Y; };  
/** function declarations **/
bool moveToGoal(double xGoal, double yGoal);
void generatePoints(double (&points)[100][1]); 
int main(int argc, char **argv)
{
	
    ros::init(argc, argv, "example_node");
    ros::NodeHandle n("~");
	//create listener that gives us the coordinates of the robot from amcl
	tf::TransformListener listener;

    ros::Rate loop_rate(50);
	double points[100][1];
	generatePoints(points);
	int count = 0;
    while (ros::ok())
    {
		double choiceX = points[count][0];
		double choiceY = points[count][1];

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


	
	
		/*
		std::cout << "choose whether random points or input points enter 1 or 2 ";
		std::cin >> choiceX;
		std::cout << std::endl;
		if(choiceX == 1){
		srand(time(NULL));
		choiceX = rand()%9+1;
		srand(time(NULL)+1);
		choiceY = rand()%9+1;
		}
		else if(choiceX == 2){
		std::cout << "enter x coordinate" << std::endl;
		std::cin >> choiceX;
		std::cout << "enter y coordinate" << std::endl;
		std::cin >> choiceY;
		}
		*/

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

void generatePoints(double (&points)[100][1]){
	
	for (int i = 0; i <= 100; i++)
	{
		srand(time(NULL)+i);
		points[i][0] = rand()%8+1;
		srand(time(NULL)+i+1);
		points[i][1] = rand()%8+1;
	}
	
}
