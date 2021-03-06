/**
 *                                 [PHOTIC]
 *                                  v3.2.0
 *
 * This file is part of Photic, a collection of utilities for writing high-power
 * rocket flight computer software. Developed in Austin, TX by the Longhorn
 * Rocketry Association at the University of Texas at Austin.
 */

#include "KalmanFilter.hpp"
#include "MathUtils.hpp"

namespace Photic
{

/***************************** PUBLIC FUNCTIONS *******************************/

KalmanFilter::KalmanFilter ()
{
    // State transition matrix is initially the identity. The time-variant
    // elements which do the transition are set in setDeltaT.
    mA = MathUtils::makeMatrix3 (1, 0, 0,
                                 0, 1, 0,
                                 0, 0, 1);

    // Process noise covariance is always 0. This is currently unused.
    mQ.fill (0);

    // State -> observation map looks like [1 0 0
    //                                      0 0 1]
    mH.fill (0);
    mH (0, 0) = 1;
    mH (1, 2) = 1;

    // Measurement noise covariance is initially 0. The elements on its
    // diagonal are set in setSensorVariance. This actually makes it a variance
    // matrix (observations of different state variables are not expected to
    // co-vary).
    mR.fill (0);

    // Error covariance is initially the identity. This is computed
    // side-by-side with the Kalman gain in computeKg.
    mP = MathUtils::makeMatrix3 (1, 0, 0,
                                 0, 1, 0,
                                 0, 0, 1);
}

void KalmanFilter::setDeltaT (const Real_t kDt)
{
    mA (0, 1) = kDt;
    mA (0, 2) = 0.5 * kDt * kDt;
    mA (1, 2) = kDt;
}

void KalmanFilter::setSensorVariance (const Real_t kAltVar,
                                      const Real_t kAccelVar)
{
    mR (0, 0) = kAltVar;
    mR (1, 1) = kAccelVar;
}

void KalmanFilter::setInitialState (const Real_t kAlt, const Real_t kVel,
                                    const Real_t kAccel)
{
    mE = MathUtils::makeVector3 (kAlt, kVel, kAccel);
}

void KalmanFilter::computeKg (const uint32_t kIterations)
{
    mP = MathUtils::makeMatrix3 (1, 0, 0,
                                 0, 1, 0,
                                 0, 0, 1);
    for (uint32_t i = 0; i < kIterations; i++)
    {
        this->computeKg ();
    }
}

Vector3_t KalmanFilter::filter (const Real_t kAlt, const Real_t kAccel)
{
    Vector2_t observation = MathUtils::makeVector2 (kAlt, kAccel);
    Vector3_t estNew = mA * mE;
    Vector3_t estNewF = estNew + mK * (observation - mH * estNew);
    mE = estNewF;
    return mE;
}

/***************************** PRIVATE FUNCTIONS ******************************/

void KalmanFilter::computeKg ()
{
    Matrix<2, 2> x = mH * mP * mH.transpose () + mR;
    mK = mP * mH.transpose () * MathUtils::invertMatrix2 (x);
    Matrix<3, 3> i = MathUtils::makeMatrix3 (1, 0, 0,
                                             0, 1, 0,
                                             0, 0, 1);
    mP = (i - mK * mH) * mP;
    mP = mA * mP * mA.transpose () + mQ;
}

} // namespace Photic