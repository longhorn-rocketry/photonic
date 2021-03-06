/**
 *                                 [PHOTIC]
 *                                  v3.2.0
 *
 * This file is part of Photic, a collection of utilities for writing high-power
 * rocket flight computer software. Developed in Austin, TX by the Longhorn
 * Rocketry Association at the University of Texas at Austin.
 */

#include "RocketTracker.hpp"
#include "History.hpp"
#include "MathUtils.hpp"

namespace Photic
{

/***************************** PUBLIC FUNCTIONS *******************************/

RocketTracker::Config_t RocketTracker::getDefaultConfig ()
{
    static RocketTracker::Config_t defaultConfig =
    {
        nullptr, // IMU interface provided by user.
        nullptr, // Barometer interface provided by user.
        0.1,     // Timestep in seconds.
        2,       // Idx used by Adafruit, but user may be using a diff IMU.
        50       // Kalman gain calculation iterations. Based on LRA experience.
    };

    return defaultConfig;
}

RocketTracker::RocketTracker (const Config_t& kConfig) :
    mPImu (kConfig.pImu),
    mPBarometer (kConfig.pBarometer),
    mVertAccelIdx (kConfig.vertAccelIdx)
{
    // Estimate the launchpad altitude and variance in the rocket's IMU and
    // barometer readings.
    Real_t baroVar = 0;
    Real_t imuVar = 0;
    this->profileSensors (baroVar, imuVar, mLpAltitude);

    // Configure the Kalman filter.
    mKf.setDeltaT (kConfig.dt);
    mKf.setInitialState (mLpAltitude, 0, 0);
    mKf.setSensorVariance (baroVar, imuVar);
    mKf.computeKg (kConfig.kgIterations);
}

Vector3_t RocketTracker::track (const bool kRunSensors)
{
    // Get most recent sensor data.
    if (kRunSensors)
    {
        mPImu->run ();
        mPBarometer->run ();
    }

    // Compute vertical acceleration relative to the Earth.
    Vector4_t quatOrient = mPImu->getQuaternionOrientation ();
    Vector3_t vecAccelRocket = mPImu->getAccelerationVector ();
    Vector3_t vecAccelWorld = MathUtils::rotateVector (quatOrient, vecAccelRocket);
    Real_t accelVertical = vecAccelWorld[mVertAccelIdx];

    // Get altitude estimate from barometer.
    Real_t altitude = mPBarometer->getAltitude ();

    // Floor estimated altitude at the launchpad altitude. Large drops in
    // measured altitude have been observed at liftoff during previous launches,
    // likely due to the mass of inert air in the avionics bay rushing into
    // the barometer.
    altitude = altitude < mLpAltitude ? mLpAltitude : altitude;

    // Filter new state.
    return mKf.filter (altitude, accelVertical);
}

/***************************** PRIVATE FUNCTIONS ******************************/

void RocketTracker::profileSensors (Real_t& kBaroVarRet, Real_t& kImuVarRet,
                                    Real_t& kLpAltRet) const
{
    // Number of readings in sensor variance sample.
    static constexpr uint32_t numSamples = 1000;

    // Estimate the barometer's altitude measurement variance.
    History<numSamples> altitudeReadings;
    while (!altitudeReadings.atCapacity ())
    {
        mPBarometer->run ();
        altitudeReadings.add (mPBarometer->getAltitude ());
    }
    Real_t baroStdev = altitudeReadings.getStdev ();
    kBaroVarRet = baroStdev * baroStdev;

    // Estimate launchpad altitude as the average barometer altitude reading.
    kLpAltRet = altitudeReadings.getMean ();

    // Estimate the IMU's acceleration measurement variance.
    History<numSamples> accelReadings;
    while (!accelReadings.atCapacity ())
    {
        mPImu->run ();
        Real_t vertAccel = (mPImu->getAccelerationVectorPtr ())[mVertAccelIdx];
        accelReadings.add (vertAccel);
    }
    Real_t imuStdev = accelReadings.getStdev ();
    kImuVarRet = imuStdev * imuStdev;
}

} // namespace Photic