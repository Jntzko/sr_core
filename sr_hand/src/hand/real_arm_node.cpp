/**
* @file   real_arm_node.cpp
* @author Ugo Cupcic <ugo@shadowrobot.com>, Contact <contact@shadowrobot.com>
* @date   Tue Jun 29 14:56:10 2010
*
*
* Copyright 2011 Shadow Robot Company Ltd.
*
* This program is free software: you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* @brief Contains the main for the real arm. We start the publishers / subscribers in this node.
* They all share the same RealArm object, this way the subscriber can update the arm properties,
* while the publishers publish up to date data. The diagnostics and the other publisher are started
* in two different threads, to allow them to be published at different frequencies.
*
*
*/

#include <ros/ros.h>

#include <boost/thread/thread.hpp>
#include <boost/smart_ptr.hpp>

#include "sr_hand/sr_subscriber.h"
#include "sr_hand/sr_publisher.h"
#include "sr_hand/sr_diagnosticer.h"
#include "sr_hand/hand/real_arm.h"

using ros::NodeHandle;
using ros::ok;
using shadowrobot::RealArm;
using shadowrobot::SRDiagnosticer;
using shadowrobot::SRSubscriber;
using shadowrobot::SRPublisher;
using shadowrobot::sr_arm_hardware;

/////////////////////////////////
//           MAIN              //
/////////////////////////////////

void run_diagnotics(boost::shared_ptr <SRDiagnosticer> shadowhand_diag)
{
  while (ok())
  {
    shadowhand_diag->publish();
  }
}

void run_publisher(boost::shared_ptr <SRPublisher> shadowhand_pub)
{
  while (ok())
  {
    shadowhand_pub->publish();
  }
}


/**
* The main function initialises this ROS subscriber and sets the different callbacks.
* This ROS subscriber will listen for new commands and send them to
* the real robot.
*
* @param argc
* @param argv
*
* @return 0 on success
*/
int main(int argc, char **argv)
{
  ros::init(argc, argv, "shadowarm");
  NodeHandle n;

  boost::shared_ptr <RealArm> real_arm(new RealArm());
  boost::shared_ptr <SRSubscriber> shadowhand_subscriber(new SRSubscriber(real_arm));

  boost::shared_ptr <SRPublisher> shadowhand_pub(new SRPublisher(real_arm));
  boost::shared_ptr <SRDiagnosticer> shadowhand_diag(new SRDiagnosticer(real_arm, sr_arm_hardware));

  boost::thread
  thrd1(boost::bind(&run_diagnotics, shadowhand_diag));
  boost::thread
  thrd2(boost::bind(&run_publisher, shadowhand_pub));
  thrd1.join();
  thrd2.join();

  return 0;
}
