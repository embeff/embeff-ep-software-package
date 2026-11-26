// Copyright (C) 2020 embeff GmbH
// SPDX-License-Identifier: Apache-2.0
// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com

// This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.
// Examples taken from http://www.ganssle.com/approx/sincos.cpp

// Math constants we'll use
static float const pi=3.1415926535897932384626433f;	// pi
static float const twopi=2.0f*pi;			// pi times 2
static float const two_over_pi= 2.0f/pi;		// 2/pi

// *********************************************************
// ***
// ***   Routines to compute sine and cosine to 3.2 digits
// ***  of accuracy.
// ***
// *********************************************************
//
//		cos_32s computes cosine (x)
//
//  Accurate to about 3.2 decimal digits over the range [0, pi/2].
//  The input argument is in radians.
//
//  Algorithm:
//		cos(x)= c1 + c2*x**2 + c3*x**4
//   which is the same as:
//		cos(x)= c1 + x**2(c2 + c3*x**2)
//
float cos_32s(float x)
{
    const float c1= 0.99940307f;
    const float c2=-0.49558072f;
    const float c3= 0.03679168f;

    float x2 = x*x;
    return (c1 + x2*(c2 + c3 * x2));
}


// This is the function that is benchmarked in cos_bench.robot
//
//  This is the main cosine approximation "driver"
// It reduces the input argument's range to [0, pi/2],
// and then calls the approximator.
// See the notes for an explanation of the range reduction.
//
float cos_32(float x){
	int quad;						// what quadrant are we in?

	if(x<0)x=-x;					// cos(-x) = cos(x)
	quad=int(x * two_over_pi);			// Get quadrant # (0 to 3) we're in
	switch (quad){
	case 0: return  cos_32s(x);
	case 1: return -cos_32s(pi-x);
	case 2: return -cos_32s(x-pi);
	case 3: return  cos_32s(twopi-x);
	}
    return 0.0f;
}
