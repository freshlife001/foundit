#pragma once

// {BFF32FA4-26BD-4061-8C5E-B21C6F8AB789}
static IID const IID_IBorder = 
{ 0xbff32fa4, 0x26bd, 0x4061, { 0x8c, 0x5e, 0xb2, 0x1c, 0x6f, 0x8a, 0xb7, 0x89 } };

class IBorder : public IUnknown
{
public:
    virtual void SetPos(int x, int y) = 0 ;
    virtual void SetSize(int width, int height) = 0 ;
    //virtual void GetBound(RECT &bound) = 0 ;

    virtual void SetInset(int left, int top, int right, int bottom) = 0 ;
    //virtual void GetInset(RECT &bound) = 0 ;

    virtual void Draw(HDC hdc, RECT const &update) = 0 ;
};

// {F2DFC12A-1B00-4245-9220-34478BD2E2D3}
static IID const IID_IRectangleBorder = 
{ 0xf2dfc12a, 0x1b00, 0x4245, { 0x92, 0x20, 0x34, 0x47, 0x8b, 0xd2, 0xe2, 0xd3 } };

class RectangleBorder : public IBorder
{
public:
    RectangleBorder() ;

    //////////////////////////////////////////////////////////////////////////
    // IUnknown Interface
    virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG _stdcall AddRef(void);
    virtual ULONG _stdcall Release(void);

    //////////////////////////////////////////////////////////////////////////
    // IBorder Interface
    virtual void SetPos(int x, int y) ;
    virtual void SetSize(int width, int height) ;
    //virtual void GetBound(RECT &bound) ;

    virtual void SetInset(int left, int top, int right, int bottom) ;
    //virtual void GetInset(RECT &inset) ;

    virtual void Draw(HDC hdc, RECT const &update) ;

    //////////////////////////////////////////////////////////////////////////
    // RectangleBorder Interface
    COLORREF SetColor(COLORREF color) ;
    COLORREF GetColor() const ;

private:
    ULONG m_ref_count ;
    POINT m_position ;
    SIZE m_size ;
    RECT m_inset ;
    COLORREF m_color ;

};

HRESULT CreateBorder(IID const &iid, IUnknown ** ppUnknown) ;