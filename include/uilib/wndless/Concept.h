#pragma once
#include "uilib/uilib_exports.h"

//SHELL 是某种 CWndLessShellBase
template <class SHELL>  class CWndLessImpBase;


//IMPT 是某种 CWndLessImpBase, 创建默认实现
template <class IMPT> 
UILIB_EXPORT IMPT *CreateDefaultImp( typename IMPT::BaseType *);


//壳模型
template <class IT, class IMPT> 
class CWndLessShellBase: public IT
{
public: 
	typedef IT BaseType;
	typedef IMPT ImpType;

public:
	CWndLessShellBase ()
	{
		m_wlimp = CreateDefaultImp< IMPT >(this);
	}

	virtual ~CWndLessShellBase()
	{
		if (m_wlimp)
		{
			delete m_wlimp;
			m_wlimp = NULL;
		}
	}

protected:
	typename IMPT* m_wlimp;
};



//实现模型， 每个实现类型都继承于其上一级的壳
template <class SHELL> 
class CWndLessImpBase: public SHELL
{
public:
	CWndLessImpBase () : m_wrapper(NULL)
	{
	}

	virtual ~CWndLessImpBase()
	{
	}

	virtual void SetWrapper(typename SHELL::BaseType * wrapper)
	{
		m_wrapper = wrapper;
		m_wlimp->SetWrapper(wrapper);
	}

protected:
	typename SHELL::BaseType * m_wrapper;

};

