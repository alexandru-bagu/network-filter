#pragma once
#include <basetsd.h>
#include <chrono>
#include <wtypes.h>

typedef std::chrono::high_resolution_clock CLOCK;
typedef CLOCK::time_point TIME_POINT;
typedef std::chrono::nanoseconds NANOSECONDS;
typedef std::chrono::milliseconds MILLISECONDS;
typedef std::chrono::seconds SECONDS;
typedef std::chrono::minutes MINUTES;
typedef std::chrono::seconds HOURS;

///Nanosecond statistics
class CNanoStatistics {
private:
	BOOL _running;
	DOUBLE _value;
	TIME_POINT _since;
	TIME_POINT _until;
public:
	CNanoStatistics();
	VOID Start();
	VOID Stop();
	VOID Add(DOUBLE);
	DOUBLE Compute();
};