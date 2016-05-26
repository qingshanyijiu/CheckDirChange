// CriticalLock.h: interface for the CCriticalLock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRITICALLOCK_H__88612DA1_1A5C_420F_B1D4_EE7B2521309C__INCLUDED_)
#define AFX_CRITICALLOCK_H__88612DA1_1A5C_420F_B1D4_EE7B2521309C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCriticalLock  
{
public:
	CCriticalLock();
	virtual ~CCriticalLock();
	void Lock();
	void Unlock();

public:
	class CAutoLock
	{
	public:
		CAutoLock(CCriticalLock&  cs);
		~CAutoLock();

	protected:
		CAutoLock& operator = (CAutoLock&);
		CAutoLock(CAutoLock&);

	private:
		CCriticalLock& m_lock;
	};

protected:
	CCriticalLock& operator = (CCriticalLock&);
	CCriticalLock(CCriticalLock&);

private:
	CRITICAL_SECTION	m_cs;
};

#endif // !defined(AFX_CRITICALLOCK_H__88612DA1_1A5C_420F_B1D4_EE7B2521309C__INCLUDED_)
