#pragma once
#include <chrono>
#include <string>
#include <iostream>

#ifdef QUERYTIMER
#include <Windows.h>
const long long g_Frequency = []() -> long long
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return frequency.QuadPart;
}();

struct highResClock
{
    typedef long long rep;
    typedef std::nano period;
    typedef std::chrono::duration<rep, period> duration;
    typedef std::chrono::time_point<highResClock> time_point;
    static const bool is_steady = true;

	static time_point now() {
		LARGE_INTEGER count;
		QueryPerformanceCounter(&count);
		return time_point(duration(count.QuadPart * static_cast<rep>(period::den) / g_Frequency)); 
	}
};
typedef highResClock timeclass;
#else
typedef std::chrono::high_resolution_clock timeclass;
#endif

template <class rettm>
class Timer
{
private:
	std::string _print;
	double mod, mint;
	timeclass::time_point stime, etime, ptime;
	bool active;
public:
	static bool print_on_destruct;
	static double prevTime;

	void start()
	{
		stime = timeclass::now();
		active = true;
	}

	void reset()
	{
		stime = timeclass::now();
	}

	void stop()
	{
		etime = timeclass::now();
		active = false;
	}

	auto getTime() -> decltype(std::chrono::duration_cast<rettm>(etime - stime))
	{
		timeclass::time_point rtime = etime;
		if (active) rtime = timeclass::now();
		return std::chrono::duration_cast<rettm>(rtime - stime);
	}

	Timer()
	{
		mod = 1.0;
		mint = 0.0;
		active = false;
		stime = timeclass::now();
		etime = timeclass::now();
	}

	Timer(std::string print, double mod = 1.0, double mint = 0.0)
	{
		this->mod = mod;
		this->mint = mint;
		active = false;
		stime = timeclass::now();
		etime = timeclass::now();
		_print = print;
		start();
	}

	~Timer()
	{
		prevTime = double(getTime().count());
		double printtime = prevTime / mod;
		if (print_on_destruct && printtime >= mint) printf("%.3f%s\n", printtime, _print.c_str());
	}
};

template <class rettm>
bool Timer<rettm>::print_on_destruct = false;

template <class rettm>
double Timer<rettm>::prevTime = 0.0;

typedef Timer<std::chrono::milliseconds> militimer;
typedef Timer<std::chrono::microseconds> microtimer;
typedef Timer<std::chrono::nanoseconds> nanotimer;

