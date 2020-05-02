#pragma once
#include "CNanoStatistics.h"
#include <vector>

typedef std::chrono::high_resolution_clock::duration DURATION;
typedef std::vector<CNanoStatistics> SAMPLE_VECTOR;
/// Millisecond based statistics with dynamic duration average
class CStatistics {
private:
	SAMPLE_VECTOR _samples;
	INT32 _index, _sampleCount;
	FLOAT _recentSampleQuantifier;
	DURATION _duration;
	TIME_POINT _start;
	INT32 computeIndex();
public:
	///Defaults to 30 samples and 20% recent samples
	CStatistics(DURATION duration);
	///Defaults 20% recent samples
	CStatistics(DURATION duration, INT32 samples);
	CStatistics(DURATION duration, INT32 samples, FLOAT recentSampleQuantifier);
	~CStatistics();
	VOID Add(DOUBLE);

	/// Returns average based on all samples
	DOUBLE Compute();

	/// Returns average based on last 20% samples
	DOUBLE ComputeRecent();

	/// Returns the sample duration
	DURATION SampleDuration();
};