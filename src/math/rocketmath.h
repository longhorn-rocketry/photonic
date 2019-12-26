/**
 * Various mathematical utilities and formulas relevant to high-power rocketry.
 */

#ifndef PHOTIC_MATH_ROCKETMATH_H
#define PHOTIC_MATH_ROCKETMATH_H

#include <math.h>

namespace photic
{

/**
 * Gets if two numbers are approximately equal.
 *
 * @param   k_a LHS.
 * @param   k_b RHS.
 *
 * @ret     True if k_a is approximately k_b, false otherwise.
 */
bool fapprox(float k_a, float k_b, float k_negligence=1e-6);

/**
 * The hypsometric formula, used to compute GL altitude when under 11 km.
 *
 * @param   k_p0 Initial pressure, likely on launchpad (kP).
 * @param   k_p  Current pressure (kP).
 * @param   k_t  Current temperature (C).
 *
 * @ret     Approximate GL altitude (m).
 */
float hypso(float k_p0, float k_p, float k_t);

/**
 * Computes the density of a gas according to the Ideal Gas Law.
 *
 * @param   k_p Pressure (P).
 * @param   k_t Temperature (K).
 *
 * @ret     Density (moles/m^3).
 */
float igl_density(float k_p, float k_t);

/**
 * Linear interpolation between two values.
 *
 * @param   k_lower Lower value.
 * @param   k_upper Upper value.
 * @param   k_p     Interpolation percentage, probability between 0 and 1.
 *
 * @ret     Interpolated value.
 */
template <typename T>
T lerp(T k_lower, T k_upper, double k_p)
{
    return k_lower + (k_upper - k_lower) * k_p;
}

} // namespace photic

#endif
