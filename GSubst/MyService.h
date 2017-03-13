#pragma once

#include "ServiceBase.h"

class CMyService : public CServiceBase
{
public:
	CMyService(PWSTR pszServiceName,
		BOOL fCanStop = TRUE,
		BOOL fCanShutdown = TRUE,
		BOOL fCanPauseContinue = FALSE);
	virtual ~CMyService();

protected:

	virtual void OnStart(DWORD dwArgc, PWSTR *pszArgv);
	virtual void OnStop();

	void ServiceWorkerThread(void);

private:

	HANDLE m_hStoppedEvent;
	HANDLE m_hFinishEvent;
};

