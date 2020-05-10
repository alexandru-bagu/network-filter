#pragma once
#include <mutex>
#include <memory>

typedef std::mutex MUTEX;
typedef std::lock_guard<MUTEX> LOCK_GUARD;
typedef std::unique_ptr<MUTEX> MUTEX_PTR;

#define BEGIN_LOCK(T) { MUTEX& mutex = T; const LOCK_GUARD lock(mutex);
#define BEGIN_LOCK_PTR(T) { MUTEX* mutexPtr = (T); MUTEX& mutex = *mutexPtr; const LOCK_GUARD lock(mutex);
#define END_LOCK(T) }