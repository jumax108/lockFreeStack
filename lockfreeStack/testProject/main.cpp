#include <stdio.h>
#include <thread>

#include "../headers/lockFreeStack.h"

constexpr int THREAD_NUM = 4;
constexpr int NODE_NUM_EACH_THREAD = 500;

constexpr int TOTAL_NODE_NUM = THREAD_NUM * NODE_NUM_EACH_THREAD;

#include "dump/headers/dump.h"
#pragma comment(lib, "lib/dump/dump")

#include "profilerTLS/headers/profilerTLS.h"
#pragma comment(lib, "lib/profilerTLS/profilerTLS")

CDump dump;

CProfilerTLS profile;

struct stNode {

	stNode() {
		num = 0;
	}

	int num;

};

using stack = CLockFreeStack<stNode*>;

stack* lockFreeStack = new stack();
stack* stackForDebug = nullptr;

unsigned __stdcall logicTestFunc(void* args);

int tps = 0;
int main() {

	HANDLE th[THREAD_NUM];

	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	for (int nodeCnt = 0; nodeCnt < TOTAL_NODE_NUM; ++nodeCnt) {
		lockFreeStack->push(new stNode);
	}

	for (int threadCnt = 0; threadCnt < THREAD_NUM; ++threadCnt) {
		th[threadCnt] = (HANDLE)_beginthreadex(nullptr, 0, logicTestFunc, nullptr, 0, nullptr);
	}
	/*
	for (;;) {
		printf("stack Size: %d\n", lockFreeStack->size());
		printf("tps : %d\n\n\n\n", tps);
		tps = 0;
		Sleep(999);
	}
	*/

	WaitForMultipleObjects(THREAD_NUM, th, true, INFINITE);

	profile.printToFile();

	return 0;

}

SRWLOCK lock;

unsigned __stdcall logicTestFunc(void* args) {

	stNode* nodes[NODE_NUM_EACH_THREAD];

	for (int i = 0; i < 2; i++) {
		for (int nodeCnt = 0; nodeCnt < NODE_NUM_EACH_THREAD; ++nodeCnt) {
			//lockFreeStack->front(&nodes[nodeCnt]);
			lockFreeStack->pop(&nodes[nodeCnt]);
		}
		for (int nodeCnt = 0; nodeCnt < NODE_NUM_EACH_THREAD; ++nodeCnt) {
			lockFreeStack->push(nodes[nodeCnt]);
		}
	}

	for (int loop=0; loop<2000000; loop++) {

		///////////////////////////////////////////////////
		// 1. stack???? node?? pop
		for (int nodeCnt = 0; nodeCnt < NODE_NUM_EACH_THREAD; ++nodeCnt) {
			profile.begin("pop");
			//lockFreeStack->front(&nodes[nodeCnt]);
			lockFreeStack->pop(&nodes[nodeCnt]);
			profile.end("pop");
		}
		///////////////////////////////////////////////////
		/*
		///////////////////////////////////////////////////
		// 2. node?? ?????Ͱ? 0???? Ȯ??
		for (int nodeCnt = 0; nodeCnt < NODE_NUM_EACH_THREAD; ++nodeCnt) {
			if (nodes[nodeCnt]->num != 0) {
				stackForDebug = lockFreeStack;
				lockFreeStack = nullptr;
				CDump::crash();
			}
		}
		///////////////////////////////////////////////////

		///////////////////////////////////////////////////
		// 3. ?????? 1 ????
		for (int nodeCnt = 0; nodeCnt < NODE_NUM_EACH_THREAD; ++nodeCnt) {
			InterlockedIncrement((LONG*)&nodes[nodeCnt]->num);
		}
		///////////////////////////////////////////////////

		///////////////////////////////////////////////////
		// 4. node?? ?????Ͱ? 1???? Ȯ??
		for (int nodeCnt = 0; nodeCnt < NODE_NUM_EACH_THREAD; ++nodeCnt) {
			if (nodes[nodeCnt]->num != 1) {
				stackForDebug = lockFreeStack;
				lockFreeStack = nullptr;
				CDump::crash();
			}
		}
		///////////////////////////////////////////////////

		///////////////////////////////////////////////////
		// 5. ?????? 1 ????
		for (int nodeCnt = 0; nodeCnt < NODE_NUM_EACH_THREAD; ++nodeCnt) {
			InterlockedDecrement((LONG*)&nodes[nodeCnt]->num);
		}
		///////////////////////////////////////////////////

		///////////////////////////////////////////////////
		// 6. node?? ?????Ͱ? 0???? Ȯ??
		for (int nodeCnt = 0; nodeCnt < NODE_NUM_EACH_THREAD; ++nodeCnt) {
			if (nodes[nodeCnt]->num != 0) {
				stackForDebug = lockFreeStack;
				lockFreeStack = nullptr;
				CDump::crash();
			}
		}
		///////////////////////////////////////////////////
		*/
		///////////////////////////////////////////////////
		// 7. stack?? ????
		for (int nodeCnt = 0; nodeCnt < NODE_NUM_EACH_THREAD; ++nodeCnt) {
			profile.begin("push");
			lockFreeStack->push(nodes[nodeCnt]);
			profile.end("push");
		}

		//Sleep(0);

		///////////////////////////////////////////////////
		//InterlockedIncrement((LONG*)&tps);
	}

	return 0;
}