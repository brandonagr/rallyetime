///////////////////////////////////////////////////////
// CSpeech class
//
// Copyright © 2005 Dave Jacobs
///////////////////////////////////////////////////////

#define _WIN32_DCOM
#define WINVER 0x0400

#include <sapi.h>
#include <process.h>
#include <stdlib.h>
#include "speech.h"

#ifndef TRACE
#define TRACE ATLTRACE
#endif


CSpeech::CSpeech(void)
: m_iV(NULL)
, m_hEventWakeup(NULL)
, m_hThread(NULL)
, m_bAbort(FALSE)
{
	HRESULT hr;
	unsigned int threadID;
	
	InitializeCriticalSection(&m_csQueue);

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER,
		IID_ISpVoice, (void**) &m_iV);
	_ASSERT(hr == S_OK);	// If this assertion is hit then the Speech runtime is probably not
						// installed. All calls to Speak() will be ignored.

	m_hEventWakeup = CreateEvent(NULL, TRUE, FALSE, NULL);
	_ASSERT(m_hEventWakeup);

	m_hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (void*) this, 0, &threadID);
	_ASSERT(m_hThread != NULL);
}

CSpeech::~CSpeech(void)
{
	BOOL rc;
	_ASSERT(m_hThread != NULL);
	_ASSERT(!m_bAbort);

	{	// Flush the queue
		CCS ccs(&m_csQueue);

		m_bAbort = TRUE;

		if (TRUE)	// TRUE=Purge the queue to prevent waiting for queued speech to finish.
		{			// Currently speaking text will continue until done. FALSE=Wait for all queued speech.
			if (m_Queue.GetCount() > 0)
			{
				TRACE(_T("CSpeech: purging speech queue.\r\n"));
				m_Queue.RemoveAll();
			}
		}
	}

	rc = SetEvent(m_hEventWakeup);
	_ASSERT(rc);

	TRACE(_T("CSpeech: Waiting for speech thread to terminate.\r\n"));
	WaitForSingleObject(m_hThread, INFINITE);	// In case talking, wait here a while.
	CloseHandle(m_hThread);
	TRACE(_T("CSpeech: Speech thread terminated.\r\n"));

	if (m_iV != NULL)
		m_iV->Release();

	CloseHandle(m_hEventWakeup);

	CoUninitialize();

	DeleteCriticalSection(&m_csQueue);
}

// Worker thread which allows the caller of CSpeech::Speak to return immediately
// without having to wait for the speech to complete.
unsigned __stdcall CSpeech::ThreadProc(void* Arg)
{
	ULONG		nStream;
	CSpeech*	pThis = (CSpeech*) Arg;
	CString		string;
	BOOL		bEmptyQueue;

	_ASSERT(pThis->m_iV != NULL);
	_ASSERT(Arg != NULL);

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	pThis->m_iV->SetRate(-1);

	while (1)	// Loop until abort flag
	{
		bEmptyQueue = FALSE;	// Assume something's in there.
		{
			CCS ccs(&pThis->m_csQueue);

			string.Empty();
			if (pThis->m_Queue.GetCount() > 0)
			{
				TRACE(_T("CSpeech: Dequeueing text\r\n"));
				string = pThis->m_Queue.RemoveTail();
			}
			else
			{	// Queue is empty
				TRACE(_T("CSpeech: Speech queue is empty.\r\n"));
				ResetEvent(pThis->m_hEventWakeup);
				bEmptyQueue = TRUE;
			}
		}

		if (!bEmptyQueue)
		{
			if (!string.IsEmpty())
			{
				TRACE(_T("CSpeech: Speech thread saying <%s>\r\n"), string);
#ifndef _UNICODE
				// If the project is mbcs then convert this string to wide.
				WCHAR* lpWideString = NULL;
				size_t nWideSize = mbstowcs(NULL, string, string.GetLength());
				if (nWideSize > 0)
				{
					nWideSize += 2;	// Add room for the null at the end.
					lpWideString = new WCHAR[nWideSize];
					_ASSERT(lpWideString);
					memset(lpWideString, 0, nWideSize * 2);	// Clear garbage
					mbstowcs(lpWideString, string, string.GetLength());

					pThis->m_iV->Speak(lpWideString, SPF_ASYNC | SPF_PURGEBEFORESPEAK, &nStream);
					delete [] lpWideString;
				}
				else _ASSERT(0);	// for some reason the string converion failed. Maybe unsup'd char.
#else
				// ...else since we're using unicode, just submit the cstring itself.
				pThis->m_iV->Speak(string, SPF_ASYNC | SPF_PURGEBEFORESPEAK, &nStream);
#endif
			}
		}
		else
		{	// Wait for trigger since there's nothing in the queue.
			if (pThis->m_bAbort)
			{
				TRACE(_T("CSpeech: Speech thread received abort command.\r\n"));
				break;
			}

			TRACE(_T("CSpeech: Speech thread going to sleep.\r\n"));
			WaitForSingleObject(pThis->m_hEventWakeup, INFINITE);
			TRACE(_T("CSpeech: Speech thread signaled.\r\n"));
			ResetEvent(pThis->m_hEventWakeup);
		}
	}	// while ...

	// Abort was issued if this point was reached.

	CoUninitialize();	// This is only for the worker thread.

	return 0;
}

#define MAX_STRING_LENGTH 65536		// This is the largest size string allowed.

// Add a new string to the speech queue to be spoken in the order that it was queued.
// Returns FALSE if SAPI was not available when the CSpeech object was instantiated.
BOOL CSpeech::speak(LPCTSTR lpFormat, ...)
{
	TCHAR	buffer[MAX_STRING_LENGTH];
	va_list ptr;

	if (m_iV == NULL) return FALSE;	// No SAPI, don't do anything.

	// Lock the CString's buffer in order to write the var args to the CString.
	memset(&buffer, 0, MAX_STRING_LENGTH * sizeof(TCHAR));	// Clear the buffer first

	// Convert the args into the formatted string. (same as printf does). Watch the string
	// length if it can get long because there isn't yet a way to prevent a buffer overrun here.
	va_start(ptr, lpFormat);
	_vstprintf(buffer, lpFormat, ptr);
	va_end(ptr);

	{
		CCS ccs(&m_csQueue);

		if (!m_bAbort)
		{
			if (m_Queue.GetCount() < MAX_QUEUE_SIZE)
			{
				m_Queue.AddHead(buffer);
				SetEvent(m_hEventWakeup);
			}
			else
			{
				TRACE(_T("CSpeech: Speech queue length exceeded. Text ignored."));
			}
		}
	}

	return TRUE;
}
