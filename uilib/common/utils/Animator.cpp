
#include "stdafx.h"
#include "Animator.h"

//////////////////////////////////////////////////////////////////////////
// 匀变化动画

BOOL UNIFORMANIMATOR::Init(int nBeginValue, int nEndValue, DWORD dwTotalCostTime) 
{
	if (dwTotalCostTime == 0)
		return FALSE ;

	m_nBeginValue = nBeginValue ;
	m_nEndValue = nEndValue ;
	m_dwTotalCostTime = dwTotalCostTime ;

	return TRUE ;
}

int UNIFORMANIMATOR::Start() 
{
	m_dwBeginTime = GetTickCount() ;
	return m_nBeginValue ;
}

int UNIFORMANIMATOR::Get() 
{
	DWORD dwElapse = GetTickCount() - m_dwBeginTime ;
	if (dwElapse >= m_dwTotalCostTime)
		return m_nEndValue ;

	return m_nBeginValue + int(dwElapse) * (m_nEndValue - m_nBeginValue) / int(m_dwTotalCostTime) ;
}

BOOL UNIFORMANIMATOR::ReachEnd() 
{
	DWORD dwElapse = GetTickCount() - m_dwBeginTime ;
	if (dwElapse >= m_dwTotalCostTime)
		return TRUE ;

	return FALSE ;
}

//////////////////////////////////////////////////////////////////////////
// 匀加速动画

BOOL CONSTACCANIMATOR::Init(int nBeginValue, int nEndValue, DWORD dwTotalCostTime, BOOL bSpeedUp)
{
	if (dwTotalCostTime == 0)
		return FALSE ;

	m_nBeginValue = nBeginValue ;
	m_nEndValue = nEndValue ;
	m_dwTotalCostTime = dwTotalCostTime ;
	m_bSpeedUp = bSpeedUp ;

	m_dAcceleration = 2 * double(m_nEndValue - m_nBeginValue) / (m_dwTotalCostTime * m_dwTotalCostTime) ;

	return TRUE ;
}

int CONSTACCANIMATOR::Start() 
{
	m_dwBeginTime = GetTickCount() ;
	return m_nBeginValue ;
}

int CONSTACCANIMATOR::Get() 
{
	DWORD dwElapse = GetTickCount() - m_dwBeginTime ;
	if (dwElapse >= m_dwTotalCostTime)
		return m_nEndValue ;

	int nRes ;
	if (m_bSpeedUp)
	{
		nRes = m_nBeginValue + int(m_dAcceleration * dwElapse * dwElapse / 2) ;

		int d = m_nEndValue > m_nBeginValue ? 1 : -1 ;
		if (((nRes - m_nBeginValue) - (m_nEndValue - m_nBeginValue)) * d > 0)
			nRes = m_nEndValue ;
	}
	else
	{
		DWORD dwInverse = m_dwTotalCostTime - dwElapse ;
		nRes = m_nEndValue - int(m_dAcceleration * dwInverse * dwInverse / 2) ;

		int d = m_nBeginValue > m_nEndValue ? 1 : -1 ;
		if (((nRes - m_nEndValue) - (m_nBeginValue - m_nEndValue)) * d > 0)
			nRes = m_nBeginValue ;
	}

	return nRes ;
}

BOOL CONSTACCANIMATOR::ReachEnd() 
{
	DWORD dwElapse = GetTickCount() - m_dwBeginTime ;
	if (dwElapse >= m_dwTotalCostTime)
		return TRUE ;

	return FALSE ;
}

