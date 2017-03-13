#include "stdafx.h"
#include "MyService.h"
#include "ThreadPool.h"


CMyService::CMyService(PWSTR pszServiceName,
	BOOL fCanStop,
	BOOL fCanShutdown,
	BOOL fCanPauseContinue)
	: CServiceBase(pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue)
	, m_hStoppedEvent(::CreateEventW(NULL, TRUE, FALSE, NULL))
	, m_hFinishEvent(::CreateEventW(NULL, TRUE, FALSE, NULL))
{
	// Create a manual-reset event that is not signaled at first to indicate 
	// the stopped signal of the service.	
}


CMyService::~CMyService()
{
	if (m_hStoppedEvent)
	{
		::CloseHandle(m_hStoppedEvent);
		m_hStoppedEvent = nullptr;
	}
	if (m_hFinishEvent)
	{
		::CloseHandle(m_hFinishEvent);
		m_hFinishEvent = nullptr;
	}
}
//
//   FUNCTION: CSampleService::OnStart(DWORD, LPWSTR *)
//
//   PURPOSE: The function is executed when a Start command is sent to the 
//   service by the SCM or when the operating system starts (for a service 
//   that starts automatically). It specifies actions to take when the 
//   service starts. In this code sample, OnStart logs a service-start 
//   message to the Application log, and queues the main service function for 
//   execution in a thread pool worker thread.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
void CMyService::OnStart(DWORD dwArgc, PWSTR * pszArgv)
{
	// Log a service start message to the Application log.
	WriteEventLogEntry(L"CppWindowsService in OnStart",
		EVENTLOG_INFORMATION_TYPE);

	// Queue the main service function for execution in a worker thread.
	CThreadPool::QueueUserWorkItem(&CMyService::ServiceWorkerThread, this);
}


//
//   FUNCTION: CSampleService::ServiceWorkerThread(void)
//
//   PURPOSE: The method performs the main function of the service. It runs 
//   on a thread pool worker thread.
//
void CMyService::ServiceWorkerThread(void)
{
	// Periodically check if the service is stopping.
	while (WAIT_TIMEOUT == ::WaitForSingleObject(m_hFinishEvent, 2000))
	{
		// Perform main service function here...		
	}

	// Signal the stopped event.
	SetEvent(m_hStoppedEvent);
}


//
//   FUNCTION: CSampleService::OnStop(void)
//
//   PURPOSE: The function is executed when a Stop command is sent to the 
//   service by SCM. It specifies actions to take when a service stops 
//   running. In this code sample, OnStop logs a service-stop message to the 
//   Application log, and waits for the finish of the main service function.
//
//   COMMENTS:
//   Be sure to periodically call ReportServiceStatus() with 
//   SERVICE_STOP_PENDING if the procedure is going to take long time. 
//
void CMyService::OnStop()
{
	// Log a service stop message to the Application log.
	WriteEventLogEntry(L"CppWindowsService in OnStop",
		EVENTLOG_INFORMATION_TYPE);

	// Indicate that the service is stopping and wait for the finish of the 
	// main service function (ServiceWorkerThread).
	::SetEvent(m_hFinishEvent);
	if (WaitForSingleObject(m_hStoppedEvent, INFINITE) != WAIT_OBJECT_0)
	{
		throw GetLastError();
	}
}