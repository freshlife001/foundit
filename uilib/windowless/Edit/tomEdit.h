//////////////////////////////////////////////////////////////////////////
//
//	描述:   Windowless Rich Edit Implement
//
//	
//	日期:   2011/10/11
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////
#pragma once
#include <Imm.h>
#include <textserv.h>
#include <RichOle.h>
#include <set>

#include "../wndless.h"

#include "../ScrollBar/WLScrollBar.h"
#include "common/utils/Border.h"
#include "Caret.h"

#include "IWLTomEdit.h"

#pragma comment(lib, "RichEd20")

class ReObject ;
class ReImage ;
class tomEditCallback ;
/*
class ReObjectManager
{
    // ReObject通过调用该接口向tomEdit登记自己刚刚被绘制，
    // tomEdit通过此判断未登记的ReObject不在显示区域，
    // 再通知未登记的ReObject停止动画
    virtual void HaveJustBeenDrawn(ReObject * object) = 0 ;
};
*/
class tomEdit : public WLTomEditImpBase, public ITextHost
{
public:
    tomEdit() ;
    ~tomEdit() ;

public:
    bool SetBackgroundTransparent(bool transparent) ;

    COLORREF SetBackgroundColor(bool use_system_color, COLORREF color) ;
    COLORREF GetBackgroundColor() const ;
    COLORREF SetHighLightTextColor(bool use_system_color, COLORREF color) ;
    COLORREF GetHighLightTextColor() const ;
    COLORREF SetHighLightTextBackgroundColor(bool use_system_color, COLORREF color) ;
    COLORREF GetHighLightTextBackgroundColor() const ;

    IRichEditOle * GetIRichEditOle() ;
    bool SetOLECallback(IRichEditOleCallback* pCallback) ;

    void EmptyUndoBuffer() ;
    WORD GetSelectionType() const ;

    HRESULT GetObject(LONG iob, REOBJECT * reobject, DWORD flags) ;
    BOOL InsertImageObject(LONG char_position, std::wstring const image_file_path) ;

    void ReplaceSel(wchar_t const * new_text, BOOL can_undo = FALSE) ;

    void RemoveObject(IOleObject * object) ;

//     void RevokeDragDrop() ;
//     void RegisterDragDrop() ;

    // 测试接口
    BOOL TestInsertObject() ;
    BOOL TestChangeObjectSize(int cx, int cy) ;
    BOOL TestChangeObjectSizeSelf(int cx, int cy) ;
    void TestGetObjectBound() ;     // succeed half
    void TestHideObject() ;         // failed

protected:
    //////////////////////////////////////////////////////////////////////////
    // IWndLess Interface
    virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;
    virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;

    virtual void OnDestroy() ;

    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;

    virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;

protected:
    bool Init() ;
    void InitDefaultCharFormat() ;
    void InitDefaultParaFormat() ;

    void GetNoborderBound(RECT &noborder_bound) ;
    void GetClientBound(RECT &client_bound) ;
    void GetTextBound(RECT &text_bound) ;

    void OnVScroll(LPSCROLLMSGINFO, WLScrollBar*) ;
    void OnHScroll(LPSCROLLMSGINFO, WLScrollBar*) ;

protected:
    //////////////////////////////////////////////////////////////////////////
    // IUnknown Interface

    virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG _stdcall AddRef(void);
    virtual ULONG _stdcall Release(void);

protected:
    //////////////////////////////////////////////////////////////////////////
    // ITextHost Interface
    //@cmember Get the DC for the host
    virtual HDC TxGetDC() ;

    //@cmember Release the DC gotten from the host
    virtual INT TxReleaseDC(HDC hdc) ;

    //@cmember Show the scroll bar
    virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow) ;

    //@cmember Enable the scroll bar
    virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags) ;

    //@cmember Set the scroll range
    virtual BOOL TxSetScrollRange(
        INT fnBar,
        LONG nMinPos,
        INT nMaxPos,
        BOOL fRedraw) ;

    //@cmember Set the scroll position
    virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw) ;

    //@cmember InvalidateRect
    virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode) ;

    //@cmember Send a WM_PAINT to the window
    virtual void TxViewChange(BOOL fUpdate) ;

    //@cmember Create the caret
    virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) ;

    //@cmember Show the caret
    virtual BOOL TxShowCaret(BOOL fShow) ;

    //@cmember Set the caret position
    virtual BOOL TxSetCaretPos(INT x, INT y) ;

    //@cmember Create a timer with the specified timeout
    virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout) ;

    //@cmember Destroy a timer
    virtual void TxKillTimer(UINT idTimer) ;

    //@cmember Scroll the content of the specified window's client area
    virtual void TxScrollWindowEx (
        INT dx,
        INT dy,
        LPCRECT lprcScroll,
        LPCRECT lprcClip,
        HRGN hrgnUpdate,
        LPRECT lprcUpdate,
        UINT fuScroll) ;

    //@cmember Get mouse capture
    virtual void TxSetCapture(BOOL fCapture) ;

    //@cmember Set the focus to the text window
    virtual void TxSetFocus() ;

    //@cmember Establish a new cursor shape
    virtual void TxSetCursor(HCURSOR hcur, BOOL fText) ;

    //@cmember Converts screen coordinates of a specified point to the client coordinates
    virtual BOOL TxScreenToClient (LPPOINT lppt) ;

    //@cmember Converts the client coordinates of a specified point to screen coordinates
    virtual BOOL TxClientToScreen (LPPOINT lppt) ;

    //@cmember Request host to activate text services
    virtual HRESULT TxActivate( LONG * plOldState ) ;

    //@cmember Request host to deactivate text services
    virtual HRESULT TxDeactivate( LONG lNewState ) ;

    //@cmember Retrieves the coordinates of a window's client area
    virtual HRESULT TxGetClientRect(LPRECT prc) ;

    //@cmember Get the view rectangle relative to the inset
    virtual HRESULT TxGetViewInset(LPRECT prc) ;

    //@cmember Get the default character format for the text
    virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF ) ;

    //@cmember Get the default paragraph format for the text
    virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF) ;

    //@cmember Get the background color for the window
    virtual COLORREF TxGetSysColor(int nIndex) ;

    //@cmember Get the background (either opaque or transparent)
    virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle) ;

    //@cmember Get the maximum length for the text
    virtual HRESULT		TxGetMaxLength(DWORD *plength) ;

    //@cmember Get the bits representing requested scroll bars for the window
    virtual HRESULT		TxGetScrollBars(DWORD *pdwScrollBar) ;

    //@cmember Get the character to display for password input
    virtual HRESULT		TxGetPasswordChar(TCHAR *pch) ;

    //@cmember Get the accelerator character
    virtual HRESULT		TxGetAcceleratorPos(LONG *pcp) ;

    //@cmember Get the native size
    virtual HRESULT		TxGetExtent(LPSIZEL lpExtent) ;

    //@cmember Notify host that default character format has changed
    virtual HRESULT 	OnTxCharFormatChange (const CHARFORMATW * pcf) ;

    //@cmember Notify host that default paragraph format has changed
    virtual HRESULT		OnTxParaFormatChange (const PARAFORMAT * ppf) ;

    //@cmember Bulk access to bit properties
    virtual HRESULT		TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) ;

    //@cmember Notify host of events
    virtual HRESULT		TxNotify(DWORD iNotify, void *pv) ;

    // Far East Methods for getting the Input Context
    //#ifdef WIN95_IME
    virtual HIMC		TxImmGetContext() ;
    virtual void		TxImmReleaseContext( HIMC himc ) ;
    //#endif

    //@cmember Returns HIMETRIC size of the control bar.
    virtual HRESULT		TxGetSelectionBarWidth (LONG *lSelBarWidth) ;

    // ITextHost2 Interface
    //@cmember Is a double click in the message queue?
    virtual BOOL		TxIsDoubleClickPending() ; 
    //@cmember Get the overall window for this control	 
    virtual HRESULT		TxGetWindow(HWND *phwnd) ;
    //@cmember Set control window to foreground
    virtual HRESULT		TxSetForegroundWindow() ;
    //@cmember Set control window to foreground
    virtual HPALETTE	TxGetPalette() ;

private:
    static LONG s_XPixelPerInch ;
    static LONG s_YPixelPerInch ;

    ULONG m_ref_count ;
    ITextServices * m_services ;

    CHARFORMAT2W m_default_char_format ;
    PARAFORMAT m_default_para_format ;

    WLScrollBar m_vert_scroll_bar ;
    WLScrollBar m_horz_scroll_bar ;

    int m_scroll_width ;

    IBorder * m_border ;

    RECT m_border_inset ;       // 以像素为单位
    RECT m_view_inset ;         // 以像素为单位

    COLORREF m_background_color ;
    COLORREF m_highlight_text_color ;
    COLORREF m_highlight_text_background_color ;

    Caret m_caret ;

    IRichEditOle * m_rich_edit_ole ;
    tomEditCallback * m_call_back ;

    ReImage * m_test_object ;

    // 所有插入到tomEdit中的对象
    std::set<IOleObject*> m_object_group ;

    bool m_show_vert_scroll_bar : 1 ;
    bool m_show_horz_scroll_bar : 1 ;
    bool m_background_transparent : 1 ;
    bool m_use_system_background_color : 1 ;
    bool m_use_system_highlight_text_color : 1 ;
    bool m_use_system_highlight_text_background_color : 1 ;
    bool m_word_wrap : 1 ;

};
