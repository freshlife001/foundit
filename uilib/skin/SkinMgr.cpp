#include "StdAfx.h"
#include "utils/IProfile.h"
#include "uilib/wndless/wluser.h"
#include "Skin.h"
#include "SkinMgr.h"

CSkinMgr::CSkinMgr(void) : m_skin(new DefaultImageSkin)
{
}

CSkinMgr::~CSkinMgr(void)
{
	delete m_skin ;
	m_skin =NULL;

}


ISkin const & CSkinMgr::GetSkin() const
{
	return * m_skin ;

}
void CSkinMgr::RegisterSkinWindow(HWND wnd)
{
	m_all_skin_wnds.insert(wnd) ;
}
BOOL CSkinMgr::UnRegisterSkinWindow(HWND wnd)
{
	std::set<HWND>::iterator pos = m_all_skin_wnds.find(wnd) ;
	if (pos == m_all_skin_wnds.end())
		return FALSE ;

	m_all_skin_wnds.erase(pos) ;

	return TRUE ;

}


ISkin * CSkinMgr::GetLatestSkin()
{

	ISkin * latest_skin = NULL ;
	if (GetGlobalProfile().hasProperty("skin"))
	{
		boost::shared_ptr<IPropertyTree> config ;
		config = GetGlobalProfile().createView("skin") ;

		latest_skin = SkinHelper::GetLatestSkin(config) ;
	}

	if (latest_skin == NULL)
		latest_skin = new DefaultImageSkin ;

	return latest_skin ;
}

void CSkinMgr::AddSkin(ISkin * skin)
{
	if (skin == NULL)
		return ;

	boost::shared_ptr<IPropertyTree> config ;
	config = GetGlobalProfile().createView("skin") ;
	SkinHelper::AddSkin(config, skin) ;

}

void CSkinMgr::DeleteSkin(ISkin * skin)
{
	if (skin == NULL)
		return ;

	boost::shared_ptr<IPropertyTree> config ;
	config = GetGlobalProfile().createView("skin") ;
	SkinHelper::DeleteSkin(config, skin) ;

}

void CSkinMgr::UseSkin(ISkin * skin, BOOL save_to_config)
{
	skin->UpdateUseTime() ;

	ISkin * old_skin = ChangeSkin(skin) ;
	delete old_skin ;

	if (save_to_config)
	{
		boost::shared_ptr<IPropertyTree> config ;
		config = GetGlobalProfile().createView("skin") ;
		SkinHelper::SetLatestSkin(config, skin) ;
	}

	//换肤后清理一下工作集
	if (skin->GetMode() == ISkin::CustomImage)
	{
		::SetProcessWorkingSetSize( GetCurrentProcess(), -1, -1 );
	}

}


ISkin * CSkinMgr::ChangeSkin(ISkin * skin)
{
	ISkin * old_skin = NULL ;
	if (m_skin != skin)
	{
		old_skin = m_skin ;
		m_skin = skin ;

		// 通知注册的界面换肤
		std::set<HWND>::iterator pos ;
		for (pos = m_all_skin_wnds.begin() ; pos != m_all_skin_wnds.end() ; ++ pos)
		{
			::PostMessage((*pos), MSG_SKIN_CHANGED, 0, 0) ;
		}
	}

	return old_skin ;
}


static ISkinMgr* s_skinMgr = NULL;
ISkinMgr * GetSkinMgr()
{
	if (s_skinMgr == NULL)
	{
		s_skinMgr = new CSkinMgr();
	}
	return s_skinMgr;
}