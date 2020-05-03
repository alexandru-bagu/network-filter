#pragma once
#include <mutex>

typedef std::mutex MUTEX;
typedef std::lock_guard<MUTEX> LOCK_GUARD;

#define BEGIN_LOCK(T) { MUTEX& mutex = T; const LOCK_GUARD lock(mutex);
#define END_LOCK(T) }