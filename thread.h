#ifndef _THREAD_H_
#define _THREAD_H_

//---------------------------------------------------------------------
class Task {
public:
	// Runs in parallel with other tasks
	virtual void Run() = 0;
	// Called after Run. It's guaranteed that only one ExlusiveRun can be active at the moment
	virtual void ExclusiveRun() {} 
};

//---------------------------------------------------------------------
class Thread {
public:
	Thread(int processor = -1);
	static void CommitTask(Task* task);
	static void WaitForTasks(bool waitAll);

private:
	static DWORD WINAPI Main(PVOID pvParam);

private:
	HANDLE m_thread;
	HANDLE m_taskCompletedEvent;

	static Task* m_pendingTask;
	static HANDLE m_taskReadyEvent;
	static HANDLE m_taskCommitedEvent;
	static HANDLE m_getResultsEvent;
	static std::vector<HANDLE> m_taskCompletedEvents;
};

#endif //_THREAD_H_
