#pragma once

#include <array>
#include <vector>

#include <immintrin.h>

#include "ao/kernel/eval/interval.hpp"

struct Result {
    /*
     *  Sets a particular value in the array
     *  (inlined for efficiency)
     */
    void set(float v, size_t index)
    {
        f[index] = v;
    }

    /*
     *  Sets the interval value in the array
     */
    void set(Interval V);

    /*
     *  Returns the float at the given index
     */
    float get(size_t index) const { return f[index]; }

    /*
     *  Sets all of the values to the given constant float
     *  (across the Interval, float, Gradient, and __m256 arrays)
     *
     *  Gradients are set to {0, 0, 0}
     */
    void fill(float v);

    /*
     *  Fills the derivative arrays with the given values
     */
    void deriv(float x, float y, float z);

#ifdef __AVX__
    /*
     *  Packs values from the float array into the AVX array
     */
    void packAVX();

    /*
     *  Unpacks values from the AVX array into the float array
     */
    void unpackAVX();
#endif

protected:
    float f[256];
    float dx[256];
    float dy[256];
    float dz[256];

    Interval i;

#ifdef __AVX__
    __m256 mf[32];
    __m256 mdx[32];
    __m256 mdy[32];
    __m256 mdz[32];
#endif

    friend class Evaluator;
    friend class Clause;
};
