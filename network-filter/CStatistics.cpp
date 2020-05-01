#include "pch.h"
#include "CStatistics.h"
#include <math.h>

CStatistics::CStatistics(DURATION duration) : CStatistics(duration, 30) {}
CStatistics::CStatistics(DURATION duration, INT32 samples) : CStatistics(duration, samples, 0.2f) {}
CStatistics::CStatistics(DURATION duration, INT32 samples, FLOAT recentSampleQuantifier)
{
	this->_sampleCount = samples;
	this->_recentSampleQuantifier = recentSampleQuantifier;
	this->_duration = duration;
	this->_samples.reserve(samples);
	for (INT32 i = 0; i < this->_sampleCount; i++) {
		this->_samples.push_back(new CNanoStatistics);
	}
	this->_start = CLOCK::now();
	this->_index = this->computeIndex(); //aka 0
}

CStatistics::~CStatistics()
{
	for (INT32 i = 0; i < this->_sampleCount; i++) {
		delete this->_samples[i];
	}
}

INT32 CStatistics::computeIndex() {
	auto past = CLOCK::now() - this->_start;
	return (past.count() / this->_duration.count()) % this->_sampleCount;
}

VOID CStatistics::Add(DOUBLE value) {
	INT32 newIndex = this->computeIndex();
	auto current = this->_samples[this->_index];
	current->Add(value);
	if (newIndex != this->_index) {
		auto next = this->_samples[newIndex];
		current->Stop();
		next->Start();
		this->_index = newIndex;
	}
}

DOUBLE CStatistics::Compute() {
	DOUBLE value = 0;
	for (INT32 i = 0; i < this->_sampleCount; i++) {
		value += this->_samples[i]->Compute();
	}
	return value / this->_sampleCount;
}

DOUBLE CStatistics::ComputeRecent() {
	INT32 index = this->_index;
	DOUBLE value = 0;
	INT32 count = (INT32)ceil((INT64)this->_sampleCount * (DOUBLE)this->_recentSampleQuantifier);
	if (count == 0) count = 1;
	for (INT32 i = 0; i < count; i++) {
		value += this->_samples[index]->Compute();
		index--;
		if (index < 0) index = this->_sampleCount - 1;
	}
	return value / count;
}

DURATION CStatistics::SampleDuration() {
	return this->_duration;
}