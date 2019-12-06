#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <iostream>
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_listener.h>
#include "p1_turtlebot_pkg/pointmsg.h"
#include "sound_play/sound_play.h"

using namespace std;
struct point
{
double x;
double y;
bool inRoute;
};
struct line
{
point firstPoint, secondPoint;
double a, b;
bool vertical;
};
vector<geometry_msgs::Point> mypoints;
vector<point> pointArray;
/** function declarations **/
bool moveToGoal(double xGoal, double yGoal);
void getpoints(const p1_turtlebot_pkg::pointmsg::ConstPtr& msg);
void greedyRouteGen(vector<point> &pointArray, point currentPosition);
line lineFormCalc (line calcLine);
bool intersectCheck(line currentLine, line testline);
void routeModifier(vector<point> &pointArray);


int main(int argc, char **argv)
{
	
    ros::init(argc, argv, "generate_route_node");
    ros::NodeHandle n("~");
	ros::Subscriber sub = n.subscribe("/generate_points_node/point_list", 100, getpoints);
	
	//create listener that gives us the coordinates of the robot from amcl
	tf::TransformListener listener;
	sound_play::SoundClient sc;
    ros::Rate loop_rate(50);
	 
	int count = 0;
		/* code */
	while(mypoints.size()<=1)
	{
		cout << "waiting for publisher"<< endl;
		ros::spinOnce();
    	loop_rate.sleep();
	}
	
		if(mypoints.size()>=1)
		{
			for(int i = 0; i <= mypoints.size(); i++)
			{
				point temp;
				temp.x = mypoints[i].x;
				temp.y = mypoints[i].y;
				temp.inRoute = false;
				pointArray.push_back(temp);
			}
		}

//create a time stamped transform so that we can also get previous positions and possible future positions
		tf::StampedTransform transform;
		point currentPosition;
		currentPosition.inRoute = true;
		//Attempt to get the position of the robot
		try
		{
			/* code */
			listener.lookupTransform("/map","/base_link",ros::Time(0), transform);
			ROS_INFO("Got a transform! x = %f, y = %f",transform.getOrigin().x(),transform.getOrigin().y());
			currentPosition.x = transform.getOrigin().x();
			currentPosition.y = transform.getOrigin().y();
			currentPosition.inRoute = false;
			cout << "yea" << endl;
		}
		catch(tf::TransformException ex)
		{
			//ROS_ERROR("no transform", ex.what());
			cout << "no transform send help" << endl;
		}
		
	
		greedyRouteGen(pointArray, currentPosition);
		routeModifier(pointArray);
		for	(int p; p<pointArray.size(); p++)
		{
			cout << pointArray[p].x << " , " << pointArray[p].y << ": " << endl;
		}
		 
    while (ros::ok() && pointArray.size() >= 2)
    {	
		

		int temp;
		cin >> temp;
		
		double choiceX = pointArray[count].x;
		double choiceY = pointArray[count].y;

		if(moveToGoal(choiceX,choiceY))
		{
			std::cout << "reached location:" << std::endl;
			//play sound
			sc.playWave("/home/ros/P1-Project/src/p1_turtlebot_pkg/src/Crow_Call_2.wav", 1.0f);
			
			if(count <= 100)
			{
				count++;
			}
		}
		else if(!moveToGoal(choiceX,choiceY))
		{
			std::cout << "location: not reached going to next goal" << std::endl;
			
			if(count <= 100)
			{
				count++;
			}
		}
        
    }

	ros::spinOnce();
    loop_rate.sleep();
	
} 


void getpoints(const p1_turtlebot_pkg::pointmsg::ConstPtr& msg) {
    //ROS_INFO("first point: x=%.2f, y=%.2f", msg->points[msg->another_field].x, msg->points[msg->another_field].y);
	if(mypoints.size()<= 1){
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

void greedyRouteGen(vector<point> &pointArray, point currentPosition)
{
    vector<point> routeOrder;
    double shortestDist, currentDist;
    int bestPoint;

    shortestDist = hypot((currentPosition.x - pointArray[0].x), (currentPosition.y - pointArray[0].y));
    bestPoint=0;

    for (int i=0; i<pointArray.size();i++)
    {
        currentDist = hypot((currentPosition.x - pointArray[i].x), (currentPosition.y - pointArray[i].y));
        if(currentDist<shortestDist)
        {
            bestPoint=i;
            shortestDist=currentDist;
            //cout << "It is. Setting current shortest distance to " << shortestDist << endl;
        }
        else
        {
            //cout << "It isn't" << endl;
        }
    }

    routeOrder.push_back(pointArray[bestPoint]);
    pointArray[bestPoint].inRoute=true;

    for (int n=0; n<pointArray.size()-1;n++)
    {
        int r = -1;
        do
        {
            r=r+1;
            if (!pointArray[r].inRoute)
            {
                bestPoint=r;
                shortestDist = hypot((routeOrder[n].x - pointArray[r].x), (routeOrder[n].y - pointArray[r].y));
            }
        }
        while (pointArray[r].inRoute);

        for (int i=0; i<pointArray.size();i++)
        {
            if (!pointArray[i].inRoute)
            {
                currentDist = hypot((routeOrder[n].x - pointArray[i].x), (routeOrder[n].y - pointArray[i].y));
                //cout << "the distance from point " << n << " to point " << i << " is: " << currentDist << endl;
                //cout << "Testing if " << currentDist << " is lower than " << shortestDist << endl;
                if(currentDist<shortestDist)
                {
                    bestPoint=i;
                    shortestDist=currentDist;
                    //cout << "It is. Setting current shortest distance to " << shortestDist << endl;
                }
                else
                {
                    //cout << "It isn't" << endl;
                }
            }
        }
        routeOrder.push_back(pointArray[bestPoint]);
        pointArray[bestPoint].inRoute=true;
    }

    routeOrder.push_back(currentPosition);
    pointArray=routeOrder;
}

line lineFormCalc (line calcLine)
{
    if (calcLine.firstPoint.x != calcLine.secondPoint.x)
    {
        calcLine.a = (calcLine.firstPoint.y-calcLine.secondPoint.y)/(calcLine.firstPoint.x-calcLine.secondPoint.x);
        calcLine.b = (calcLine.firstPoint.y - (calcLine.a*calcLine.firstPoint.x));
        calcLine.vertical=false;
    }
    else
    {
        calcLine.a = 1000;
        calcLine.b = (calcLine.firstPoint.x);
        calcLine.vertical=true;
    }

    return calcLine;
}
bool intersectCheck(line currentLine, line testline)
{
    point intersectionPlacement;

    if (currentLine.vertical)
    {
        intersectionPlacement.x = currentLine.firstPoint.x;
        intersectionPlacement.y = ((testline.a*intersectionPlacement.x)+testline.b);

        if (currentLine.firstPoint.y <= intersectionPlacement.y && intersectionPlacement.y <= currentLine.secondPoint.y)
        {
                if (testline.firstPoint.x <= intersectionPlacement.x && intersectionPlacement.x <= testline.firstPoint.x)
                {
                    return true;
                }
                else if (testline.firstPoint.x >= intersectionPlacement.x && intersectionPlacement.x >= testline.secondPoint.x)
                {
                    return true;
                }
                else
                {
                    return false;
                }
        }
        else if (currentLine.firstPoint.y >= intersectionPlacement.y && intersectionPlacement.y >= currentLine.secondPoint.y)
        {
                if (testline.firstPoint.x <= intersectionPlacement.x && intersectionPlacement.x <= testline.firstPoint.x)
                {
                    return true;
                }
                else if (testline.firstPoint.x >= intersectionPlacement.x && intersectionPlacement.x >= testline.secondPoint.x)
                {
                    return true;
                }
                else
                {
                    return false;
                }
        }
        else
        {
            return false;
        }
    }
    else if (testline.vertical)
    {
        intersectionPlacement.x = testline.firstPoint.x;
        intersectionPlacement.y = ((currentLine.a*intersectionPlacement.x)+currentLine.b);

        if (currentLine.firstPoint.x <= intersectionPlacement.x && intersectionPlacement.x <= currentLine.secondPoint.x)
        {
                if (testline.firstPoint.y <= intersectionPlacement.y && intersectionPlacement.y <= testline.firstPoint.y)
                {
                    return true;
                }
                else if (testline.firstPoint.y >= intersectionPlacement.y && intersectionPlacement.y >= testline.secondPoint.y)
                {
                    return true;
                }
                else
                {
                    return false;
                }
        }
        else if (currentLine.firstPoint.x >= intersectionPlacement.x && intersectionPlacement.x >= currentLine.secondPoint.x)
        {
                if (testline.firstPoint.y <= intersectionPlacement.y && intersectionPlacement.y <= testline.firstPoint.y)
                {
                    return true;
                }
                else if (testline.firstPoint.y >= intersectionPlacement.y && intersectionPlacement.y >= testline.secondPoint.y)
                {
                    return true;
                }
                else
                {
                    return false;
                }
        }
        else
        {
            return false;
        }
    }
    else
    {
        intersectionPlacement.x = ((testline.b-currentLine.b)/(currentLine.a-testline.a));
        intersectionPlacement.y = ((currentLine.a*intersectionPlacement.x)+currentLine.b);

        if (currentLine.firstPoint.x <= intersectionPlacement.x && intersectionPlacement.x <= currentLine.secondPoint.x)
        {
                if (testline.firstPoint.x <= intersectionPlacement.x && intersectionPlacement.x <= testline.firstPoint.x)
                {
                    return true;
                }
                else if (testline.firstPoint.x >= intersectionPlacement.x && intersectionPlacement.x >= testline.secondPoint.x)
                {
                    return true;
                }
                else
                {
                    return false;
                }
        }
        else if (currentLine.firstPoint.x >= intersectionPlacement.x && intersectionPlacement.x >= currentLine.secondPoint.x)
        {
                if (testline.firstPoint.x <= intersectionPlacement.x && intersectionPlacement.x <= testline.firstPoint.x)
                {
                    return true;
                }
                else if (testline.firstPoint.x >= intersectionPlacement.x && intersectionPlacement.x >= testline.secondPoint.x)
                {
                    return true;
                }
                else
                {
                    return false;
                }
        }
        else
        {
            return false;
        }
    }
}


void routeModifier(vector<point> &pointArray)
{
    line currentLine;
    line testLine;
    bool intersected;
    int breaker;

    breaker = 0;
    currentLine.firstPoint = pointArray[pointArray.size()];
    currentLine.secondPoint = pointArray[0];
    currentLine = lineFormCalc(currentLine);

    do
    {
        intersected = false;

        for (int i = 1; i < pointArray.size()-2; i++)
        {
            testLine.firstPoint = pointArray[i];
            testLine.secondPoint = pointArray[i+1];
            testLine = lineFormCalc(testLine);

            if (currentLine.a != testLine.a)
            {

                bool isIntersected = intersectCheck(currentLine, testLine);
                if (isIntersected)
                {
                    cout << "The line between (" << currentLine.firstPoint.x << "," << currentLine.firstPoint.y << ") and (" << currentLine.secondPoint.x << "," << currentLine.secondPoint.y <<
                    ") \nintersects with the line between (" << testLine.firstPoint.x << "," << testLine.firstPoint.y << ") and (" << testLine.secondPoint.x << "," << testLine.secondPoint.y << ")" << endl;

                    double crossedLength, uncrossedLength;
                    crossedLength = hypot((currentLine.firstPoint.x - currentLine.secondPoint.x), (currentLine.firstPoint.y - currentLine.secondPoint.y)) + hypot((testLine.firstPoint.x - testLine.secondPoint.x), (testLine.firstPoint.y - testLine.secondPoint.y));
                    uncrossedLength = hypot((currentLine.firstPoint.x - testLine.firstPoint.x), (currentLine.firstPoint.y - testLine.firstPoint.y)) + hypot((currentLine.secondPoint.x - testLine.secondPoint.x), (currentLine.secondPoint.y - testLine.secondPoint.y));
                    if (uncrossedLength < crossedLength)
                    {
                        point tempPoint;
                        for (int f = 0, g = i; f < g; f++, g--)
                        {
                            cout << "exchanging point " << f << " and point " << g << endl;
                            tempPoint = pointArray[f];
                            pointArray[f] = pointArray[g];
                            pointArray[g] = tempPoint;
                        }
                        intersected = true;
                    }
                }
                else
                {
                    //cout << "It doesn't" << endl;
                }
            }
        }


        for (int p = 0; p < pointArray.size()-1; p++)
        {
            currentLine.firstPoint = pointArray[p];
            currentLine.secondPoint = pointArray[p+1];
            currentLine = lineFormCalc(currentLine);

            for (int i = p+2; i < pointArray.size()-1; i++)
            {
                testLine.firstPoint = pointArray[i];
                testLine.secondPoint = pointArray[i+1];
                testLine = lineFormCalc(testLine);

                if (currentLine.a != testLine.a)
                {

                    bool isIntersected = intersectCheck(currentLine, testLine);
                    if (isIntersected)
                    {
                        cout << "The line between (" << currentLine.firstPoint.x << "," << currentLine.firstPoint.y << ") and (" << currentLine.secondPoint.x << "," << currentLine.secondPoint.y <<
                        ") intersects with \nthe line between (" << testLine.firstPoint.x << "," << testLine.firstPoint.y << ") and (" << testLine.secondPoint.x << "," << testLine.secondPoint.y << ")" << endl;

                        double crossedLength, uncrossedLength;
                        crossedLength = hypot((currentLine.firstPoint.x - currentLine.secondPoint.x), (currentLine.firstPoint.y - currentLine.secondPoint.y)) + hypot((testLine.firstPoint.x - testLine.secondPoint.x), (testLine.firstPoint.y - testLine.secondPoint.y));
                        uncrossedLength = hypot((currentLine.firstPoint.x - testLine.firstPoint.x), (currentLine.firstPoint.y - testLine.firstPoint.y)) + hypot((currentLine.secondPoint.x - testLine.secondPoint.x), (currentLine.secondPoint.y - testLine.secondPoint.y));
                        if (uncrossedLength < crossedLength)
                        {
                            point tempPoint;
                            for (int f = p+1, g = i; f < g; f++, g--)
                            {
                                cout << "exchanging point " << f << " and point " << g << endl;
                                tempPoint = pointArray[f];
                                pointArray[f] = pointArray[g];
                                pointArray[g] = tempPoint;
                            }
                            intersected = true;
                        }
                    }
                    else
                    {
                        //cout << "It doesn't" << endl;
                    }
                }
            }
        }
    }
    while (intersected);
}
