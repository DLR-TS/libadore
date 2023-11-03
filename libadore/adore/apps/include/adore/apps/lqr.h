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
//#include <mcm_dmove/MCM.h>


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
            adore::env::ThreeLaneViewDecoupled three_lanes_;/**<lane-based representation of environment*/
            ros::NodeHandle* nh_;
            ros::Subscriber Vehicle1V2XMCMPredictionSubscriber;  
            double x_fm;
            double y_fm;
            double vx_fm;
            std::ofstream myfile;
            double ego_s,ego_d;
            double leading_s, leading_d;
            //OVM model parameters
            const double h_star = 20.0;
            const double v_star = 20.0;
            const double n_start = 1.570;
            const double alpha = 2;
            const double beta = 2;
            double h_telda;
            double v_telda;
            double v_telda_leader;

            public:
        LQR(ros::NodeHandle* nh_)
        {   
            x_reader_ = adore::fun::FunFactoryInstance::get()->getVehicleMotionStateReader();   
            this->nh_= nh_;    
            nh_ = new ros::NodeHandle();
            Vehicle1V2XMCMPredictionSubscriber= nh_->subscribe("v2x/MCM_Prediction",1,&LQR::receive_mcm,this);
            const char* filename = "example.txt";
            
            if (std::ifstream(filename)) {
                // File exists, so delete it
                (std::remove(filename));
                std::cout << "File deleted successfully." << std::endl;
            } 
   
            //Vehicle1V2XMCMPredictionSubscriber= nh_->subscribe("v2x/MCM_Prediction",1,&LQR::receive_mcm,this); 
        }

        void run(){
            
            adore::fun::VehicleMotionState9d x;
            if(!x_reader_->hasData())return;
            x_reader_->getData(x);
            three_lanes_.update();
            auto current = three_lanes_.getCurrentLane(); 
            auto currentTraffic = current->getOnLaneTraffic(); // other objects detected via sensors
            auto ego_x = x.getX();
            auto ego_y = x.getY();
            auto ego_vx = x.getvx();
            ros::Time current_time = ros::Time::now();

            this->myfile.open ("example.txt",std::ios::app);

            // compute the ego vehicle states 
            this->h_telda = (this->leading_s - this->ego_s)-this->h_star;
            this->v_telda = ego_vx - this->v_star;
            
            // compute the v_leader_telda
            this->v_telda_leader = this-> vx_fm - this->v_star;
        
            //double ego_s, egp_d;
            if( three_lanes_.getCurrentLane()!=nullptr   &&  three_lanes_.getCurrentLane()->isValid()){
            
                three_lanes_.getCurrentLane()->toRelativeCoordinates(ego_x, ego_y, this->ego_s, this->ego_d);
                this->myfile << "Ego vehicle      v = "<<ego_vx<<", x = "<<ego_x<<", y =  "<<ego_y<<" raduis " << sqrt(ego_x*ego_x+ego_y*ego_y)<<", s = "<<this->ego_s<<", states are h_telda"<<this->h_telda<<", v_telda" <<this->v_telda<<"\n"; 
                std::cout<<"Time "<<current_time<<", Ego vehicle      v = "<<ego_vx<<", x = "<<ego_x<<", y =  "<<ego_y<<" raduis " << sqrt(ego_x*ego_x+ego_y*ego_y)<<", s = "<<this->ego_s<<"\n";   
            }

            this->myfile.close();

            // The discrete time state space model is as the following 
            // Ad = [0.9862   -0.0820;0.2576    0.6583]*[h_telda;v_telda]
            // Bd = [-0.0044;0.0820] * u
            // Bd_1 = [.0908;.1778] * v_telda_leader

            

        }

        void receive_mcm(const adore_if_ros_msg::CooperativePlanning cp){
            ros::Time current_time = ros::Time::now();
            double raduis = sqrt(pow(cp.prediction[0].x,2)+pow(cp.prediction[0].y,2));
            this-> vx_fm = cp.prediction[0].v;
            if(raduis > 55){
                //std::cout<<"  raduis is greater than limits "<<raduis<<"\n";
                this->x_fm = cp.prediction[0].x;
                this->y_fm = -sqrt(pow(52.0613,2) - pow(cp.prediction[0].x,2));
            }
            else{
                //std::cout<<"  raduis is less than limits "<<raduis<<"\n";
                this->x_fm = cp.prediction[0].x;
                this->y_fm = cp.prediction[0].y;
            }

            this->myfile.open ("example.txt",std::ios::app);
            
            three_lanes_.update();
            auto current = three_lanes_.getCurrentLane(); 
            auto currentTraffic = current->getOnLaneTraffic(); 
            
            //double leading_s, leading_d;
            if(three_lanes_.getCurrentLane()!=nullptr   &&  three_lanes_.getCurrentLane()->isValid() ){

                three_lanes_.getCurrentLane()->toRelativeCoordinates(this->x_fm, this->y_fm, this->leading_s, this->leading_d);
                this->myfile <<"Time "<<current_time<< ",Leading vehicle  v = "<<this-> vx_fm <<", x = "<<this->x_fm<<", y =  "<<this->y_fm<<" raduis " << sqrt(this->x_fm*this->x_fm+this->y_fm*this->y_fm)<<" , s = "<<this->leading_s<<"\n";   
                std::cout<<"Time "<<current_time<<" Leading vehicle  v = "<<this-> vx_fm <<", x = "<<this->x_fm<<", y =  "<<this->y_fm<<" raduis " << sqrt(this->x_fm*this->x_fm+this->y_fm*this->y_fm)<<" , s = "<<this->leading_s<<"\n";   
            }

            this->myfile.close();

            }

        };
    }
}