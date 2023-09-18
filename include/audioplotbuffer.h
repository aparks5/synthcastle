#pragma once

struct AudioPlotBuffer
{
	std::vector<std::vector<float>> buffer;
	size_t m_writeIdx;
};
