#include "stdafx.h"
#include "utils.h"
#include "common/MemoryLeakChecker.h"
CLIPFORMAT const CF_HTML = RegisterClipboardFormat(_T("HTML Format")) ;
CLIPFORMAT const CF_FILENAME = RegisterClipboardFormat(CFSTR_FILENAME) ;
CLIPFORMAT const CF_FILENAMEMAP = RegisterClipboardFormat(CFSTR_FILENAMEMAP) ;

// Convert Himetric along the X axis to X pixels
LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch)
{
    return (LONG) MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
}

// Convert Himetric along the Y axis to Y pixels
LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch)
{
    return (LONG) MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
}

// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch)
{
    return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch)
{
    return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}
