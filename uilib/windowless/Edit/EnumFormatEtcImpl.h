#pragma once

#include <Ole2.h>

class EnumFormatEtcImpl : public IEnumFORMATETC
{
private:
    ULONG           m_cRef;         //Object reference count
    ULONG           m_iCur;         //Current element.
    ULONG           m_cfe;          //Number of FORMATETCs in us
    LPFORMATETC     m_prgfe;        //Source of FORMATETCs

public:
    EnumFormatEtcImpl(ULONG, LPFORMATETC);
    ~EnumFormatEtcImpl(void);

    //IUnknown members
    STDMETHODIMP         QueryInterface(REFIID, void **);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    //IEnumFORMATETC members
    STDMETHODIMP Next(ULONG, LPFORMATETC, ULONG *);
    STDMETHODIMP Skip(ULONG);
    STDMETHODIMP Reset(void);
    STDMETHODIMP Clone(IEnumFORMATETC **);
};
