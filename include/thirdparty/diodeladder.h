#pragma once

#ifndef M_PI
#define M_PI  (3.14159265)
#endif
#include <cstdint>
#include <cmath>

class DiodeLadder 
{
public:
    DiodeLadder(float Fs = 44100.0f)
    {
        sr = Fs;
        s0 = s1 = s2 = s3 = 0;
        zi = 0;
        xx = y0 = y1 = y2 = y3 = 0;

        volume = pow(10, 0.5 / 20);
        fc = 440.0f;
        q = 0.5;

        setVolume(10.f);
    }
    void setCutoff(float f) {
        fc = f;
    }
    void setResonance(float res) {
        q = res;
    }
    void setVolume(float db) {
        volume = pow(10, db / 20.0f);
    }

    inline double Tick(double sample, double A = 1, double X = 0, double Y = 0)
    {

        float f = tan(M_PI * fc / sr);
        float r = 7 * q + 0.5;

        const double input = sample;

        // input with half delay, for non-linearities
        const double ih = 0.5f * (input + zi);

        // gains
        double g0 = 1.f;
        double g1 = 1.836f;
        double g2 = 3 * 1.836f;

        double g0inv = 1.f / g0;
        double g1inv = 1.f / g1;
        double g2inv = 1.f / g2;

        // optimization of function calls 
        const double in0 = (ih - r * s3) * g0inv;
        const double a0 = in0 * in0;
        const double t0 = g0inv * f * (((a0 + 105) * a0 + 945) / ((15 * a0 + 420) * a0 + 945));

        const double in1 = (s1 - s0) * g1inv;
        const double a1 = in1 * in1;
        const double t1 = g1inv * f * (((a1 + 105) * a1 + 945) / ((15 * a1 + 420) * a1 + 945));

        const double in2 = (s2 - s1) * g1inv;
        const double a2 = in2 * in2;
        const double t2 = g1inv * f * (((a2 + 105) * a2 + 945) / ((15 * a2 + 420) * a2 + 945));

        const double in3 = (s3 - s2) * g1inv;
        const double a3 = in3 * in3;
        const double t3 = g1inv * f * (((a3 + 105) * a3 + 945) / ((15 * a3 + 420) * a3 + 945));

        const double in4 = (s3)*g2inv;
        const double a4 = in4 * in4;
        const double t4 = g2inv * f * (((a4 + 105) * a4 + 945) / ((15 * a4 + 420) * a4 + 945));

        // This formula gives the result for y3 thanks to MATLAB
        double y3 = (s2 + s3 + t2 * (s1 + s2 + s3 + t1 * (s0 + s1 + s2 + s3 + t0 * zi)) + t1 * (2 * s2 + 2 * s3)) * t3 + s3 + 2 * s3 * t1 + t2 * (2 * s3 + 3 * s3 * t1);
        y3 /= (t4 + t1 * (2 * t4 + 4) + t2 * (t4 + t1 * (t4 + r * t0 + 4) + 3) + 2) * t3 + t4 + t1 * (2 * t4 + 2) + t2 * (2 * t4 + t1 * (3 * t4 + 3) + 2) + 1;

        // Other outputs
        double y2 = (s3 - (1 + t4 + t3) * y3) / (-t3);
        double y1 = (s2 - (1 + t3 + t2) * y2 + t3 * y3) / (-t2);
        double y0 = (s1 - (1 + t2 + t1) * y1 + t2 * y2) / (-t1);
        double xx = (zi - r * y3);

        // update state
        s0 += denormals(2 * (t0 * xx + t1 * (y1 - y0)));
        s1 += denormals(2 * (t2 * (y2 - y1) - t1 * (y1 - y0)));
        s2 += denormals(2 * (t3 * (y3 - y2) - t2 * (y2 - y1)));
        s3 += denormals(2 * (-t4 * (y3)-t3 * (y3 - y2)));

        zi = input;

        return y3 * r * volume;
    }
    void reset()
    {
        zi = 0;
        s0 = s1 = s2 = s3 = 0;
    }

private:
    double s0, s1, s2, s3;
    double zi;
    double xx, y0, y1, y2, y3;
    double sr;
    double fc, q, volume;

	inline float fclamp(float in, float min, float max){
	    return fmin(fmax(in, min), max);
	}

    inline float denormals(float in) {
        return fclamp(in, -1e30, 1e30);
    }

};
