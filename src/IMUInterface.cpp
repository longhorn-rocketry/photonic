/**
 *                                 [PHOTIC]
 *                                  v3.2.0
 *
 * This file is part of Photic, a collection of utilities for writing high-power
 * rocket flight computer software. Developed in Austin, TX by the Longhorn
 * Rocketry Association at the University of Texas at Austin.
 */

#include "IMUInterface.hpp"

namespace Photic
{

IMUInterface::IMUInterface () {}

Vector3_t IMUInterface::getAccelerationVector () const
{
    return mData.vecAccel;
}

Vector3_t IMUInterface::getMagneticVector () const
{
    return mData.vecMag;
}

Vector3_t IMUInterface::getEulerOrientation () const
{
    return mData.orientEuler;
}

Vector4_t IMUInterface::getQuaternionOrientation () const
{
    return mData.orientQuat;
}

Real_t* IMUInterface::getAccelerationVectorPtr ()
{
    return &mData.vecAccel[0];
}

Real_t* IMUInterface::getMagneticVectorPtr ()
{
    return &mData.vecMag[0];
}

Real_t* IMUInterface::getEulerOrientationPtr ()
{
    return &mData.orientEuler[0];
}

Real_t* IMUInterface::getQuaternionOrientationPtr ()
{
    return &mData.orientQuat[0];
}

} // namespace Photic