// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include <afxdhtml.h>
#include "DHtmlWnd.h"
#include "logex.h"
#include "utils/strutils.h"

using namespace hdui;

/////////////////////////////////////////////////////////////////////////////
// CDHtmlWnd

BEGIN_MESSAGE_MAP(CDHtmlWnd, CWnd)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(CDHtmlWnd::WM_DESTROYMODELESS, &CDHtmlWnd::OnDestroyModeless)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDHtmlWnd, CWnd)
	ON_EVENT(CDHtmlWnd, AFX_IDC_BROWSER, 252 /* NavigateComplete2 */, _OnNavigateComplete2, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CDHtmlWnd, AFX_IDC_BROWSER, 250 /* BeforeNavigate2 */, _OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CDHtmlWnd, AFX_IDC_BROWSER, 259 /* DocumentComplete */, _OnDocumentComplete, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(CDHtmlWnd, CWnd)


/////////////////////////////////////////////////////////////////////////////
// CBrowserControlSite1

CBrowserControlSite1::CBrowserControlSite1(COleControlContainer* pCtrlCont, CDHtmlWnd *pHandler) :
COleControlSite(pCtrlCont)
{
	ASSERT(pHandler);
	m_pHandler = pHandler;
}

LPUNKNOWN CBrowserControlSite1::GetInterfaceHook(const void *iid)
{
	if (IsEqualIID((REFIID) (*(IID*)iid), IID_IDocHostUIHandler))
		return (IDocHostUIHandler *) this;
	return NULL;
}

// IUnknown methods
STDMETHODIMP CBrowserControlSite1::QueryInterface(REFIID riid, void **ppvObject)
{
	return COleControlSite::ExternalQueryInterface(&riid, ppvObject);
}
STDMETHODIMP_(ULONG) CBrowserControlSite1::AddRef()
{
	return ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBrowserControlSite1::Release()
{
	return ExternalRelease();
}

// IDocHostUIHandler methods
STDMETHODIMP CBrowserControlSite1::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	return m_pHandler->ShowContextMenu(dwID, ppt, pcmdtReserved, pdispReserved);
}
STDMETHODIMP CBrowserControlSite1::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	return m_pHandler->GetHostInfo(pInfo);
}
STDMETHODIMP CBrowserControlSite1::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
{
	return m_pHandler->ShowUI(dwID, pActiveObject, pCommandTarget, pFrame, pDoc);
}
STDMETHODIMP CBrowserControlSite1::HideUI(void)
{
	return m_pHandler->HideUI();
}
STDMETHODIMP CBrowserControlSite1::UpdateUI(void)
{
	return m_pHandler->UpdateUI();
}
STDMETHODIMP CBrowserControlSite1::EnableModeless(BOOL fEnable)
{
	return m_pHandler->EnableModeless(fEnable);
}
STDMETHODIMP CBrowserControlSite1::OnDocWindowActivate(BOOL fActivate)
{
	return m_pHandler->OnDocWindowActivate(fActivate);
}
STDMETHODIMP CBrowserControlSite1::OnFrameWindowActivate(BOOL fActivate)
{
	return m_pHandler->OnFrameWindowActivate(fActivate);
}
STDMETHODIMP CBrowserControlSite1::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow)
{
	return m_pHandler->ResizeBorder(prcBorder, pUIWindow, fRameWindow);
}
STDMETHODIMP CBrowserControlSite1::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
	return m_pHandler->TranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
}
STDMETHODIMP CBrowserControlSite1::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw)
{
	return m_pHandler->GetOptionKeyPath(pchKey, dw);
}
STDMETHODIMP CBrowserControlSite1::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	return m_pHandler->GetDropTarget(pDropTarget, ppDropTarget);
}
STDMETHODIMP CBrowserControlSite1::GetExternal(IDispatch **ppDispatch)
{
	return m_pHandler->GetExternal(ppDispatch);
}
STDMETHODIMP CBrowserControlSite1::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	return m_pHandler->TranslateUrl(dwTranslate, pchURLIn, ppchURLOut);
}
STDMETHODIMP CBrowserControlSite1::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
{
	return m_pHandler->FilterDataObject(pDO, ppDORet);
}

/////////////////////////////////////////////////////////////////////////////
// CDHtmlWnd

CDHtmlWnd::CDHtmlWnd() :
CWnd()
{
	Initialize();
}


CDHtmlWnd::~CDHtmlWnd()
{
}

BOOL CDHtmlWnd::CreateControlSite(COleControlContainer* pContainer, 
									 COleControlSite** ppSite, UINT /* nID */, REFCLSID /* clsid */)
{
	if(ppSite == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CBrowserControlSite1 *pBrowserSite = 
		new CBrowserControlSite1(pContainer, this);
	if (!pBrowserSite)
		return FALSE;

	*ppSite = pBrowserSite;
	return TRUE;
}

HRESULT CDHtmlWnd::GetDHtmlDocument(IHTMLDocument2 **pphtmlDoc)
{
	if(pphtmlDoc == NULL)
	{
		ASSERT(FALSE);
		return E_POINTER;
	}

	*pphtmlDoc = NULL;

	if (m_spHtmlDoc)
	{
		*pphtmlDoc = m_spHtmlDoc;
		(*pphtmlDoc)->AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

void CDHtmlWnd::GetCurrentUrl(CString& szUrl)
{
	szUrl = m_strCurrentUrl;
}

BOOL CDHtmlWnd::CanAccessExternal()
{
	// if the dispatch we have is safe, 
	// we allow access
	if (IsExternalDispatchSafe())
		return TRUE;

	// the external dispatch is not safe, so we check
	// whether the current zone allows for scripting
	// of objects that are not safe for scripting
	if (m_spHtmlDoc == NULL)
		return FALSE;

	CComPtr<IInternetHostSecurityManager> spSecMan;
	m_spHtmlDoc->QueryInterface(IID_IInternetHostSecurityManager,
		(void **) &spSecMan);
	if (spSecMan == NULL)
		return FALSE;

	HRESULT hr = spSecMan->ProcessUrlAction(URLACTION_ACTIVEX_OVERRIDE_OBJECT_SAFETY,
		NULL, 0, NULL, 0, 0, PUAF_DEFAULT);
	if (hr == S_OK)
		return TRUE;
	return FALSE;
}

void CDHtmlWnd::OnBeforeNavigate(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	szUrl; // unused

	if (pDisp != m_pBrowserApp)
		return;
	DisconnectDHtmlEvents();
	m_spHtmlDoc = NULL;
	m_strCurrentUrl.Empty();
}

void CDHtmlWnd::OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	if (pDisp != m_pBrowserApp)
		return;
	IDispatch *pdispDoc = NULL;
	m_pBrowserApp->get_Document(&pdispDoc);
	if (!pdispDoc)
		return;

	ASSERT(m_spHtmlDoc==NULL);

	pdispDoc->QueryInterface(IID_IHTMLDocument2, (void **) &m_spHtmlDoc);

	if (m_bUseHtmlTitle)
	{
		CComBSTR bstrTitle;
		m_spHtmlDoc->get_title(&bstrTitle);
		CString str = CString(bstrTitle);
		SetWindowText(str);
	}

	m_strCurrentUrl = szUrl;

	ConnectDHtmlEvents(pdispDoc);
	pdispDoc->Release();
}

void CDHtmlWnd::OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl)
{
	szUrl; // unused

	if (pDisp != m_pBrowserApp)
		return;
	ConnectDHtmlElementEvents((((DWORD_PTR)static_cast< CDHtmlSinkHandler* >(this)) - (DWORD_PTR)this));
	UpdateData(FALSE);

	HWND hParent = ::GetParent(this->GetSafeHwnd());
	if(::IsWindow(hParent))
		::SendMessage(hParent, WM_DOCUMENT_COMPLETE, 0, NULL);
}

BOOL CDHtmlWnd::LoadFromResource(LPCTSTR lpszResource)
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);

	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];

	int ret = GetModuleFileName(hInstance, lpszModule, _MAX_PATH);

	if (ret == 0 || ret == _MAX_PATH)
		bRetVal = FALSE;
	else
	{
		strResourceURL.Format(_T("res://%s/%s"), lpszModule, lpszResource);
		Navigate(strResourceURL, 0, 0, 0);
	}

	delete [] lpszModule;
	return bRetVal;
}

BOOL CDHtmlWnd::LoadFromResource(UINT nRes)
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);

	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];

	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%d"), lpszModule, nRes);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
		bRetVal = FALSE;

	delete [] lpszModule;
	return bRetVal;
}

void CDHtmlWnd::Navigate(LPCTSTR lpszURL, DWORD dwFlags /*= 0*/, 
							LPCTSTR lpszTargetFrameName /*= NULL*/, LPCTSTR lpszHeaders /*= NULL*/,
							LPVOID lpvPostData /*= NULL*/, DWORD dwPostDataLen /*= 0*/)
{
	CComBSTR bstrURL = lpszURL;

	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
			dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}

	m_pBrowserApp->Navigate(bstrURL,
		COleVariant((long) dwFlags, VT_I4),
		COleVariant(lpszTargetFrameName, VT_BSTR),
		vPostData,
		COleVariant(lpszHeaders, VT_BSTR));
}

void CDHtmlWnd::DestroyModeless()
{
	if (!CDHtmlWnd::WM_DESTROYMODELESS)
		CDHtmlWnd::WM_DESTROYMODELESS = RegisterWindowMessage(_T("DHtmlDialogDestroy"));
	PostMessage(CDHtmlWnd::WM_DESTROYMODELESS, 0, 0);
}

bool CDHtmlWnd::GetJScript(CComPtr<IDispatch>& spDisp)
{
	CComPtr<IHTMLDocument2> pHtmlDoc=NULL;
	CComPtr<IDispatch> spDocDisp;
	HRESULT hr = S_OK;
	if(spDisp == NULL)
	{
		m_pBrowserApp->get_Document(&spDocDisp);
		if (!spDocDisp)
			return FAILED(hr);

		if(spDocDisp)
		{
			hr = spDocDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHtmlDoc);
			if (SUCCEEDED(hr) && pHtmlDoc)
			{
				// get script
				hr = pHtmlDoc->get_Script(&spDisp);
				ATLASSERT(SUCCEEDED(hr));

			}
		}
	}
	return SUCCEEDED(hr);
}

bool CDHtmlWnd::CallJScript(const CString strFunc,CComVariant* pVarResult)
{
	CStringArray paramArray;
	return CallJScript(strFunc, paramArray, pVarResult);
}

bool CDHtmlWnd::CallJScript(const CString strFunc,const CString strArg1,CComVariant* pVarResult)
{
	CStringArray paramArray;
	paramArray.Add(strArg1);
	return CallJScript(strFunc,paramArray,pVarResult);
}

const CString GetSystemErrorMessage(DWORD dwError)
{
	CString strError;
	LPTSTR lpBuffer;

	if(!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,  dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
		(LPTSTR) &lpBuffer, 0, NULL))

	{
		strError = _T("FormatMessage Netive Error") ;
	}
	else
	{
		strError = lpBuffer;
		LocalFree(lpBuffer);
	}
	return strError;
}

bool CDHtmlWnd::CallJScript(const CString strFunc, const CStringArray& paramArray,CComVariant* pVarResult)
{
	CComPtr<IDispatch> spScript=NULL;
	if(!GetJScript(spScript))
	{
		AfxMessageBox (_T("Cannot GetScript"));
		return false;
	}
	CComBSTR bstrMember(strFunc);
	DISPID dispid = NULL;
	HRESULT hr = spScript->GetIDsOfNames(IID_NULL,&bstrMember,1,
		LOCALE_SYSTEM_DEFAULT,&dispid);
	if(FAILED(hr))
	{
// 		AfxMessageBox(GetSystemErrorMessage(hr));
		return false;
	}

	const int arraySize = paramArray.GetSize();

	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs = arraySize;
	dispparams.rgvarg = new VARIANT[dispparams.cArgs];

	for( int i = 0; i < arraySize; i++)
	{
		CComBSTR bstr = paramArray.GetAt(arraySize - 1 - i); // back reading
		bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
		dispparams.rgvarg[i].vt = VT_BSTR;
	}
	dispparams.cNamedArgs = 0;

	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	CComVariant vaResult;
	UINT nArgErr = (UINT)-1;  // initialize to invalid arg

	hr = spScript->Invoke(dispid,IID_NULL,0,
		DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

	delete [] dispparams.rgvarg;
	if(FAILED(hr))
	{
		AfxMessageBox(GetSystemErrorMessage(hr));
		return false;
	}

	if(pVarResult)
	{
		*pVarResult = vaResult;
	}
	return true;
}

void CDHtmlWnd::OnDestroy()
{
	if(m_pFilesDropTarget.get())
	{
		m_pFilesDropTarget->DragRevoke();
		m_pFilesDropTarget = NULL;
	}
	DisconnectDHtmlEvents();
	m_spHtmlDoc = NULL;

	// now tell the browser control we're shutting down
	if (m_pBrowserApp)
	{
		CComPtr<IOleObject> spObject;
		m_pBrowserApp->QueryInterface(IID_IOleObject, (void **) &spObject);
		if (spObject != NULL)
		{
			spObject->Close(OLECLOSE_NOSAVE);
			spObject.Release();
		}
		m_pBrowserApp = NULL;
	}


	__super::OnDestroy();
}

void CDHtmlWnd::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	if (!m_bAttachedControl && m_wndBrowser.m_hWnd)
		m_wndBrowser.MoveWindow(0, 0, cx, cy);	
}

BOOL CDHtmlWnd::Create(
							DWORD dwExStyle,
							DWORD dwStyle,
							LPCTSTR lpszWindowName,
							int x, int y, int cx, int cy,
							HWND hParent,
							HMENU nIDorHMenu
							) 
{

	BOOL ret =  CWnd::CreateEx(
		dwExStyle, 
		0,
		lpszWindowName,
		dwStyle,
		x, y, cx, cy,
		hParent,
		nIDorHMenu,
		0
		) ;

	if (!ret)
	{
		return FALSE;
	}

	AfxEnableControlContainer();

	RECT rectClient;
	GetClientRect(&rectClient);

	// if we've been created from the dynamic template
	// set the caption
	SetWindowText(m_strDlgCaption);

	// check if there is a browser control on the dialog
	// already
	CWnd *pCtrl = GetDlgItem(AFX_IDC_BROWSER);
	LPUNKNOWN lpUnk;
	if (pCtrl)
	{
		lpUnk = pCtrl->GetControlUnknown();
		if (lpUnk && SUCCEEDED(lpUnk->QueryInterface(IID_IWebBrowser2, (void **) &m_pBrowserApp)))
		{
			m_wndBrowser.Attach(pCtrl->m_hWnd);
			m_bAttachedControl = TRUE;
		}
	}
	if (m_pBrowserApp == NULL)
	{
		// create the control window
		m_wndBrowser.CreateControl(CLSID_WebBrowser, NULL,
			WS_VISIBLE | WS_CHILD, rectClient, this, AFX_IDC_BROWSER);
		lpUnk = m_wndBrowser.GetControlUnknown();
		if (FAILED(lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp)))
		{
			m_wndBrowser.DestroyWindow();
			DestroyWindow();
			return TRUE;
		}
	}

	if (m_pBrowserApp)
	{
		m_pBrowserApp->put_Silent( VARIANT_TRUE );
	}

	return TRUE;
}

void CDHtmlWnd::ShowScroll(bool show)
{
	if (!show)
	{
		m_dwHostFlags |= DOCHOSTUIFLAG_SCROLL_NO;
	}
	else
	{
		m_dwHostFlags &= ~DOCHOSTUIFLAG_SCROLL_NO;
	}
}

BEGIN_DHTML_EVENT_MAP(CDHtmlWnd)
DHTML_EVENT(DISPID_EVMETH_ONREADYSTATECHANGE, NULL, OnDocumentReadyStateChange)
END_DHTML_EVENT_MAP()

void CDHtmlWnd::Initialize()
{
	m_dwHostFlags = DOCHOSTUIFLAG_NO3DOUTERBORDER | DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME;
	m_bUseHtmlTitle = FALSE;
	m_bAttachedControl = FALSE;
	m_pBrowserApp = NULL;
	m_dwDHtmlEventSinkCookie = 0;
	m_szHtmlResID = NULL;
	m_nHtmlResID = 0;

	m_pFilesDropTarget = new FilesDropTarget(this);
}

void CDHtmlWnd::_OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel)
{
	Flags; // unused
	TargetFrameName; // unused
	PostData; // unused
	Headers; // unused
	Cancel; // unused

	CString str(V_BSTR(URL));
	OnBeforeNavigate(pDisp, str);
}

void CDHtmlWnd::_OnNavigateComplete2(LPDISPATCH pDisp, VARIANT FAR* URL)
{
	ASSERT(V_VT(URL) == VT_BSTR);

	CString str(V_BSTR(URL));

	OnNavigateComplete(pDisp, str);
}

void CDHtmlWnd::_OnDocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	ASSERT(V_VT(URL) == VT_BSTR);

	CString str(V_BSTR(URL));
	OnDocumentComplete(pDisp, str);
}

HRESULT CDHtmlWnd::OnDocumentReadyStateChange(IHTMLElement *phtmlElem)
{
	phtmlElem; // unused

	CComPtr<IHTMLDocument2> sphtmlDoc;
	GetDHtmlDocument(&sphtmlDoc);
	if (sphtmlDoc)
	{
		CComBSTR bstrState;
		sphtmlDoc->get_readyState(&bstrState);
		if (bstrState)
		{
			if (bstrState==TEXT("complete"))
				ConnectDHtmlElementEvents((((DWORD_PTR)static_cast< CDHtmlSinkHandler* >(this)) - (DWORD_PTR) this));
			else if (bstrState==TEXT("loading"))
				DisconnectDHtmlElementEvents();
		}
	}
	return S_OK;
}

LRESULT CDHtmlWnd::OnDestroyModeless(WPARAM, LPARAM)
{
	DestroyWindow();
	return 0;
}

void CDHtmlWnd::DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, CString& value)
{
	DDX_DHtml_ElementText(szId, dispid, value, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, short& value)
{
	DDX_DHtml_ElementText(szId, dispid, value, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, int& value)
{
	DDX_DHtml_ElementText(szId, dispid, value, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, long& value)
{
	DDX_DHtml_ElementText(szId, dispid, value, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, DWORD& value)
{
	DDX_DHtml_ElementText(szId, dispid, value, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, float& value)
{
	DDX_DHtml_ElementText(szId, dispid, value, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, double& value)
{
	DDX_DHtml_ElementText(szId, dispid, value, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_CheckBox(CDataExchange* pDX, LPCTSTR szId, int& value)
{
	DDX_DHtml_CheckBox(szId, value, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_Radio(CDataExchange* pDX, LPCTSTR szId, long& value)
{
	DDX_DHtml_Radio(szId, value, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_SelectValue(CDataExchange* pDX, LPCTSTR szId, CString& value)
{
	DDX_DHtml_SelectValue(szId, value, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_SelectString(CDataExchange* pDX, LPCTSTR szId, CString& value)
{
	DDX_DHtml_SelectString(szId, value, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_SelectIndex(CDataExchange* pDX, LPCTSTR szId, long& value)
{
	DDX_DHtml_SelectIndex(szId, value, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_AxControl(CDataExchange *pDX, LPCTSTR szId, DISPID dispid, VARIANT& var)
{
	DDX_DHtml_AxControl(szId, dispid, var, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_AxControl(CDataExchange *pDX, LPCTSTR szId, LPCTSTR szPropName, VARIANT& var)
{
	DDX_DHtml_AxControl(szId, szPropName, var, pDX->m_bSaveAndValidate);
}

void CDHtmlWnd::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, CString& value, BOOL bSave)
{
	CComPtr<IHTMLDocument2> sphtmlDoc;
	GetDHtmlDocument(&sphtmlDoc);
	if (sphtmlDoc == NULL)
		return;

	CComVariant var;
	if (bSave)
	{
		var = GetElementProperty(szId, dispid);
		var.ChangeType(VT_BSTR);
		value = var.bstrVal;
	}
	else
	{
		var = (LPCTSTR)value;
		SetElementProperty(szId, dispid, &var);
	}
}

void CDHtmlWnd::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, short& value, BOOL bSave)
{
	if (bSave)
		DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%sd"), AFX_IDP_PARSE_INT, bSave, &value);
	else
		DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%hd"), AFX_IDP_PARSE_INT, bSave, value);
}

void CDHtmlWnd::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, int& value, BOOL bSave)
{
	if (bSave)
		DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%d"), AFX_IDP_PARSE_INT, bSave, &value);
	else
		DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%d"), AFX_IDP_PARSE_INT, bSave, value);
}

void CDHtmlWnd::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, long& value, BOOL bSave)
{
	if (bSave)
		DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%ld"), AFX_IDP_PARSE_INT, bSave, &value);
	else
		DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%ld"), AFX_IDP_PARSE_INT, bSave, value);
}

void CDHtmlWnd::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, DWORD& value, BOOL bSave)
{
	if (bSave)
		DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%lu"), AFX_IDP_PARSE_UINT, bSave, &value);
	else
		DDX_DHtml_ElementTextWithFormat(szId, dispid, _T("%lu"), AFX_IDP_PARSE_UINT, bSave, value);
}

void CDHtmlWnd::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, float& value, BOOL bSave)
{
	DDX_DHtml_ElementTextFloatFormat(szId, dispid, &value, value, FLT_DIG, bSave);
}

void CDHtmlWnd::DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, double& value, BOOL bSave)
{
	DDX_DHtml_ElementTextFloatFormat(szId, dispid, &value, value, DBL_DIG, bSave);
}

/////////////////////////////////////////////////////////////////////////////
// Data exchange for special controls
void CDHtmlWnd::DDX_DHtml_CheckBox(LPCTSTR szId, int& value, BOOL bSave)
{
	COleVariant var;

	CComPtr<IHTMLDocument2> sphtmlDoc;
	GetDHtmlDocument(&sphtmlDoc);
	if (sphtmlDoc == NULL)
		return;

	CComPtr<IHTMLOptionButtonElement> spOptionButton;
	HRESULT hr = S_OK;

	hr = GetElementInterface(szId, __uuidof(IHTMLOptionButtonElement), (void **) &spOptionButton);
	if (spOptionButton == NULL)
		goto Error;


	if (bSave)
	{
		VARIANT_BOOL bIndeterminate;
		VARIANT_BOOL bChecked;
		hr = spOptionButton->get_checked(&bChecked);
		if (FAILED(hr))
			goto Error;
		hr = spOptionButton->get_indeterminate(&bIndeterminate);
		if (FAILED(hr))
			goto Error;
		value = ((bChecked != ATL_VARIANT_FALSE) ? BST_CHECKED : BST_UNCHECKED) | 
			((bIndeterminate != ATL_VARIANT_FALSE) ? BST_INDETERMINATE : 0);
	}
	else
	{
		hr = spOptionButton->put_checked((value & BST_CHECKED) ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		if (FAILED(hr))
			goto Error;
		hr = spOptionButton->put_indeterminate((value & BST_INDETERMINATE) ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		if (FAILED(hr))
			goto Error;
	}
Error:
	if (FAILED(hr))
	{
		TRACE(traceHtml, 0, "Failed DDX_DHtml_CheckBox\n");
	}
}

void CDHtmlWnd::DDX_DHtml_Radio(LPCTSTR szId, long& value, BOOL bSave)
{
	COleVariant var;

	CComPtr<IHTMLDocument2> sphtmlDoc;
	GetDHtmlDocument(&sphtmlDoc);
	if (sphtmlDoc == NULL)
		return;

	CComPtr<IHTMLOptionButtonElement> spOptionButton;
	CComPtr<IDispatch> spdispCollection;
	CComPtr<IHTMLElementCollection> spElementColl;
	BOOL bIsCollection = FALSE;
	CComPtr<IDispatch> spdispElem;

	HRESULT hr = S_OK;
	COleVariant varIndex;
	COleVariant varEmpty;
	varIndex.vt = VT_I4;
	varIndex.lVal = 0;

	// get the radio buttons in the group
	hr = GetElement(szId, &spdispCollection, &bIsCollection);
	if (spdispCollection && bIsCollection)
	{
		hr = spdispCollection->QueryInterface(__uuidof(IHTMLElementCollection), (void **) &spElementColl);
		if (spElementColl == NULL)
		{
			TRACE(traceHtml, 0, "Error: Collection didn't support IHTMLElementCollection!\n");
			ASSERT(FALSE);
			goto Error;
		}

		if (bSave)
			value = -1;     // value if none found

		long lCount = 0;
		spElementColl->get_length(&lCount);

		for (long lCntr = 0; lCntr < lCount; lCntr++)
		{				
			spdispElem = NULL;
			spOptionButton = NULL;
			varIndex.lVal = lCntr;
			hr = spElementColl->item(varIndex, varEmpty, &spdispElem);
			if (spdispElem == NULL)
				break;
			hr = spdispElem->QueryInterface(__uuidof(IHTMLOptionButtonElement), (void **) &spOptionButton);

			if (bSave)
			{
				VARIANT_BOOL bChecked;
				hr = spOptionButton->get_checked(&bChecked);
				if (FAILED(hr))
					goto Error;
				if (bChecked != ATL_VARIANT_FALSE)
				{
					value = varIndex.lVal;
					break;
				}
			}
			else
			{
				if (varIndex.lVal == value)
				{
					spOptionButton->put_checked(ATL_VARIANT_TRUE);
					break;
				}
			}
		};
	}
Error:
	if (FAILED(hr))
	{
		TRACE(traceHtml, 0, "Warning: Failed DDX_DHtml_Radio\n");
	}
}

void CDHtmlWnd::DDX_DHtml_SelectValue(LPCTSTR szId, CString& value, BOOL bSave)
{
	CComPtr<IHTMLDocument2> sphtmlDoc;
	GetDHtmlDocument(&sphtmlDoc);
	if (sphtmlDoc == NULL)
		return;

	CComBSTR bstrText;
	CComPtr<IHTMLSelectElement> spSelect;
	HRESULT hr = GetElementInterface(szId, __uuidof(IHTMLSelectElement), (void **) &spSelect);
	if (FAILED(hr))
		goto Error;

	if (bSave)
	{
		spSelect->get_value(&bstrText);
		if (bstrText)
			value = bstrText;
	}
	else
	{
		bstrText.Attach(value.AllocSysString());
		spSelect->put_value(bstrText);
	}

Error:
	return;
}

void CDHtmlWnd::DDX_DHtml_SelectString(LPCTSTR szId, CString& value, BOOL bSave)
{
	CComPtr<IHTMLDocument2> sphtmlDoc;
	GetDHtmlDocument(&sphtmlDoc);
	if (sphtmlDoc == NULL)
		return;

	COleVariant varEmpty, varIndex;

	CComPtr<IHTMLSelectElement> spSelect;
	CComPtr<IDispatch> spdispOption;
	CComPtr<IHTMLOptionElement> spOption;
	CComBSTR bstrText;
	HRESULT hr = S_OK;
	long lIndex=-1;

	hr = GetElementInterface(szId, __uuidof(IHTMLSelectElement), (void **) &spSelect);
	if (spSelect == NULL)
		return;

	if (bSave)
	{
		// get the selected item
		value.Empty();
		spSelect->get_selectedIndex(&lIndex);
		if (lIndex >= 0)
		{
			varIndex = lIndex;

			spSelect->item(varIndex, varEmpty, &spdispOption);
			if (spdispOption)
			{
				spdispOption->QueryInterface(__uuidof(IHTMLOptionElement), (void **) &spOption);
				if (spOption)
				{
					spOption->get_text(&bstrText);
					if (bstrText)
						value = bstrText;
				}
			}
		}
	}
	else
	{
		bstrText.Attach(value.AllocSysString());
		lIndex = Select_FindString(spSelect, bstrText, FALSE);
		spSelect->put_selectedIndex(lIndex);
	}
}

void CDHtmlWnd::DDX_DHtml_SelectIndex(LPCTSTR szId, long& value, BOOL bSave)
{
	CComPtr<IHTMLDocument2> sphtmlDoc;
	GetDHtmlDocument(&sphtmlDoc);
	if (sphtmlDoc == NULL)
		return;

	CComPtr<IHTMLSelectElement> spSelect;
	HRESULT hr;

	hr = GetElementInterface(szId, __uuidof(IHTMLSelectElement), (void **) &spSelect);
	if (spSelect == NULL)
		return;

	if (bSave)
		spSelect->get_selectedIndex(&value);
	else
		spSelect->put_selectedIndex(value);
}

void CDHtmlWnd::DDX_DHtml_AxControl(LPCTSTR szId, DISPID dispid, VARIANT& var, BOOL bSave)
{
	if (bSave)
		var = GetControlProperty(szId, dispid);
	else
		SetControlProperty(szId, dispid, &var);
}

void CDHtmlWnd::DDX_DHtml_AxControl(LPCTSTR szId, LPCTSTR szPropName, VARIANT& var, BOOL bSave)
{
	if (bSave)
		var = GetControlProperty(szId, szPropName);
	else
		SetControlProperty(szId, szPropName, &var);
}

void CDHtmlWnd::OnDDXError(LPCTSTR szId, UINT nIDPrompt, BOOL /*bSave*/)
{
	AfxMessageBox(nIDPrompt);
	// default implementation just sets the
	// focus back to the offending element
	SetFocusToElement(szId);
	CComPtr<IHTMLEventObj> sphtmlEvent;
	GetEvent(&sphtmlEvent);
	if (sphtmlEvent)
		sphtmlEvent->put_cancelBubble(ATL_VARIANT_TRUE);
	AfxThrowUserException();
}

void __cdecl CDHtmlWnd::DDX_DHtml_ElementTextWithFormat(LPCTSTR szId,
														   DISPID dispid, LPCTSTR lpszFormat, UINT nIDPrompt, BOOL bSave, ...)
														   // only supports windows output formats - no floating point
{
	va_list pData;
	va_start(pData, bSave);

	CString value;

	if (bSave)
	{
		DDX_DHtml_ElementText(szId, dispid, value, bSave);
		// the following works for %d, %u, %ld, %lu
		if (!_AfxSimpleScanf(value, lpszFormat, pData))
		{
			// set the focus to the offending element
			SetFocusToElement(szId);

			OnDDXError(szId, nIDPrompt, bSave);
			va_end(pData);
			return;
		}
	}
	else
	{
		CString strTemp ;
		strTemp.FormatV(lpszFormat,pData);
		// does not support floating point numbers - see dlgfloat.cpp
		DDX_DHtml_ElementText(szId, dispid, strTemp, bSave);
	}
	va_end(pData);
}

void CDHtmlWnd::DDX_DHtml_ElementTextFloatFormat(LPCTSTR szId,
													DISPID dispid, void* pData, double value, int nSizeGcvt, BOOL bSave)
{
	ATLASSERT(pData != NULL);

	CString strValue;
	if (bSave)
	{
		DDX_DHtml_ElementText(szId, dispid, strValue, bSave);
		double d;
		if (!_AfxSimpleFloatParse(strValue, d))
		{
			OnDDXError(szId, AFX_IDP_PARSE_REAL, bSave);
		}
		if (nSizeGcvt == FLT_DIG)
			*((float*)pData) = (float)d;
		else
			*((double*)pData) = d;
	}
	else
	{
		CString szBuffer;
		szBuffer.Format(_T("%.*g"), nSizeGcvt, value);
		DDX_DHtml_ElementText(szId, dispid, szBuffer, bSave);
	}
}


BOOL CDHtmlWnd::_AfxSimpleScanf(LPCTSTR lpszText,
								   LPCTSTR lpszFormat, va_list pData)
{
	ATLASSERT(lpszText != NULL);
	ATLASSERT(lpszFormat != NULL);

	ATLASSERT(*lpszFormat == '%');
	lpszFormat++;        // skip '%'

	BOOL bLong = FALSE;
	BOOL bShort = FALSE;
	if (*lpszFormat == 'l')
	{
		bLong = TRUE;
		lpszFormat++;
	}
	else if (*lpszFormat == 's')
	{
		bShort = TRUE;
		lpszFormat++;
	}

	ATLASSERT(*lpszFormat == 'd' || *lpszFormat == 'u');
	ATLASSERT(lpszFormat[1] == '\0');

	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;
	TCHAR chFirst = lpszText[0];
	long l, l2;
	if (*lpszFormat == 'd')
	{
		// signed
		l = _tcstol(lpszText, (LPTSTR*)&lpszText, 10);
		l2 = (int)l;
	}
	else
	{
		// unsigned
		if (*lpszText == '-')
			return FALSE;
		l = (long)_tcstoul(lpszText, (LPTSTR*)&lpszText, 10);
		l2 = (unsigned int)l;
	}
	if (l == 0 && chFirst != '0')
		return FALSE;   // could not convert

	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;
	if (*lpszText != '\0')
		return FALSE;   // not terminated properly

	if (bShort)
	{
		if ((short)l != l)
			return FALSE;   // too big for short
		*va_arg(pData, short*) = (short)l;
	}
	else
	{
		ATLASSERT(sizeof(long) == sizeof(int));
		ATLASSERT(l == l2);
		*va_arg(pData, long*) = l;
	}

	// all ok
	return TRUE;
}

BOOL CDHtmlWnd::_AfxSimpleFloatParse(LPCTSTR lpszText, double& d)
{
	ATLASSERT(lpszText != NULL);
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	TCHAR chFirst = lpszText[0];
	d = _tcstod(lpszText, (LPTSTR*)&lpszText);
	if (d == 0.0 && chFirst != '0')
		return FALSE;   // could not convert
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	if (*lpszText != '\0')
		return FALSE;   // not terminated properly

	return TRUE;
}

long CDHtmlWnd::Select_FindString(IHTMLSelectElement *pSelect, BSTR bstr, BOOL /*fExact*/)
{
	long lIndexFound = -1;
	COleVariant varIndex, varEmpty;
	CComPtr<IDispatch> spdispOption;
	CComPtr<IHTMLOptionElement> spOption;
	long lCount = 0;

	HRESULT hr = pSelect->get_length(&lCount);
	if (FAILED(hr))
		goto Error;
	varIndex.vt = VT_I4;
	// loop through the items searching for the string
	for (varIndex.lVal=0; varIndex.lVal<lCount; varIndex.lVal++)
	{
		pSelect->item(varIndex, varEmpty, &spdispOption);
		if (spdispOption)
		{
			spdispOption->QueryInterface(__uuidof(IHTMLOptionElement), (void **) &spOption);
			if (spOption)
			{
				CComBSTR bstrText;
				spOption->get_text(&bstrText);
				if (bstrText && !wcscmp(bstrText, bstr))
				{
					// we found it
					lIndexFound = varIndex.lVal;
					break;
				}
				spOption = NULL;
			}
			spdispOption = NULL;
		}
	}

Error:
	return lIndexFound;
}

void CDHtmlWnd::SetFocusToElement(LPCTSTR szId)
{
	// check if the element is a control element
	CComPtr<IHTMLControlElement> sphtmlCtrlElem;
	HRESULT hr = GetElementInterface(szId, &sphtmlCtrlElem);
	if (sphtmlCtrlElem)
	{
		sphtmlCtrlElem->focus();
		return;
	}

	// check if the element is an anchor element
	CComPtr<IHTMLAnchorElement> sphtmlAnchorElem;
	hr = GetElementInterface(szId, &sphtmlAnchorElem);
	if (sphtmlAnchorElem)
	{
		sphtmlAnchorElem->focus();
		return;
	}

	// otherwise all we can do is scroll the element into view
	CComPtr<IHTMLElement> sphtmlElem;
	hr = GetElementInterface(szId, &sphtmlElem);
	if (sphtmlElem)
	{
		CComVariant var;
		var.vt = VT_BOOL;
		var.boolVal = ATL_VARIANT_TRUE;
		sphtmlElem->scrollIntoView(var);
	}
}

HRESULT CDHtmlWnd::ConnectDHtmlEvents(IUnknown *punkDoc)
{
	return ConnectToConnectionPoint(punkDoc, __uuidof(HTMLDocumentEvents), &m_dwDHtmlEventSinkCookie);
}

void CDHtmlWnd::DisconnectDHtmlEvents()
{
	CComPtr<IHTMLDocument2> sphtmlDoc;
	GetDHtmlDocument(&sphtmlDoc);

	if (sphtmlDoc == NULL)
		return;
	DisconnectFromConnectionPoint(sphtmlDoc, __uuidof(HTMLDocumentEvents), m_dwDHtmlEventSinkCookie);
	DisconnectDHtmlElementEvents();
}

HRESULT CDHtmlWnd::ConnectDHtmlElementEvents(DWORD_PTR dwThunkOffset /*= 0*/)
{
	HRESULT hr = S_OK;
	const DHtmlEventMapEntry* pEventMap = GetDHtmlEventMap();
	if (!pEventMap)
		return hr;

	for (int i=0; pEventMap[i].nType != DHTMLEVENTMAPENTRY_END; i++)
	{
		if (pEventMap[i].nType==DHTMLEVENTMAPENTRY_ELEMENT)
		{
			// an element name must be specified when using element events
			ATLASSERT(pEventMap[i].szName);

			// connect to the element's event sink
			CComPtr<IDispatch> spdispElement;
			GetElement(pEventMap[i].szName, &spdispElement);
			if (spdispElement)
			{
				if (!IsSinkedElement(spdispElement))
				{
					CDHtmlElementEventSink *pSink = NULL;
					ATLTRY(pSink = new CDHtmlElementEventSink(this, spdispElement));
					if (pSink == NULL)
						return E_OUTOFMEMORY;
					hr = AtlAdvise(spdispElement, pSink, __uuidof(IDispatch), &pSink->m_dwCookie);
					if (SUCCEEDED(hr))
						m_SinkedElements.Add(pSink);
					else
						delete pSink;
#ifdef _DEBUG
					if (FAILED(hr))
						TRACE(traceHtml, 0, "Warning: Failed to connect to ConnectionPoint!\n");
#endif
				}
			}
		}
		else if (pEventMap[i].nType==DHTMLEVENTMAPENTRY_CONTROL)
		{
			// check if we already have a sink connected to this control
			if (!FindSinkForObject(pEventMap[i].szName))
			{
				// create a new sink and
				// connect it to the element's event sink
				CComPtr<IDispatch> spdispElement;
				GetElement(pEventMap[i].szName, &spdispElement);
				if (spdispElement)
				{
					CComPtr<IHTMLObjectElement> sphtmlObj;
					spdispElement->QueryInterface(__uuidof(IHTMLObjectElement), (void **) &sphtmlObj);
					if (sphtmlObj)
					{
						CComPtr<IDispatch> spdispControl;
						sphtmlObj->get_object(&spdispControl);
						if (spdispControl)
						{
							// create a new control sink to connect to the control's events
							CDHtmlControlSink *pSink = NULL; 
							ATLTRY(pSink = new CDHtmlControlSink(spdispControl, this, pEventMap[i].szName, dwThunkOffset));
							if (pSink == NULL)
								return E_OUTOFMEMORY;
							m_ControlSinks.Add(pSink);
						}
					}
				}
			}
		}
	}
	return hr;
}

BOOL CDHtmlWnd::FindSinkForObject(LPCTSTR szName)
{	
	int nLength = m_ControlSinks.GetSize();
	if (nLength > 0)
	{
		ENSURE_ARG(szName!=NULL);
	}
	for (int i=0; i<nLength; i++)
	{
		if (!_tcscmp(szName, m_ControlSinks[i]->m_szControlId))
			return TRUE;
	}
	return FALSE;
}

BOOL CDHtmlWnd::IsSinkedElement(IDispatch *pdispElem)
{
	ENSURE_ARG(pdispElem!=NULL);
	CComPtr<IUnknown> spunk;
	pdispElem->QueryInterface(__uuidof(IUnknown), (void **) &spunk);
	if (!spunk)
		return FALSE;
	for (int i=0; i<m_SinkedElements.GetSize(); i++)
	{
		if (spunk == m_SinkedElements[i]->m_spunkElem)
			return TRUE;
	}
	return FALSE;
}

void CDHtmlWnd::DisconnectDHtmlElementEvents()
{
	const DHtmlEventMapEntry* pEventMap = GetDHtmlEventMap();

	if (!pEventMap)
		return;

	int i;

	// disconnect from element events
	for (i=0; i<m_SinkedElements.GetSize(); i++)
	{
		CDHtmlElementEventSink *pSink = m_SinkedElements[i];
		AtlUnadvise(pSink->m_spunkElem, __uuidof(IDispatch), pSink->m_dwCookie);
		delete pSink;
	}
	m_SinkedElements.RemoveAll();

	// disconnect from control events
	for (i=0; i<m_ControlSinks.GetSize(); i++)
	{
		DisconnectFromConnectionPoint(m_ControlSinks[i]->m_spunkObj, 
			m_ControlSinks[i]->m_iid, m_ControlSinks[i]->m_dwCookie);
		delete m_ControlSinks[i];
	}
	m_ControlSinks.RemoveAll();
	return;
}

HRESULT CDHtmlWnd::GetElement(LPCTSTR szElementId, IDispatch **ppdisp, 
								 BOOL *pbCollection /*= NULL*/)
{
	ENSURE_ARG(ppdisp!=NULL);
	CComPtr<IHTMLElementCollection> sphtmlAll;
	CComPtr<IHTMLElementCollection> sphtmlColl;
	CComPtr<IDispatch> spdispElem;
	CComVariant varName;
	CComVariant varIndex;
	HRESULT hr = S_OK;
	CComPtr<IHTMLDocument2> sphtmlDoc;

	*ppdisp = NULL;

	if (pbCollection)
		*pbCollection = FALSE;

	hr = GetDHtmlDocument(&sphtmlDoc);
	if (sphtmlDoc == NULL)
		return hr;

	if(szElementId == NULL)
		return E_INVALIDARG;

	const CString strElementId(szElementId);
	varName.vt = VT_BSTR;
	varName.bstrVal = strElementId.AllocSysString();

#ifndef _UNICODE	
	if (!varName.bstrVal)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}
#endif

	hr = sphtmlDoc->get_all(&sphtmlAll);
	if (sphtmlAll == NULL)
		goto Error;
	hr = sphtmlAll->item(varName, varIndex, &spdispElem);
	if (spdispElem == NULL)
	{
		hr = E_NOINTERFACE;
		goto Error;
	}

	spdispElem->QueryInterface(__uuidof(IHTMLElementCollection), (void **) &sphtmlColl);
	if (sphtmlColl)
	{
		if (pbCollection)
			*pbCollection = TRUE;
#ifdef _DEBUG
		else
		{
			TRACE(traceHtml, 0, "Warning: duplicate IDs or NAMEs.\n");
			ATLASSERT(FALSE);
		}
#endif

	}
Error:
	if (SUCCEEDED(hr))
	{
		*ppdisp = spdispElem;
		if (spdispElem)
			(*ppdisp)->AddRef();
	}
	return hr;
}

HRESULT CDHtmlWnd::GetElement(LPCTSTR szElementId, IHTMLElement **pphtmlElement)
{
	return GetElementInterface(szElementId, __uuidof(IHTMLElement), (void **) pphtmlElement);
}

HRESULT CDHtmlWnd::GetElementInterface(LPCTSTR szElementId, REFIID riid, void **ppvObj)
{
	ENSURE_ARG(ppvObj!=NULL);
	HRESULT hr = E_NOINTERFACE;
	*ppvObj = NULL;
	CComPtr<IDispatch> spdispElem;

	hr = GetElement(szElementId, &spdispElem);

	if (spdispElem)
		hr = spdispElem->QueryInterface(riid, ppvObj);
	return hr;
}

BSTR CDHtmlWnd::GetElementText(LPCTSTR szElementId)
{
	BSTR bstrText = NULL;
	CComPtr<IHTMLElement> sphtmlElem;
	GetElement(szElementId, &sphtmlElem);
	if (sphtmlElem)
		sphtmlElem->get_innerText(&bstrText);
	return bstrText;
}

void CDHtmlWnd::SetElementText(LPCTSTR szElementId, BSTR bstrText)
{
	CComPtr<IHTMLElement> sphtmlElem;
	GetElement(szElementId, &sphtmlElem);
	if (sphtmlElem)
		sphtmlElem->put_innerText(bstrText);
}

void CDHtmlWnd::SetElementText(IUnknown *punkElem, BSTR bstrText)
{
	ENSURE_ARG(punkElem!=NULL);
	CComPtr<IHTMLElement> sphtmlElem;
	punkElem->QueryInterface(__uuidof(IHTMLElement), (void **) &sphtmlElem);
	if (sphtmlElem != NULL)
		sphtmlElem->put_innerText(bstrText);
}

BSTR CDHtmlWnd::GetElementHtml(LPCTSTR szElementId)
{
	BSTR bstrText = NULL;
	CComPtr<IHTMLElement> sphtmlElem;
	GetElement(szElementId, &sphtmlElem);
	if (sphtmlElem)
		sphtmlElem->get_innerHTML(&bstrText);
	return bstrText;
}

void CDHtmlWnd::SetElementHtml(LPCTSTR szElementId, BSTR bstrText)
{
	CComPtr<IHTMLElement> sphtmlElem;
	GetElement(szElementId, &sphtmlElem);
	if (sphtmlElem)
		sphtmlElem->put_innerHTML(bstrText);
}

void CDHtmlWnd::SetElementHtml(IUnknown *punkElem, BSTR bstrText)
{
	ENSURE_ARG(punkElem!=NULL);
	CComPtr<IHTMLElement> sphtmlElem;
	punkElem->QueryInterface(__uuidof(IHTMLElement), (void **) &sphtmlElem);
	if (sphtmlElem != NULL)
		sphtmlElem->put_innerHTML(bstrText);
}

VARIANT CDHtmlWnd::GetElementProperty(LPCTSTR szElementId, DISPID dispid)
{
	VARIANT varRet;
	CComPtr<IDispatch> spdispElem;
	varRet.vt = VT_EMPTY;
	GetElement(szElementId, &spdispElem);
	if (spdispElem)
	{
		DISPPARAMS dispparamsNoArgs = { NULL, NULL, 0, 0 };
		spdispElem->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, 
			DISPATCH_PROPERTYGET, &dispparamsNoArgs, &varRet, NULL, NULL);
	}
	return varRet;
}

void CDHtmlWnd::SetElementProperty(LPCTSTR szElementId, DISPID dispid, VARIANT *pVar)
{
	CComPtr<IDispatch> spdispElem;

	GetElement(szElementId, &spdispElem);
	if (spdispElem)
	{
		DISPPARAMS dispparams = {NULL, NULL, 1, 1};
		dispparams.rgvarg = pVar;
		DISPID dispidPut = DISPID_PROPERTYPUT;
		dispparams.rgdispidNamedArgs = &dispidPut;

		spdispElem->Invoke(dispid, IID_NULL,
			LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT,
			&dispparams, NULL, NULL, NULL);
	}
}

HRESULT CDHtmlWnd::GetControlDispatch(LPCTSTR szId, IDispatch **ppdisp)
{
	HRESULT hr = S_OK;
	CComPtr<IDispatch> spdispElem;

	hr = GetElement(szId, &spdispElem);

	if (spdispElem)
	{
		CComPtr<IHTMLObjectElement> sphtmlObj;

		hr = spdispElem.QueryInterface(&sphtmlObj);
		if (sphtmlObj)
		{
			spdispElem.Release();
			hr = sphtmlObj->get_object(ppdisp);
		}
	}
	return hr;
}

VARIANT CDHtmlWnd::GetControlProperty(IDispatch *pdispControl, DISPID dispid)
{
	VARIANT varRet;
	varRet.vt = VT_EMPTY;
	if (pdispControl)
	{
		DISPPARAMS dispparamsNoArgs = { NULL, NULL, 0, 0 };
		pdispControl->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, 
			DISPATCH_PROPERTYGET, &dispparamsNoArgs, &varRet, NULL, NULL);
	}
	return varRet;
}

VARIANT CDHtmlWnd::GetControlProperty(LPCTSTR szId, DISPID dispid)
{
	CComPtr<IDispatch> spdispElem;

	GetControlDispatch(szId, &spdispElem);
	return GetControlProperty(spdispElem, dispid);
}

VARIANT CDHtmlWnd::GetControlProperty(LPCTSTR szId, LPCTSTR szPropName)
{
	CComVariant varEmpty;
	CComPtr<IDispatch> spdispElem;

	GetControlDispatch(szId, &spdispElem);
	if (!spdispElem)
		return varEmpty;

	DISPID dispid;
	USES_CONVERSION;
	LPOLESTR pPropName = (LPOLESTR)T2COLE(szPropName);
	HRESULT hr = spdispElem->GetIDsOfNames(IID_NULL, &pPropName, 1, LOCALE_USER_DEFAULT, &dispid);
	if (SUCCEEDED(hr))
		return GetControlProperty(spdispElem, dispid);
	return varEmpty;
}

void CDHtmlWnd::SetControlProperty(IDispatch *pdispControl, DISPID dispid, VARIANT *pVar)
{
	if (pdispControl != NULL)
	{
		DISPPARAMS dispparams = {NULL, NULL, 1, 1};
		dispparams.rgvarg = pVar;
		DISPID dispidPut = DISPID_PROPERTYPUT;
		dispparams.rgdispidNamedArgs = &dispidPut;

		pdispControl->Invoke(dispid, IID_NULL,
			LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT,
			&dispparams, NULL, NULL, NULL);
	}
}

void CDHtmlWnd::SetControlProperty(LPCTSTR szElementId, DISPID dispid, VARIANT *pVar)
{
	CComPtr<IDispatch> spdispElem;
	GetControlDispatch(szElementId, &spdispElem);

	SetControlProperty(spdispElem, dispid, pVar);
}

void CDHtmlWnd::SetControlProperty(LPCTSTR szElementId, LPCTSTR szPropName, VARIANT *pVar)
{
	CComPtr<IDispatch> spdispElem;
	GetControlDispatch(szElementId, &spdispElem);
	if (!spdispElem)
		return;
	DISPID dispid;
	USES_CONVERSION;
	LPOLESTR pPropName = (LPOLESTR)T2COLE(szPropName);
	HRESULT hr = spdispElem->GetIDsOfNames(IID_NULL, &pPropName, 1, LOCALE_USER_DEFAULT, &dispid);
	if (SUCCEEDED(hr))
		SetControlProperty(spdispElem, dispid, pVar);
}

HRESULT CDHtmlWnd::GetEvent(IHTMLEventObj **ppEventObj)
{
	ENSURE_ARG(ppEventObj!=NULL);
	CComPtr<IHTMLWindow2> sphtmlWnd;
	CComPtr<IHTMLDocument2> sphtmlDoc;

	*ppEventObj = NULL;

	HRESULT hr = GetDHtmlDocument(&sphtmlDoc);
	if (sphtmlDoc == NULL)
		return hr;

	hr = sphtmlDoc->get_parentWindow(&sphtmlWnd);
	if (FAILED(hr))
		goto Error;
	hr = sphtmlWnd->get_event(ppEventObj);

Error:
	return hr;
}

void CDHtmlWnd::SetHostFlags(DWORD dwFlags)
{
	m_dwHostFlags = dwFlags;
}

void CDHtmlWnd::SetExternalDispatch(IDispatch *pdispExternal)
{
	m_spExternalDisp = pdispExternal;
}

BOOL CDHtmlWnd::IsExternalDispatchSafe()
{
	return FALSE;
}


STDMETHODIMP CDHtmlWnd::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
{
	dwID; // unused
	ppt; // unused
	pcmdtReserved; // unused
	pdispReserved; // unused

// 	AfxMessageBox(_T("OnShowContextMenu"));
	return S_OK;
}
STDMETHODIMP CDHtmlWnd::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	pInfo->dwFlags = m_dwHostFlags;
	return S_OK;
}
STDMETHODIMP CDHtmlWnd::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
{
	dwID; // unused
	pActiveObject; // unused
	pCommandTarget; // unused
	pFrame; // unused
	pDoc; // unused

	return S_FALSE;
}
STDMETHODIMP CDHtmlWnd::HideUI(void)
{
	return E_NOTIMPL;
}
STDMETHODIMP CDHtmlWnd::UpdateUI(void)
{
	return E_NOTIMPL;
}
STDMETHODIMP CDHtmlWnd::EnableModeless(BOOL fEnable)
{
	fEnable; // unused

	return E_NOTIMPL;
}
STDMETHODIMP CDHtmlWnd::OnDocWindowActivate(BOOL fActivate)
{
	fActivate; // unused

	return E_NOTIMPL;
}
STDMETHODIMP CDHtmlWnd::OnFrameWindowActivate(BOOL fActivate)
{
	fActivate; // unused

	return E_NOTIMPL;
}
STDMETHODIMP CDHtmlWnd::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow)
{
	prcBorder; // unused
	pUIWindow; // unused
	fRameWindow; // unused

	return E_NOTIMPL;
}
STDMETHODIMP CDHtmlWnd::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
	lpMsg; // unused
	pguidCmdGroup; // unused
	nCmdID; // unused

	return S_FALSE;
}
STDMETHODIMP CDHtmlWnd::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw)
{
	pchKey; // unused
	dw; // unused

	return E_NOTIMPL;
}
STDMETHODIMP CDHtmlWnd::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
	pDropTarget; // unused

	m_pFilesDropTarget->DragRegister(this->GetSafeHwnd());
	m_pFilesDropTarget->AddRef();
	*ppDropTarget = m_pFilesDropTarget;
	return S_OK;
}

DWORD CDHtmlWnd::FilesDropTarget::OnDragEnter(IDataObject* data_object,
								   DWORD key_state,
								   POINT cursor_position,
								   DWORD effect)
{
	if(m_pHtml == NULL)
		return DROPEFFECT_NONE;

	COleDataObject data;
	data.Attach(data_object, FALSE);
	if(!data.IsDataAvailable(CF_HDROP))
	{
		data.Detach();
		return DROPEFFECT_NONE;
	}

	Json::FastWriter writer;
	Json::Value params;
	params["point_x"] = cursor_position.x;
	params["point_y"] = cursor_position.y;
	params["key_state"] = (LONG)key_state;
	const CString str_params = M2U(writer.write(params)).c_str();
	m_pHtml->CallJScript(_T("OnDragFilesEnter"), str_params);

	return effect;
}

DWORD CDHtmlWnd::FilesDropTarget::OnDragOver(IDataObject* data_object,
								  DWORD key_state,
								  POINT cursor_position,
								  DWORD effect)
{
	if(m_pHtml == NULL)
		return DROPEFFECT_NONE;

	COleDataObject data;
	data.Attach(data_object, FALSE);
	if(!data.IsDataAvailable(CF_HDROP))
	{
		data.Detach();
		return DROPEFFECT_NONE;
	}

	Json::FastWriter writer;
	Json::Value params;
	params["point_x"] = cursor_position.x;
	params["point_y"] = cursor_position.y;
	params["key_state"] = (LONG)key_state;
	const CString str_params = M2U(writer.write(params)).c_str();
	m_pHtml->CallJScript(_T("OnDragFilesOver"), str_params);

	return effect;
}

void CDHtmlWnd::FilesDropTarget::OnDragLeave(IDataObject* data_object)
{
	if(m_pHtml == NULL)
		return;

	COleDataObject data;
	data.Attach(data_object, FALSE);
	if(data.IsDataAvailable(CF_HDROP))
	{
		m_pHtml->CallJScript(_T("OnDragFilesLeave"));
	}
	data.Detach();
}

DWORD CDHtmlWnd::FilesDropTarget::OnDrop(IDataObject* data_object,
							  DWORD key_state,
							  POINT cursor_position,
							  DWORD effect)
{
	if(m_pHtml == NULL)
		return DROPEFFECT_NONE;

	COleDataObject data;
	data.Attach(data_object, FALSE);
	if(!data.IsDataAvailable(CF_HDROP))
	{
		data.Detach();
		return DROPEFFECT_NONE;
	}

	HDROP hDropFiles = NULL;
	HGLOBAL hMemData = NULL;

	hMemData = data.GetGlobalData(CF_HDROP);
	hDropFiles = (HDROP)GlobalLock((HGLOBAL)hMemData);
	if(hDropFiles != NULL)
	{
		Json::FastWriter writer;
		Json::Value params;
		Json::Value files;

		OnDropFiles(hDropFiles, files);

		params["point_x"] = cursor_position.x;
		params["point_y"] = cursor_position.y;
		params["key_state"] = (LONG)key_state;
		params["files"] = files;
		const CString str_params = M2U(writer.write(params)).c_str();
		m_pHtml->CallJScript(_T("OnDragFilesDrop"), str_params);
	}
	GlobalUnlock(hMemData);
	data.Detach();

	return effect;
}

void CDHtmlWnd::FilesDropTarget::OnDropFiles(HDROP hDropInfo, Json::Value& files)
{
	UINT nFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	for(UINT i = 0; i < nFiles; i++)
	{
		char szFname[MAX_PATH];
		DragQueryFileA(hDropInfo, i, szFname, MAX_PATH);
		files.append(szFname);
	}
	DragFinish(hDropInfo);
}

STDMETHODIMP CDHtmlWnd::GetExternal(IDispatch **ppDispatch)
{
	if(ppDispatch == NULL)
		return E_POINTER;

	*ppDispatch = NULL;
	if (m_spExternalDisp.p && CanAccessExternal())
	{
		m_spExternalDisp.p->AddRef();
		*ppDispatch = m_spExternalDisp.p;
		return S_OK;
	}
	return E_NOTIMPL;
}
STDMETHODIMP CDHtmlWnd::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
	dwTranslate; // unused
	pchURLIn; // unused

	if(ppchURLOut == NULL)
		return E_POINTER;

	*ppchURLOut = NULL;
	return S_FALSE;
}
STDMETHODIMP CDHtmlWnd::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
{
	pDO; // unused;

	if(ppDORet == NULL)
		return E_POINTER;

	*ppDORet = NULL;
	return S_FALSE;
}

//// This function is overridden in derived class via DECLARE_DHTML_EVENT_MAP	
//const DHtmlEventMapEntry* CDHtmlWnd::GetDHtmlEventMap()
//{
//	return NULL;
//}
//
