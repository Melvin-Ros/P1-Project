cmake_minimum_required(VERSION 2.8.3)
project(turtlesim_games)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

## Find catkin macros and libraries
find_package(catkin REQUIRED COMPONENTS
  roscpp
  turtlesim
)

###################################
## catkin specific configuration ##
###################################
catkin_package(
)

###########
## Build ##
###########
include_directories(
  ${catkin_INCLUDE_DIRS}
)

# Declare Kill Turtles C++ executable and rename to short name (in case catkin_make is used).
add_executable(${PROJECT_NAME}_kill_turtles src/kill_turtles.cpp)
set_target_properties(${PROJECT_NAME}_kill_turtles PROPERTIES OUTPUT_NAME kill_turtles PREFIX "")
# Link to Catkin libraries
target_link_libraries(${PROJECT_NAME}_kill_turtles
  ${catkin_LIBRARIES}
)

# Vacuum Executable
add_executable(${PROJECT_NAME}_vacuum src/vacuum.cpp)
set_target_properties(${PROJECT_NAME}_vacuum PROPERTIES OUTPUT_NAME vacuum PREFIX "")
target_link_libraries(${PROJECT_NAME}_vacuum
  ${catkin_LIBRARIES}
)

# Make Box Executable
add_executable(${PROJECT_NAME}_make_box src/make_box.cpp)
set_target_properties(${PROJECT_NAME}_make_box PROPERTIES OUTPUT_NAME make_box PREFIX "")
target_link_libraries(${PROJECT_NAME}_make_box
  ${catkin_LIBRARIES}
)

#############
## Install ##
#############

# Mark executables for installation
install(TARGETS
  ${PROJECT_NAME}_kill_turtles
  ${PROJECT_NAME}_vacuum
  ${PROJECT_NAME}_make_box
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
