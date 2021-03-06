/*
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
 *   ROS package name: cob_obstacle_distance
 *
 * \author
 *   Author: Marco Bezzon, email: Marco.Bezzon@ipa.fraunhofer.de
 *
 * \date Date of creation: May, 2015
 *
 * \brief
 *   This header contains the declaration and template implementation of MarkerShapes
 *   which represent a combination of ROS markers and FCL geometric shapes.
 *
 ****************************************************************/

#ifndef MARKER_SHAPES_IMPL_HPP_
#define MARKER_SHAPES_IMPL_HPP_

#include "cob_obstacle_distance/marker_shapes.hpp"

/* BEGIN MarkerShape ********************************************************************************************/
template <typename T>
MarkerShape<T>::MarkerShape(T &fcl_object,
      double x, double y, double z,
      double quat_x, double quat_y, double quat_z, double quat_w,
      double color_r, double color_g, double color_b, double color_a) : fcl_marker_converter_(fcl_object), is_drawn_(false)
{
    this->init(x, y, z, quat_x, quat_y, quat_z, quat_w, color_r, color_g, color_b, color_a);
}


template <typename T>
MarkerShape<T>::MarkerShape(double x, double y, double z,
            double quat_x, double quat_y, double quat_z, double quat_w,
            double color_r, double color_g, double color_b, double color_a) : is_drawn_(false)
{
    this->init(x, y, z, quat_x, quat_y, quat_z, quat_w, color_r, color_g, color_b, color_a);
}



template <typename T>
void MarkerShape<T>::init(double x, double y, double z,
          double quat_x, double quat_y, double quat_z, double quat_w,
          double color_r, double color_g, double color_b, double color_a)
{
    marker_.pose.position.x = x;
    marker_.pose.position.y = y;
    marker_.pose.position.z = z;
    marker_.pose.orientation.x = quat_x;
    marker_.pose.orientation.y = quat_y;
    marker_.pose.orientation.z = quat_z;
    marker_.pose.orientation.w = quat_w;

    marker_.color.r = color_r;
    marker_.color.g = color_g;
    marker_.color.b = color_b;
    marker_.color.a = color_a;

    marker_.header.frame_id = g_frame_id;
    marker_.header.stamp = ros::Time::now();
    marker_.ns = g_marker_namespace;
    marker_.action = visualization_msgs::Marker::ADD;
    marker_.id = class_ctr_;

    fcl_marker_converter_.assignValues(marker_);
}


template <typename T>
inline uint32_t MarkerShape<T>::getId() const
{
    return this->marker_.id;
}


template <typename T>
inline void MarkerShape<T>::setColor(double color_r, double color_g, double color_b, double color_a)
{
    marker_.color.r = color_r;
    marker_.color.g = color_g;
    marker_.color.b = color_b;
    marker_.color.a = color_a;
}


template <typename T>
inline visualization_msgs::Marker MarkerShape<T>::getMarker()
{
    this->marker_.header.stamp = ros::Time::now();
    return this->marker_;
}


template <typename T>
inline void MarkerShape<T>::setDrawn()
{
    this->is_drawn_ = true;
}


template <typename T>
inline bool MarkerShape<T>::isDrawn() const
{
    return this->is_drawn_;
}


template <typename T>
fcl::CollisionObject MarkerShape<T>::getCollisionObject() const
{
    fcl::Transform3f x(fcl::Quaternion3f(this->marker_.pose.orientation.w,
                                         this->marker_.pose.orientation.x,
                                         this->marker_.pose.orientation.y,
                                         this->marker_.pose.orientation.z),
                       fcl::Vec3f(this->marker_.pose.position.x,
                                  this->marker_.pose.position.y,
                                  this->marker_.pose.position.z));

    T geoShape = fcl_marker_converter_.getGeoShape();
    fcl::CollisionObject cobj(boost::shared_ptr<fcl::CollisionGeometry>(new T(geoShape)), x);
    return cobj;
}

/* END MarkerShape **********************************************************************************************/


#endif /* MARKER_SHAPES_IMPL_HPP_ */
