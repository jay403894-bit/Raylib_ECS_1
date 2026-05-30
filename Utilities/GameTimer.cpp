#include "GameTimer.h"


GameTimer::GameTimer()
	: mSecondsPerCount(0.0), mDeltaTime(-1.0),
	mBaseTime(0), mPausedTime(0), mPrevTime(0), mCurrTime(0),
	mStopped(false)
{
	LARGE_INTEGER countsPerSec;
	if (QueryPerformanceFrequency(&countsPerSec) == 0) {
		throw std::runtime_error("QueryPerformanceFrequency failed.");
	}

	// Convert to milliseconds per count
	mSecondsPerCount = 1.0 / static_cast<double>(countsPerSec.QuadPart);
	// Convert to milliseconds
	mSecondsPerCount *= 1000.0;  // Now mSecondsPerCount is in milliseconds
}

void GameTimer::Tick()
{
	if (mStopped) {
		mDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;  // Delta time in milliseconds
	mPrevTime = mCurrTime;

	if (mDeltaTime < 0.0) {
		mDeltaTime = 0.0;
	}
}

float GameTimer::DeltaTime() const
{
	return (float)mDeltaTime;  // Delta time in milliseconds
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Stop()
{
	// If we are already stopped, then don’t do anything.
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mStopTime = currTime;
		mStopped = true;
	}
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (mStopped)
	{
		mPausedTime += (startTime - mStopTime);

		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

float GameTimer::TotalTime() const {
	if (mStopped)
	{
		return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);  // Total time in milliseconds
	}
	else
	{
		return (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);  // Total time in milliseconds
	}
}