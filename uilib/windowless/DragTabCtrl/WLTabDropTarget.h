#pragma once

// WLTabDropTarget ÃüÁîÄ¿±ê

class WLDragTabCtrlImp;
class WLTabDropTarget : public COleDropTarget
{
public:
	WLTabDropTarget( );
	virtual ~WLTabDropTarget();

	BOOL Register( WLDragTabCtrlImp* pWLWnd );

	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);

private:
	BOOL IsPointInControl( CWnd* pWnd, CPoint point );

private:
	WLDragTabCtrlImp* m_pTabCtrl;

	BOOL m_bEntered;
};


