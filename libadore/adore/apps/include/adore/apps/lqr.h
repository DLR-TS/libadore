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

#include <adore/env/afactory.h>
#include <adore/fun/afactory.h>
#include <adore/mad/adoremath.h>
#include <adore/params/afactory.h>
#include <adore/env/threelaneviewdecoupled.h>
#include <iostream>


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
            public:
        LQR()
        {   
            x_reader_ = adore::fun::FunFactoryInstance::get()->getVehicleMotionStateReader();            
        }

        void run()
        {
            std::cout<<"\nHELLO";
        adore::fun::VehicleMotionState9d x;
        if(!x_reader_->hasData())return;
        x_reader_->getData(x);
        three_lanes_.update();
        auto current = three_lanes_.getCurrentLane(); 
        auto currentTraffic = current->getOnLaneTraffic() ; // other objects detected via sensors
        auto ego_x = x.getX();
        auto ego_y = x.getY();
        auto ego_vx = x.getvx();
        double ego_s, egp_d;
        if( three_lanes_.getCurrentLane()!=nullptr   &&  three_lanes_.getCurrentLane()->isValid() )
        {
        three_lanes_.getCurrentLane()->toRelativeCoordinates(ego_x, ego_y, ego_s, egp_d);

        }
        
        }
               

        };
    }
}