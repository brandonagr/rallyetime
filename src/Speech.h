///////////////////////////////////////////////////////
// CSpeech class
//
// Copyright © 2005 Dave Jacobs
///////////////////////////////////////////////////////

#pragma once

#include <sapi.h>
#include <atlstr.h>
#include <atlcoll.h>

#define MAX_QUEUE_SIZE 1000		// Limit queue strings to this many.

class CSpeech
{
public:
			CSpeech(void);
			~CSpeech(void);

	BOOL	speak(LPCTSTR lpFormat, ...);

protected:
	static unsigned __stdcall ThreadProc( void* pArguments );

	ISpVoice*			m_iV;			// Voice Interface

	//CStringList			m_Queue;		// Store text here in FIFO
	CAtlList<CString>			m_Queue;

	HANDLE				m_hEventWakeup;	// Wake up worker and examin queue
	CRITICAL_SECTION	m_csQueue;		// Protect the integrity of the queue.
	HANDLE				m_hThread;		// Worker actually does the speaking.
	BOOL				m_bAbort;		// Tell worker to shutdown
};

// Critical section wrapper class used by the CSpeech class.
class CCS
{
	public:
	// Constructor to lock the CS
	__inline CCS(LPCRITICAL_SECTION lpCS): m_lpCS(NULL)
	{
		_ASSERT(lpCS != NULL);

		m_lpCS = lpCS;	// Save
		EnterCriticalSection(lpCS);
	}

	// Destructor unlocks it.
	__inline ~CCS()
	{
		if (m_lpCS != NULL)
			LeaveCriticalSection(m_lpCS);
	}
	
	LPCRITICAL_SECTION	m_lpCS;
};
