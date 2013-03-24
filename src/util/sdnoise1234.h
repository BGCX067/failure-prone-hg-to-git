/* sdnoise1234, Simplex noise with true analytic
 * derivative in 1D to 4D.
 *
 * Copyright © 2003-2011, Stefan Gustavson
 *
 * Contact: stefan.gustavson@gmail.com
 *
 * This library is public domain software, released by the author
 * into the public domain in February 2011. You may do anything
 * you like with it. You may even remove all attributions,
 * but of course I'd appreciate it if you kept my name somewhere.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

/** \file
    \brief C header file for Perlin simplex noise with analytic
    derivative over 1, 2, 3 and 4 dimensions.
    \author Stefan Gustavson (stefan.gustavson@gmail.com)
*/

/*
 * This is an implementation of Perlin "simplex noise" over one
 * dimension (x), two dimensions (x,y), three dimensions (x,y,z)
 * and four dimensions (x,y,z,w). The analytic derivative is
 * returned, to make it possible to do lots of fun stuff like
 * flow animations, curl noise, analytic antialiasing and such.
 *
 */

#include <math.h>

/** 1D simplex noise with derivative.
 * If the last argument is not null, the analytic derivative
 * is also calculated.
 */
float sdnoise1( float x, float *dnoise_dx);

/** 2D simplex noise with derivatives.
 * If the last two arguments are not null, the analytic derivative
 * (the 2D gradient of the scalar noise field) is also calculated.
 */
float sdnoise2( float x, float y, float *dnoise_dx, float *dnoise_dy );

/** 3D simplex noise with derivatives.
 * If the last tthree arguments are not null, the analytic derivative
 * (the 3D gradient of the scalar noise field) is also calculated.
 */
float sdnoise3( float x, float y, float z,
                float *dnoise_dx, float *dnoise_dy, float *dnoise_dz );

/** 4D simplex noise with derivatives.
 * If the last four arguments are not null, the analytic derivative
 * (the 4D gradient of the scalar noise field) is also calculated.
 */
float sdnoise4( float x, float y, float z, float w,
                float *dnoise_dx, float *dnoise_dy,
                float *dnoise_dz, float *dnoise_dw);

/* WRAPERS
   freq is the frequency of the first octave
   persisrence controls the roughness, larger values produce rougher noise. persistence determines how quickly the amplitude
   dimish for successive octaves. 
   lacunarity specifies the frequency multipler between successive octaves */
float fbm1(float x, float persistence, float lacunarity, float freq, unsigned int noctaves );

float fbm2(float x, float y, float  persistence, float lacunarity, float freq, unsigned int noctaves);

float fbm3(float x, float y, float z, float persistence, float lacunarity, float freq, unsigned int noctaves);

// usado pra motanhas pedregrosas ou marmore offset = 1, gain = 2 H = 1.0
float ridgedMulti(float x, float y,  float lacunarity, float freq, float gain, float offset, float H, unsigned int noctaves);

float turbulence2(float x, float y, float freq, float scale, unsigned int noctaves);

