/********************************************************************************
 * Copyright (C) 2017-2021 German Aerospace Center (DLR). 
 * Eclipse ADORe, Automated Driving Open Research https://eclipse.org/adore
 *
 * This program and the accompanying materials are made available under the 
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0 
 *
 * Contributors: 
 *   Reza Dariani
 *  Anas Abulehia
 ********************************************************************************/

#pragma once
#include <ros/ros.h>
#include <adore/env/afactory.h>
#include <adore/fun/afactory.h>
#include <adore/mad/adoremath.h>
#include <adore/params/afactory.h>
#include <adore/env/threelaneviewdecoupled.h>
#include <iostream>
#include <fstream>

#include<nlohmann/json.hpp>

using json = nlohmann::json;

#include <bits/stdc++.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
//#include <mcm_dmove/MCM.h>


#define QUOTE(...) #__VA_ARGS__


#define QUOTE(...) #__VA_ARGS__
const char *CONTRACTS_JSON_MESSAGE_FORMAT_STRING = QUOTE
({ 
  "Time":"%f",
   "velocity":"%f",
   "odometry":"%f"
  
  
});


namespace adore
{

    namespace apps
    {
        /**

         */
        class LQR
        {
            private:
            adore::mad::AReader<adore::fun::VehicleMotionState9d>* x_reader_;/**< compare vehicle position to checkpoint position */
            adore::env::ThreeLaneViewDecoupled three_lanes_1;/**<lane-based representation of environment*/
            adore::env::ThreeLaneViewDecoupled three_lanes_2;
            ros::NodeHandle* nh_;
            ros::Subscriber Vehicle1V2XMCMPredictionSubscriber;  
            double x_fm;
            double y_fm;
            double vx_fm;

            //OVM model parameters
            double ego_s,ego_d;
            double leading_s, leading_d;
            double current_time;
            const double h_star = 80;
            const double v_star = 10.0;
            const double n_start = 1.570;
            const double alpha = 2;
            const double beta = 2;
            double h_telda;
            double v_telda;
            double v_telda_leader;
            double h_telda_next;
            double v_telda_next;
            double h_,v_;
            double r, r_telda;
            double e,e_telda,e_telda_next;
            double desired_headway = 20.0;//desired headway distance (user defined) 

            //loging and streaming parameters 
            int udpSocket;
            std::ofstream myfile;

            public:
        LQR(ros::NodeHandle* nh_)
        {   
            x_reader_ = adore::fun::FunFactoryInstance::get()->getVehicleMotionStateReader();   
            this->nh_= nh_;    
            nh_ = new ros::NodeHandle();
            
            const char* filename0 = "ego_logs.csv";
            const char* filename1 = "leading_logs.csv";

            // create and initialize csv for logging purposes
            if (std::ifstream(filename0)) {
                // File exists, so delete it
                (std::remove(filename0));
                std::cout << "File deleted successfully." << std::endl;
            } 
            this->myfile.open ("ego_logs.csv");
            this->myfile <<"Time, Velocity, distance\n";
            this->myfile.close();

            if (std::ifstream(filename1)) {
                // File exists, so delete it
                (std::remove(filename1));
                std::cout << "File deleted successfully." << std::endl;
            } 
            this->myfile.open ("leading_logs.csv");
            this->myfile <<"Time, Velocity, distance\n";
            this->myfile.close();

            // create socket instance for loging and connectivty with other applications, i.e plot juggler
            this->udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
            if (udpSocket == -1) {
            std::cout << "Socket creation failed." << std::endl;
            }
            
            // Create a subscriber object to v2x/MCM_Prediction , it constains variables for the leading vehicle
            this->Vehicle1V2XMCMPredictionSubscriber= nh_->subscribe("v2x/MCM_Prediction",1,&LQR::receive_mcm,this);
        }

        void run(){
            // x is the object which contains ego vehicle states and other infromation
            adore::fun::VehicleMotionState9d x;
            if(!x_reader_->hasData())return;
            x_reader_->getData(x);
            three_lanes_1.update();
            auto current = three_lanes_1.getCurrentLane(); 
            auto currentTraffic = current->getOnLaneTraffic(); // other objects detected via sensors
            
            // read the x,y,v and time of the ego vehicle 
            auto ego_x = x.getX();
            auto ego_y = x.getY();
            auto ego_vx = x.getvx();
            current_time = x.getTime();

            //writes time, velocity, x,y positions and progress on the ego_logs.csv file and on cout stream
            this->myfile.open ("ego_logs.csv",std::ios::app);
            three_lanes_1.update();
            //double ego_s represents the distance or progress of the vehicle 
            if( three_lanes_1.getCurrentLane()!=nullptr   &&  three_lanes_1.getCurrentLane()->isValid()){
                three_lanes_1.getCurrentLane()->toRelativeCoordinates(ego_x, ego_y, this->ego_s, this->ego_d);
                this->myfile <<current_time<<" ,"<<ego_vx<<", "<<this->ego_s<<"\n"; 
                std::cout<<"Time "<<current_time<<", Ego vehicle      v = "<<ego_vx<<", x = "<<ego_x<<", y =  "<<ego_y<<", s = "<<this->ego_s<<"\n";   
            }
            this->myfile.close();

            // compute the ego vehicle states according to Optimal velocity model 
            this->h_telda = (this->leading_s - this->ego_s)-this->h_star;
            this->v_telda = ego_vx - this->v_star;
            // compute the v_leader_telda
            this->v_telda_leader = this-> vx_fm - this->v_star;

            //section 3.2.2 equation 3.25 
            // OVM open loop (discrete model) , u = 0;
            // The discrete time state space model is as the following , @ 10 hz rate 
            // Ad = |0.9862   -0.0820| x |h_telda|
            //      |0.2576    0.6583|   |v_telda|
            // Bd = |-0.0044| x u_telda
            //      |0.0820 | 
            // Bd_1 =| .0908| x v_telda_leader
            //       |.1778 |
            
            h_telda_next =  0.9862*h_telda + -0.0820 * v_telda + .0908*v_telda_leader;
            v_telda_next =  0.2576*h_telda + 0.6583 * v_telda + .1778*v_telda_leader;
            h_ = h_telda_next + h_star;
            v_ = v_telda_next + v_star;// to be sent to the vehicle.
            
            //section 4.2 equation 4.28 
            // LQR controller of OVM closed loop (discrete model), u = -B*K 
            // -|0| | -17.1036, 3.0857| =  
            //  |1|
            // A-BK = | 0       -1.0000|  - |0|  |-17.1036, 3.0857| = |   0      -1.0000|
            //        |3.1416   -4.0000|    |1|                       |20.2452   -7.0857|
            //
            // after discrtization of (A-BK) + Bw * v_telda_leader, @ 10 hz rate 
            // ACL = |0.920273791342097	-0.0692712380995241| |h_telda| + |0.0892988782004615| v_telda_leader 
            //       |1.40241006957249	  0.429438579540299| |v_telda|   |0.218268684856951 |
                     
            h_telda_next = 0.920273791342097*h_telda + -0.0692712380995241 * v_telda+.0892988782004615*v_telda_leader;
            v_telda_next =  1.40241006957249*h_telda + 0.429438579540299 * v_telda + .218268684856951*v_telda_leader;
            h_ = h_telda_next + h_star;
            //v_ = v_telda_next + v_star;// to be sent to the vehicle.

            //section 4.2.2 equation 4.32
            //LQR with an Error State 
            //|0        -1.0000    0||h_telda|   |0|          |1|                 |0|
            //|3.1416   -4.0000    0||v_telda| + |1|u_telda + |2|v_telda_leader + |0| r_telda
            //|-1.0000      0      0||e_telda|   |0|          |0|                 |1|
            //closed loop of LQR with an Error State 
            //A-BK = |0        -1.0000    0||h_telda|   |0||-16.7064  3.0365 1|   |0          -1.0000      0   |
            //       |3.1416   -4.0000    0||v_telda| - |1|                     = | 19.8480   -7.0365   -1.0000|
            //       |-1.0000      0      0||e_telda|   |0|                       |-1.0000      0          0   |
            //The system becomes 
            //|0          -1.0000      0   ||h_telda|   |1|                 |0|
            //| 19.8480   -7.0365   -1.0000||v_telda| + |2|v_telda_leader + |0| r_telda
            //|-1.0000      0          0   ||e_telda|   |0|                 |1|
            // after discretization it becomes 
            //| 0.9216  -0.06946  0.003945  ||h_telda|   |0.08933   |                 |0.0001397|
            //| 1.383    0.4328  -0.06946   ||v_telda| + |0.2174    |v_telda_leader + |-0.003945| r_telda
            //|-0.09722  0.003945    0.9999 ||e_telda|   |-0.004649 |                 |0.1      |
            r_telda = this->desired_headway - this->h_star;
            e_telda = r_telda - h_telda;
            h_telda_next = 0.9216*h_telda - 0.06946 *v_telda + 0.003945*e_telda + 0.08933 * v_telda_leader + 0.0001397 * r_telda;
            v_telda_next = 1.383 *h_telda +   0.4328 *v_telda - 0.06946 *e_telda + 0.2174 * v_telda_leader + -0.003945 * r_telda;
            e_telda_next = -0.09722 *h_telda +  0.003945 *v_telda + 0.9999*e_telda -0.004649 * v_telda_leader + 0.1 * r_telda;
            h_ = h_telda_next + h_star;
            //v_ = v_telda_next + v_star;// to be sent to the vehicle.

            // Server address configuration
            struct sockaddr_in serverAddress;
            memset(&serverAddress, 0, sizeof(serverAddress));
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_port = htons(8080); // Port number
            inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)); // Server IP address

            // Here, we stream the logs on Socket object as an json file
            nlohmann::json data1;
            data1["time"] = current_time;
            data1["velocity0"] = ego_vx;
            data1["distance0"] = this->ego_s;
            data1["velocity1"] = this-> vx_fm;
            data1["distance1"] = this->leading_s;
            data1["OVM_velocity"] = v_;

            // Serialize the JSON data to a string.
            std::string jsonStr = data1.dump();

            char newJsonStr[jsonStr.length() + 1];
            strcpy(newJsonStr, jsonStr.c_str());

            char car_Jason_buffer[600];
            sprintf(car_Jason_buffer, CONTRACTS_JSON_MESSAGE_FORMAT_STRING, current_time, ego_vx, this->ego_s);
            ssize_t bytesSent = sendto(this->udpSocket, newJsonStr, strlen(newJsonStr), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress));


        }

        void receive_mcm(const adore_if_ros_msg::CooperativePlanning cp){

            // reading the states of the leading vehicle 
            this-> vx_fm = cp.prediction[0].v;
            this->x_fm = cp.prediction[0].x;
            this->y_fm = cp.prediction[0].y;
            
            
            this->myfile.open ("leading_logs.csv",std::ios::app);
            auto current = three_lanes_2.getCurrentLane(); 
            auto currentTraffic = current->getOnLaneTraffic(); 
            
            //double leading_s, leading_d;
            three_lanes_2.update();
            if(three_lanes_2.getCurrentLane()!=nullptr   &&  three_lanes_2.getCurrentLane()->isValid() ){

                three_lanes_2.getCurrentLane()->toRelativeCoordinates(this->x_fm, this->y_fm, this->leading_s, this->leading_d);
                //this->myfile <<"Time "<<current_time<< ",Leading vehicle  v = "<<this-> vx_fm <<", x = "<<this->x_fm<<", y =  "<<this->y_fm<<" , s = "<<this->leading_s<<"\n";   
                this->myfile <<current_time<<" ,"<<this-> vx_fm <<", "<<this->leading_s<<"\n"; 
                std::cout<<"Time "<<current_time<<" ,Leading vehicle  v = "<<this-> vx_fm <<", x = "<<this->x_fm<<", y =  "<<this->y_fm<<" , s = "<<this->leading_s<<"\n";   
            }

            this->myfile.close();

            }

        };
    }
}