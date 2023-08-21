/********************************************************************************
 * Copyright (C) 2017-2023 German Aerospace Center (DLR).
 * Eclipse ADORe, Automated Driving Open Research https://eclipse.org/adore
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Matthias Nichting - initial API and implementation
 ********************************************************************************/

#pragma once

namespace adore
{
    namespace sim
    {
        /**
         * @brief provides encapsulation of values needed to reset the vehicle dimensions in a simulation
         *
         */
        struct ResetVehicleDimensions
        {
          private:
            double a_, b_, c_, d_, m_, bodyWidth_;
            bool a_valid_, b_valid_, c_valid_, d_valid_, m_valid_, bodyWidth_valid_;

          public:
            ResetVehicleDimensions()
              : a_valid_(false)
              , b_valid_(false)
              , c_valid_(false)
              , d_valid_(false)
              , m_valid_(false)
              , bodyWidth_valid_(false)
            {
            }

            bool get_length(double& val) const
            {
                bool result = a_valid_ && b_valid_ && c_valid_ && d_valid_;
                if (result)
                {
                    val = a_ + b_ + c_ + d_;
                }
                return result;
            }
            bool get_a(double& val) const
            {
                if (a_valid_)
                {
                    val = a_;
                }
                return a_valid_;
            }
            bool get_b(double& val) const
            {
                if (b_valid_)
                {
                    val = b_;
                }
                return b_valid_;
            }
            bool get_c(double& val) const
            {
                if (c_valid_)
                {
                    val = c_;
                }
                return c_valid_;
            }
            bool get_d(double& val) const
            {
                if (d_valid_)
                {
                    val = d_;
                }
                return d_valid_;
            }
            bool get_m(double& val) const
            {
                if (m_valid_)
                {
                    val = m_;
                }
                return m_valid_;
            }
            bool get_width(double& val) const
            {
                if (bodyWidth_valid_)
                {
                    val = bodyWidth_;
                }
                return bodyWidth_valid_;
            }
            void set_a(double val)
            {
                a_valid_ = true;
                a_ = val;
            }
            void set_b(double val)
            {
                b_valid_ = true;
                b_ = val;
            }
            void set_c(double val)
            {
                c_valid_ = true;
                c_ = val;
            }
            void set_d(double val)
            {
                d_valid_ = true;
                d_ = val;
            }
            void set_m(double val)
            {
                m_valid_ = true;
                m_ = val;
            }
            void set_width(double val)
            {
                bodyWidth_valid_ = true;
                bodyWidth_ = val;
            }
            void invalidate_a()
            {
                a_valid_=false;
            }
            void invalidate_b()
            {
                b_valid_=false;
            }
            void invalidate_c()
            {
                c_valid_=false;
            }
            void invalidate_d()
            {
                d_valid_=false;
            }
            void invalidate_m()
            {
                m_valid_=false;
            }
            void invalidate_width()
            {
                bodyWidth_valid_=false;
            }
        };

    }  // namespace sim
}  // namespace adore