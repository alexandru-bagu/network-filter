#pragma once
#include <mutex>

typedef std::mutex MUTEX;
typedef std::lock_guard<MUTEX> LOCK_GUARD;

#define LOCK(T, Y) { const LOCK_GUARD lock(T); Y }