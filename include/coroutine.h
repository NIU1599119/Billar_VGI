#pragma once
// Modified from https://github.com/bigrando420/thomas/blob/main/sauce/th_coroutine.h
// to work with glfw

#define CAT(X,Y) CAT2(X,Y)
#define CAT2(X,Y) X##Y

#define USABLE_LINE int(CAT(__LINE__,U)) 

typedef struct Coroutine Coroutine;
struct Coroutine
{
	Coroutine() : line(0), start_time(0.0), data(nullptr) {};
	int line;
	double start_time;
	void* data;
};

#define CoroutineBegin(coro) switch (coro->line) {case 0: coro->line = 0;
#define CoroutineYield(coro) do { coro->line = USABLE_LINE; return; case USABLE_LINE:;} while(0)
#define CoroutineYieldUntil(coro, condition) while (!(condition)) { CoroutineYield(coro); }
#define CoroutineWait(coro, duration) do {if (coro->start_time == 0.0) { coro->start_time = glfwGetTime(); } CoroutineYieldUntil(coro, glfwGetTime() > coro->start_time + duration); coro->start_time = 0.0; } while (0)
#define CoroutineEnd(coro) do { coro->line = USABLE_LINE; } while (0); }
#define CoroutineReset(coro) do { coro->line = 0; } while (0); }
#define CoroutineIsFinished(coro) (coro->line == -1)