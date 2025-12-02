/**
 * @file math.h
 * @brief Math functions for freestanding environment
 * 
 * Provides basic math operations including precomputed trigonometry tables
 * for fast sin/cos lookups without FPU-heavy calculations.
 * 
 * @author Mustafa Alotbah
 * @copyright myOS Project
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// BASIC MATH FUNCTIONS
// ============================================================================

/**
 * @brief Returns the absolute value of an integer
 */
int abs(int x);

/**
 * @brief Returns the absolute value of a double
 */
double fabs(double x);

/**
 * @brief Returns the smaller of two integers
 */
int min(int a, int b);

/**
 * @brief Returns the larger of two integers
 */
int max(int a, int b);

/**
 * @brief Clamps an integer to a range
 */
int clamp(int value, int min_val, int max_val);

// ============================================================================
// TRIGONOMETRY (Lookup Table Based)
// ============================================================================

#define TRIG_TABLE_SIZE 360

/**
 * @brief Precomputed sine table (360 entries, one per degree)
 */
extern const double sin_table[TRIG_TABLE_SIZE];

/**
 * @brief Precomputed cosine table (360 entries, one per degree)
 */
extern const double cos_table[TRIG_TABLE_SIZE];

/**
 * @brief Fast sine lookup by degree (0-359)
 * @param degree Angle in degrees (will be normalized to 0-359)
 * @return Sine value
 */
double sin_deg(int degree);

/**
 * @brief Fast cosine lookup by degree (0-359)
 * @param degree Angle in degrees (will be normalized to 0-359)
 * @return Cosine value
 */
double cos_deg(int degree);

// ============================================================================
// POWER AND ROOTS
// ============================================================================

/**
 * @brief Integer power function
 * @param base Base value
 * @param exp Exponent (non-negative)
 * @return base^exp
 */
int ipow(int base, int exp);

/**
 * @brief Square root using Newton-Raphson method
 * @param x Value (must be non-negative)
 * @return Square root of x
 */
double sqrt(double x);

#ifdef __cplusplus
}
#endif

// ============================================================================
// C++ NAMESPACE WRAPPER
// ============================================================================

#ifdef __cplusplus
namespace myOS::math {
    inline double sin(int degree) { return sin_deg(degree); }
    inline double cos(int degree) { return cos_deg(degree); }
}
#endif

