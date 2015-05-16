#pragma once

// HIMETRIC units per inch (used for conversion)
#define HIMETRIC_PER_INCH 2540

extern CLIPFORMAT const CF_HTML ;
extern CLIPFORMAT const CF_FILENAME ;
extern CLIPFORMAT const CF_FILENAMEMAP ;

// Convert Himetric along the X axis to X pixels
LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch) ;
// {
//     return (LONG) MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
// }

// Convert Himetric along the Y axis to Y pixels
LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch) ;
// {
//     return (LONG) MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
// }

// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch) ;
// {
//     return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
// }

// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch) ;
// {
//     return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
// }
