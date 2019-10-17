#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/TeleportAbsolute.h>
#include <turtlesim/SetPen.h>
#include <iostream>
#include <stdlib.h>
using namespace std;
int main(int argc, char *argv[]) {
  ros::init(argc, argv, "move_turtlesim");

  float box_size = ros::param::param("~box_size", 9.0);
  int width = 0;
  float offset;
  int R,G,B;


  srand(time(NULL));
  width = rand() % 100 + 10;
  cout << width << endl;
  srand(time(NULL));
  offset = rand()% 10 + 0;
  srand(time(NULL));
  R = rand()% 255 + 0;
  srand(time(NULL));
  G = rand()% 255 + 0;
  srand(time(NULL));
  B = rand()% 255 + 0;
  cout << R << " "<<  G <<" " <<  B << endl;


  ros::NodeHandle nh;

  ros::service::waitForService("/turtle1/teleport_absolute", -1);

  ros::ServiceClient teleport_client = nh.serviceClient<turtlesim::TeleportAbsolute>("/turtle1/teleport_absolute");
  ros::ServiceClient pen_client = nh.serviceClient<turtlesim::SetPen>("/turtle1/set_pen");


  turtlesim::SetPen pen_srv;
  pen_srv.request.off = true;
  pen_client.call(pen_srv);

  turtlesim::TeleportAbsolute srv;

  srv.request.x = offsetX -box_size/2;
  srv.request.y = offsetY-box_size/2;
  teleport_client.call(srv);

  pen_srv.request.off = false;
  pen_srv.request.width = width;
  pen_srv.request.r = R;
  pen_srv.request.g = G;
  pen_client.call(pen_srv);

  srv.request.x = offset -box_size/2;
  srv.request.y = offset +box_size/2;
  teleport_client.call(srv);

  srv.request.x = offset +box_size/2;
  srv.request.y = offset +box_size/2;
  teleport_client.call(srv);

  srv.request.x = offset +box_size/2;
  srv.request.y = offset -box_size/2;
  teleport_client.call(srv);

  srv.request.x = offset -box_size/2;
  srv.request.y = offset -box_size/2;
  teleport_client.call(srv);

  pen_srv.request.off = true;
  pen_client.call(pen_srv);

  srv.request.x = offset;
  srv.request.y = offset;
  teleport_client.call(srv);

  pen_srv.request.off = false;
  pen_srv.request.width = width;
  pen_srv.request.r = R;
  pen_srv.request.g = G;
  pen_srv.request.b = B;
  pen_client.call(pen_srv);

  return 0;
}