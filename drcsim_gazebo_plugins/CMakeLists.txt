cmake_minimum_required(VERSION 2.8.3)
project(drcsim_gazebo_plugins)
# set(CMAKE_BUILD_TYPE "Debug")

find_package(catkin) # REQUIRED COMPONENTS nothing)
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES drcsim_gazebo_plugins_DRCVehiclePlugin
            drcsim_gazebo_plugins_DRCBuildingPlugin
)

# Depend on system install of Gazebo and Boost
find_package(gazebo REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread)

link_directories(
  ${GAZEBO_LIBRARY_DIRS}
)

include_directories(
  ${GAZEBO_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include  #hm, is this ok?
)

# compile and install gazebo plugins
add_library(drcsim_gazebo_plugins_DRCVehiclePlugin SHARED src/DRCVehiclePlugin.cc)
target_link_libraries(drcsim_gazebo_plugins_DRCVehiclePlugin ${GAZEBO_LIBRARIES})
install (TARGETS drcsim_gazebo_plugins_DRCVehiclePlugin DESTINATION ${CMAKE_INSTALL_PREFIX}/${CATKIN_PACKAGE_LIB_DESTINATION}/)

add_library(drcsim_gazebo_plugins_DRCBuildingPlugin SHARED src/DRCBuildingPlugin.cc)
target_link_libraries(drcsim_gazebo_plugins_DRCBuildingPlugin ${GAZEBO_LIBRARIES})
install (TARGETS drcsim_gazebo_plugins_DRCBuildingPlugin DESTINATION ${CMAKE_INSTALL_PREFIX}/${CATKIN_PACKAGE_LIB_DESTINATION}/)

## Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.hh"
)
