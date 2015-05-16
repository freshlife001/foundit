#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include "ImageResource.h"
#include "uilib/uilib_exports.h"

class CBitmap;
class Shared_ImageExImp;
class ImageEx;
class UILIB_EXPORT Shared_ImageEx
{
public:
	explicit Shared_ImageEx( ImageEx* pImg = NULL );
	Shared_ImageEx( ImageEx* pImg, boost::function<void()> fn );
	Shared_ImageEx( const Shared_ImageEx &rhs);
	Shared_ImageEx & operator=( const Shared_ImageEx &rhs);

	void SetCallBackFunction( boost::function<void()> fn );
	~Shared_ImageEx();

	// shared image data, load image from dll
	static Shared_ImageEx FromId( UINT nSourceId );
	// shared image data
	static Shared_ImageEx FromFile( LPCTSTR lpFileName );
	static Shared_ImageEx FromHICON( HICON hIcon );

	ImageEx * operator->() const { return this->operator ImageEx*();}
	bool IsNull();
	void Reset();
	operator ImageEx *() const ;


	void FillBitmap(CBitmap &bitmap);

private:
	Shared_ImageExImp* m_pImp;
};
