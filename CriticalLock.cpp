// CriticalLock.cpp: implementation of the CCriticalLock class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CriticalLock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCriticalLock::CCriticalLock()
{
	InitializeCriticalSection(&m_cs);
}

CCriticalLock::~CCriticalLock()
{
	DeleteCriticalSection(&m_cs);
}


void CCriticalLock::Lock()
{
	EnterCriticalSection(&m_cs);
}

void CCriticalLock::Unlock()
{
	LeaveCriticalSection(&m_cs);
}


CCriticalLock::CAutoLock::CAutoLock(CCriticalLock&  cs):m_lock(cs)
{
	m_lock.Lock();
}

CCriticalLock::CAutoLock::~CAutoLock()
{
	m_lock.Unlock();
}
