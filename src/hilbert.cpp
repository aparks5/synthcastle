#include "hilbert.h"

HilbertTransform::HilbertTransform()
	: x1(0.f)
{
	m_ap1[0].coeff = ap1_a;
	m_ap1[1].coeff = ap1_b;
	m_ap1[2].coeff = ap1_c;
	m_ap1[3].coeff = ap1_d;

	m_ap2[0].coeff = ap2_a;
	m_ap2[1].coeff = ap2_b;
	m_ap2[2].coeff = ap2_c;
	m_ap2[3].coeff = ap2_d;

}

float HilbertTransform::real(float in)
{
	float real = 0.f;
	// REAL PART

	// perform filter for each section in series
	float filtout = 0.f;
	float x = in;
	float out = 0.f;
	for (size_t idx = 0; idx < 4; idx++) {
		out = (m_ap1[idx].coeff * (x + m_ap1[idx].y2)) - m_ap1[idx].x2;
		// delay input
		m_ap1[idx].x2 = m_ap1[idx].x1;
		m_ap1[idx].x1 = x;
		// delay output
		m_ap1[idx].y2 = m_ap1[idx].y1;
		m_ap1[idx].y1 = out;
		x = out; // to feed next filter

	}
	filtout = x;

	// delay REAL PART by 1 sample
	real = x1;
	x1 = filtout;
	return real;

}

float HilbertTransform::imaginary(float in)
{
	float imaginary = 0.f;

	// perform filter for each section in series
	float filtout = 0.f;
	float x = in;
	float out = 0.f;
	for (size_t idx = 0; idx < 4; idx++) {
		out = (m_ap2[idx].coeff * (x + m_ap2[idx].y2)) - m_ap2[idx].x2;
		// delay input
		m_ap2[idx].x2 = m_ap2[idx].x1;
		m_ap2[idx].x1 = x;
		// delay output
		m_ap2[idx].y2 = m_ap2[idx].y1;
		m_ap2[idx].y1 = out;
		x = out; // to feed next filter

	}
	imaginary = out;

	return imaginary;
}


