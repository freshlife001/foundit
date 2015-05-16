//////////////////////////////////////////////////////////////////////////
//
//	描述:   IRichEditOleCallback Implement
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

#include <RichOle.h>
#include <string>
#include "RichTextClipboardData.h"

class tomEdit ;

class tomEditCallback : public IRichEditOleCallback, public IDataObject
{
public:
    tomEditCallback(tomEdit * edit) ;
    ~tomEditCallback() ;

    //////////////////////////////////////////////////////////////////////////
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * lplpObj) ;
    STDMETHOD_(ULONG,AddRef) (THIS)  ;
    STDMETHOD_(ULONG,Release) (THIS) ;

    //////////////////////////////////////////////////////////////////////////
    // *** IRichEditOleCallback methods ***
    STDMETHOD(GetNewStorage) (THIS_ LPSTORAGE FAR * lplpstg) ;
    STDMETHOD(GetInPlaceContext) (THIS_ LPOLEINPLACEFRAME FAR * lplpFrame,
        LPOLEINPLACEUIWINDOW FAR * lplpDoc,
        LPOLEINPLACEFRAMEINFO lpFrameInfo) ;
    STDMETHOD(ShowContainerUI) (THIS_ BOOL fShow) ;
    STDMETHOD(QueryInsertObject) (THIS_ LPCLSID lpclsid, LPSTORAGE lpstg,
        LONG cp) ;
    STDMETHOD(DeleteObject) (THIS_ LPOLEOBJECT lpoleobj) ;
    STDMETHOD(QueryAcceptData) (THIS_ LPDATAOBJECT lpdataobj,
        CLIPFORMAT FAR * lpcfFormat, DWORD reco,
        BOOL fReally, HGLOBAL hMetaPict) ;
    STDMETHOD(ContextSensitiveHelp) (THIS_ BOOL fEnterMode) ;
    STDMETHOD(GetClipboardData) (THIS_ CHARRANGE FAR * lpchrg, DWORD reco,
        LPDATAOBJECT FAR * lplpdataobj) ;
    STDMETHOD(GetDragDropEffect) (THIS_ BOOL fDrag, DWORD grfKeyState,
        LPDWORD pdwEffect) ;
    STDMETHOD(GetContextMenu) (THIS_ WORD seltype, LPOLEOBJECT lpoleobj,
        CHARRANGE FAR * lpchrg,
        HMENU FAR * lphmenu) ;

    //////////////////////////////////////////////////////////////////////////
    //IDataObject methods
    // 提供这个接口是为复制图文混合文本用
    //similar to GetClipboardData
    virtual HRESULT __stdcall GetData( FORMATETC *pFormatetcIn, STGMEDIUM *pMedium );
    virtual HRESULT __stdcall GetDataHere( FORMATETC *pFormatetc, STGMEDIUM *pMedium );
    //similar to IsClipboardFormatAvailable
    virtual HRESULT __stdcall QueryGetData( FORMATETC *pFormatetc );
    //The word "canonical" means "the simplest form of something," 
    //In general, the canonical FORMATETC should contain the most general information possible for a specific rendering.
    //The simplest implementation of GetCanonicalFormatEtc copies the input structure to the output structure, 
    //sets the output ptd field to NULL, and returns DATA_S_SAMEFORMATETC. 
    //This says that the data object doesn't care about target devices for the specific FORMATETC. 
    //If, on the other hand, the source does care, it returns NOERROR, filling the output FORMATETC as generally as it can.
    virtual HRESULT __stdcall GetCanonicalFormatEtc( FORMATETC *pFormatetcIn, FORMATETC *pFormatetcOut );
    //similar to SetClipboardData
    virtual HRESULT __stdcall SetData( FORMATETC *pFormatetc, STGMEDIUM *pMedium, BOOL fRelease );
    //similar to EnumClipboardFormats
    virtual HRESULT __stdcall EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc );
    virtual HRESULT __stdcall DAdvise( FORMATETC *pFormatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection );
    virtual HRESULT __stdcall DUnadvise( DWORD dwConnection );
    virtual HRESULT __stdcall EnumDAdvise( IEnumSTATDATA **ppEnumAdvise );

protected:
    //////////////////////////////////////////////////////////////////////////
    // Internal Methods

    bool PrepareClipboardData() ;

private:
    ULONG m_ref_count ;

    tomEdit * m_edit ;

    //std::wstring m_text_clipboard_data ;    // 纯文本剪切板数据
    //RichTextClipboardData m_mix_clipboard_data ;  // 图文混合剪切板数据

    Clipboard::TextPacker m_text_packer ;   // 纯文本剪切板数据
    Clipboard::HtmlPacker m_html_packer ;   // 图文混合剪切板数据

};
