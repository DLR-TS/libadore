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
 *   Daniel Heß - initial API and implementation
 ********************************************************************************/


#pragma once
#include "vehiclestate10d.h"

namespace adore
{
	namespace env
	{
		/**
		 * @brief Class to observe the vehicle state
		 * 
		 */
		class VehicleStateObservation
		{
		public:
			VehicleState10d x; /**< vehicle state */
			double t; /**< time */
			int actLeverPos; /**< lever position */
		};
	}
}