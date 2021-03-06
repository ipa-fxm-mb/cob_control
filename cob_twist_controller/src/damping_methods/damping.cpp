/*!
 *****************************************************************
 * \file
 *
 * \note
 *   Copyright (c) 2015 \n
 *   Fraunhofer Institute for Manufacturing Engineering
 *   and Automation (IPA) \n\n
 *
 *****************************************************************
 *
 * \note
 *   Project name: care-o-bot
 * \note
 *   ROS stack name: cob_control
 * \note
 *   ROS package name: cob_twist_controller
 *
 * \author
 *   Author: Marco Bezzon, email: Marco.Bezzon@ipa.fraunhofer.de
 *
 * \date Date of creation: March, 2015
 *
 * \brief
 *   This module contains the implementation of all available
 *   damping methods.
 *
 ****************************************************************/
#include "ros/ros.h"
#include "cob_twist_controller/damping_methods/damping.h"

/* BEGIN DampingBuilder *****************************************************************************************/
/**
 * Static builder method to create damping methods dependent on parameterization.
 */
DampingBase* DampingBuilder::create_damping(InvDiffKinSolverParams &params, t_Matrix6Xd &jacobian_data)
{
    DampingBase *db = NULL;
    switch(params.damping_method)
    {
        case NONE:
            db = new DampingNone(params, jacobian_data);
            break;
        case CONSTANT:
            db = new DampingConstant(params, jacobian_data);
            break;
        case MANIPULABILITY:
            db = new DampingManipulability(params, jacobian_data);
            break;
        case LSV:
            db = new DampingLeastSingularValues(params, jacobian_data);
            break;
        default:
            ROS_ERROR("DampingMethod %d not defined! Aborting!", params.damping_method);
            break;
    }

    return db;
}
/* END DampingBuilder *******************************************************************************************/

/* BEGIN DampingNone ********************************************************************************************/
/**
 * Method just returns the damping factor from ros parameter server.
 */
inline double DampingNone::get_damping_factor(const Eigen::VectorXd &sorted_singular_values) const
{
    return 0.0;
}

/* END DampingNone **********************************************************************************************/

/* BEGIN DampingConstant ****************************************************************************************/
/**
 * Method just returns the damping factor from ros parameter server.
 */
inline double DampingConstant::get_damping_factor(const Eigen::VectorXd &sorted_singular_values) const
{
    return this->params_.damping_factor;
}
/* END DampingConstant ******************************************************************************************/

/* BEGIN DampingManipulability **********************************************************************************/
/**
 * Method returns the damping factor according to the manipulability measure.
 * [Nakamura, "Advanced Robotics Redundancy and Optimization", ISBN: 0-201-15198-7, Page 268]
 */
double DampingManipulability::get_damping_factor(const Eigen::VectorXd &sorted_singular_values) const
{
    double w_threshold = this->params_.w_threshold;
    double lambda_max = this->params_.lambda_max;
    Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic> prod = this->jacobian_data_ * this->jacobian_data_.transpose();
    double d = prod.determinant();
    double w = std::sqrt(std::abs(d));
    double damping_factor;
    if (w < w_threshold)
    {
        double tmp_w = (1 - w / w_threshold);
        damping_factor = lambda_max * tmp_w * tmp_w;
    }
    else
    {
        damping_factor = 0.0;
    }

    return damping_factor;
}

/* END DampingManipulability ************************************************************************************/

/* BEGIN DampingLeastSingularValues **********************************************************************************/

double DampingLeastSingularValues::get_damping_factor(const Eigen::VectorXd &sorted_singular_values) const
{
    // Formula 15 Singularity-robust Task-priority Redundandancy Resolution
    if((double)sorted_singular_values(sorted_singular_values.rows()-1) < this->params_.eps_damping)
    {
        return sqrt( (1-pow((double)sorted_singular_values(sorted_singular_values.rows()-1)/this->params_.eps_damping,2)) * pow(this->params_.lambda_max,2) );
    }else
    {
        return 0;
    }
}
/* END DampingLeastSingularValues ************************************************************************************/

