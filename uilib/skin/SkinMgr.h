#pragma once
#include "uilib/skin/ISkin.h"
#include <set>
class CSkinMgr :
	public ISkinMgr
{
public:
	CSkinMgr(void);
	virtual ~CSkinMgr(void);

	virtual ISkin const & GetSkin() const;
	virtual void RegisterSkinWindow(HWND wnd);
	virtual BOOL UnRegisterSkinWindow(HWND wnd);

	// 获得最后一次使用的皮肤
	ISkin * GetLatestSkin();

	// 添加皮肤
	void AddSkin(ISkin * skin);

	// 删除皮肤
	void DeleteSkin(ISkin * skin);

	// 使用皮肤
	void UseSkin(ISkin * skin, BOOL save_to_config);

private:
	ISkin * CSkinMgr::ChangeSkin(ISkin * skin);

	ISkin * m_skin ;
	std::set<HWND> m_all_skin_wnds ;		// all request skin windows
};
