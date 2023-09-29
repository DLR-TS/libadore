/********************************************************************************
 * Copyright (C) 2017-2020 German Aerospace Center (DLR). 
 * Eclipse ADORe, Automated Driving Open Research https://eclipse.org/adore
 *
 * This program and the accompanying materials are made available under the 
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0 
 *
 * Contributors: 
 *    Reza Dariani- initial implementation and API
 *    Anas Abulehia
 ********************************************************************************/
#pragma once

#include <adore/env/afactory.h>

namespace adore
{
namespace apps
{
    /**

     */
    class LQRPlatoonController
    {
        private:

        adore::env::AFactory::TVehicleMotionStateReader* egoStateReader_;

        
        public:
        LQRPlatoonController():

        {
            egoStateReader_ = adore::env::EnvFactoryInstance::get()->getVehicleMotionStateReader();

        }
        void update()
        {
            adore::env::VehicleMotionState9d x;
            egoStateReader_->getData(x);

        }
    };
}
}

