#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <iostream>
#include <actionlib/client/simple_action_client.h>

/** function declarations **/
bool moveToGoal(double xGoal, double yGoal);

int main(int argc, char **argv)
{
    ros::init(argc, argv, "example_node");
    ros::NodeHandle n("~");
    ros::Rate loop_rate(50);
    while (ros::ok())
    {
		double choiceX;
		double choiceY;
		std::cout << "Input x coordinate: " << std::endl;
		std::cin >> choiceX;
		std::cout << "Input y coordinate: " << std::endl;
		std::cin >> choiceY;
		if(moveToGoal(choiceX,choiceY)){
			std::cout << "reached location:" << std::endl;
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
