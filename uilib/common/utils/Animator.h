#pragma once

// 匀变化动画
class UNIFORMANIMATOR
{
public:
	BOOL Init(int nBeginValue, int nEndValue, DWORD dwTotalCostTime) ;

	int Start() ;
	int Get() ;
	BOOL ReachEnd() ;

protected:
	int m_nBeginValue ;
	int m_nEndValue ;
	DWORD m_dwTotalCostTime ;

	DWORD m_dwBeginTime ;

};

// 匀加速动画
class CONSTACCANIMATOR
{
public:
	BOOL Init(int nBeginValue, int nEndValue, DWORD dwTotalCostTime, BOOL bSpeedUp) ;

	int Start() ;
	int Get() ;
	BOOL ReachEnd() ;

protected:
	int m_nBeginValue ;
	int m_nEndValue ;
	DWORD m_dwTotalCostTime ;
	BOOL m_bSpeedUp ;				// 标记加速还是减速

	DWORD m_dwBeginTime ;
	double m_dAcceleration ;

};
