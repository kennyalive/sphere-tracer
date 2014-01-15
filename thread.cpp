#include "stdafx.h"
#include "thread.h"

Task* Thread::m_pendingTask = NULL;
HANDLE Thread::m_taskReadyEvent = NULL;
HANDLE Thread::m_taskCommitedEvent = NULL;
HANDLE Thread::m_getResultsEvent = NULL;
std::vector<HANDLE> Thread::m_taskCompletedEvents;

//---------------------------------------------------------------------
Thread::Thread(int processor) {
	if (!m_taskReadyEvent) {
		m_taskReadyEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	if (!m_taskCommitedEvent) {
		m_taskCommitedEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	if (!m_getResultsEvent) {
		m_getResultsEvent = ::CreateEvent(NULL, FALSE, TRUE, NULL);
	}
	m_thread = ::CreateThread(NULL, 0, &Thread::Main, (LPVOID)this, 0, NULL);
	m_taskCompletedEvent = ::CreateEvent(NULL, FALSE, TRUE, NULL);
	m_taskCompletedEvents.push_back(m_taskCompletedEvent);

	if (m_thread && processor != -1) {
		::SetThreadAffinityMask(m_thread, 1 << processor);
	}
}

//---------------------------------------------------------------------
void Thread::CommitTask(Task* task) {
	m_pendingTask = task;
	::SetEvent(m_taskReadyEvent);
	::WaitForSingleObject(m_taskCommitedEvent, INFINITE);
}

//---------------------------------------------------------------------
void Thread::WaitForTasks(bool waitAll) {
	::WaitForMultipleObjects(m_taskCompletedEvents.size(), &m_taskCompletedEvents[0], waitAll ? TRUE : FALSE, INFINITE);
}

//---------------------------------------------------------------------
DWORD Thread::Main(PVOID pvParam) {
	Thread* thread = static_cast<Thread*>(pvParam);
	while (true) {
		// Waiting for the next task
		::WaitForSingleObject(m_taskReadyEvent, INFINITE);

		// Get pending task
		::ResetEvent(thread->m_taskCompletedEvent);
		Task* task = m_pendingTask;
		::SetEvent(m_taskCommitedEvent);
		if (!task) break;

		// Run task
		task->Run();

		// Perform exclusive operations
		::WaitForSingleObject(m_getResultsEvent, INFINITE);
		task->ExclusiveRun();
		::SetEvent(m_getResultsEvent);

		// Notify that task is completed
		::SetEvent(thread->m_taskCompletedEvent);
	}
	return 0;
}
