#ifndef		___FAST_LIST___H
#define		___FAST_LIST___H

#include "CriticalLock.h"


template<typename DataType>
class CFastList
{
public:
	typedef struct tagNoteInfo
	{
		DataType	noteData;
		tagNoteInfo*	pNext;
		
		tagNoteInfo()
		{
			pNext = NULL;
		}
		
	}NoteInfo,*PNoteInfo;

public:
	CFastList()
	{
		m_pHead = new NoteInfo();
		m_pHead->pNext = NULL;

		m_pTail = m_pHead;
	}

	~CFastList()
	{
		Clear();
		delete m_pHead;
	}

	void PushBack(DataType dataInfo)
	{
		NoteInfo* pNote = new NoteInfo();
		pNote->noteData = dataInfo;
		pNote->pNext = NULL;
		
		CCriticalLock::CAutoLock lock(m_tailLock);
		m_pTail->pNext = pNote;
		m_pTail = pNote;

	}

	DataType&	Front()
	{
		return m_pHead->pNext->noteData;
	}

	void	PopFront()
	{
		CCriticalLock::CAutoLock lock(m_headLock);
		NoteInfo* pNewHead = m_pHead->pNext;
		if (pNewHead)
		{
			delete m_pHead;
			m_pHead = pNewHead;
		}
	}

	void Clear()
	{
		CCriticalLock::CAutoLock lock(m_headLock);
		NoteInfo* pNewHead= m_pHead->pNext;
		
		while (pNewHead)
		{
			delete m_pHead;
			m_pHead = pNewHead;
			pNewHead= m_pHead->pNext;
		}
	}

protected:
	PNoteInfo		m_pHead;
	PNoteInfo		m_pTail;
	CCriticalLock	m_headLock;
	CCriticalLock	m_tailLock;
};

#endif