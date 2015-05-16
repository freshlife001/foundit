#include "StdAfx.h"
#include "Ch2PyConvertor.h"
#include <fstream>
#include "Process.h"
#include "utils/strutils.h"
#include "Resource.h"

#include "common/MemoryLeakChecker.h"

Ch2PyConvertor* Ch2PyConvertor::m_instance = NULL;

#include <streambuf>
template <typename char_type>
struct istreambuf : public std::basic_streambuf<char_type, std::char_traits<char_type> >
{
	istreambuf(char_type* buffer, std::streamsize bufferLength)
	{
		// set the "put" pointer the start of the buffer and record it's length.
		setg(buffer, buffer, buffer + bufferLength);
	}
};

// /***********************************************************************
// *      将窄字节转换为宽字节        
// ***********************************************************************/
// std::wstring toWideString( const char* pStr , int len )
// {
//  std::wstring buf ;
//  len = strlen(pStr);
//  wchar_t* buff;
//  buff = new wchar_t[len+1];
//  setlocale(LC_ALL,".936");
//  int nChars = mbstowcs(buff,pStr,len+1);
//  setlocale(LC_ALL,"C");
//  buf = buff;
//  delete [] buff; //by gyl
//  return buf;
// 
// }
// /**********************************************************************
// *      将宽字节转换为窄字节        
// ***********************************************************************/
// std::string toNarrowString( const wchar_t* pStr , int len )
// {
//  char* buff = new char[len*2+1];
//  memset(buff,0,len*2+1);
//  setlocale(LC_ALL,".936");
//  int nChars = wcstombs(buff,pStr,len*2+1);
//  setlocale(LC_ALL,"C");
//  std::string buf (buff);
//  delete [] buff;	//by gyl
//  return buf ;
//} 

Ch2PyConvertor::Ch2PyConvertor(void)
:m_bLoadFinished( false )
{
	m_hLoadThread = NULL;
}

Ch2PyConvertor::~Ch2PyConvertor(void)
{
}

Ch2PyConvertor* Ch2PyConvertor::GetInstance()
{
	if( m_instance == NULL )
	{
		m_instance = new Ch2PyConvertor();
	}
	return m_instance;
}
void Ch2PyConvertor::DestroyInstance()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
};

void Ch2PyConvertor::Load()
{
	if( m_hLoadThread == NULL )
	{
		unsigned int nDummy;
		m_hLoadThread = (HANDLE) ::_beginthreadex( NULL,0,_ThreadLoad,this,
			CREATE_SUSPENDED,&nDummy );
		if( m_hLoadThread)
			ResumeThread(m_hLoadThread);
	}
}

UINT WINAPI Ch2PyConvertor::_ThreadLoad(LPVOID pParam)
{
	Ch2PyConvertor* pConver = reinterpret_cast<Ch2PyConvertor *>(pParam);

	pConver->ThreadLoad();

	CloseHandle( pConver->m_hLoadThread );
	pConver->m_bLoadFinished = true;

	return 0;

}

void Ch2PyConvertor::ThreadLoad()
{
	if( m_pyTable.empty() )
	{
		HRSRC hRs = FindResource( NULL, MAKEINTRESOURCE( IDR_PINYIN ), TEXT("FILE"));
		if ( hRs == NULL )
		{
			return;
		}
		DWORD dwSize = SizeofResource(NULL, hRs ); 
		if (0 == dwSize)
		{
			return;
		}
		HGLOBAL hGlobal = LoadResource( NULL, hRs );
		if ( hGlobal == NULL )
		{
			return;
		}
		LPVOID pBuffer = LockResource(hGlobal); 
		if ( pBuffer == NULL )
		{
			return;
		}
		istreambuf<char> is( (char*)pBuffer, dwSize );
		std::istream fpy_table( &is );
		if( fpy_table.good())
		{
			std::string str;
			while( std::getline( fpy_table, str ))
			{
				if(! str.empty() )
				{
					int len = str.length();	
					std::wstring wstr = M2U( str );
					// used for trim
					while( !wstr.empty() && iswspace( wstr[0] ))
						wstr.erase( 0 );
					while( !wstr.empty() && iswspace( wstr.at( wstr.length() -1 )))
						wstr.erase( wstr.length() -1 );
					len = wstr.length();
					if( len > 1 )
					{
						TCHAR key = wstr.at( 0 );
						wstr = wstr.substr( 1, len );
						ASSERT( !wstr.empty() );
						// used for trim
						while( !wstr.empty() && iswspace( wstr[0] ))
							wstr.erase( 0 );
						m_pyTable[key] = wstr;
					}
				}
			}
		}
	}
}

bool Ch2PyConvertor::IsChChacracter( TCHAR ch )	//modified by gyl
{
	return ch>=0x4E00 && ch<=0x9FBF; 
}

void Ch2PyConvertor::Convert( const std::wstring& string, std::wstring& converResult )
{
	if( !m_bLoadFinished )
	{
		DWORD res = ::WaitForSingleObject( m_hLoadThread, INFINITE );
	}
	converResult.clear();
	for( unsigned int i = 0; i < string.size(); i++ )
	{
		TCHAR ch = string[i];
		if( IsChChacracter( ch ))
		{
			std::map<TCHAR,std::wstring>::iterator it = m_pyTable.find( ch );
			if( it != m_pyTable.end())
				converResult += it->second;
			else
				converResult += ch;
		}
		else
		{
			converResult += ch;
		}
	}

}

void Ch2PyConvertor::Convert( const std::wstring& string, std::wstring& convertResult, std::wstring& abbreviation )
{
	if( !m_bLoadFinished )
	{
		DWORD res = ::WaitForSingleObject( m_hLoadThread, INFINITE );
	}
	convertResult.clear();
	abbreviation.clear();
	for(unsigned int i = 0; i < string.size(); i++ )
	{
		TCHAR ch = string[i];
		if( IsChChacracter( ch ))
		{
			std::map<TCHAR,std::wstring>::iterator it = m_pyTable.find( ch );
			if( it != m_pyTable.end())
			{
				convertResult += it->second;
				if ( !it->second.empty())
				{
					abbreviation += it->second[0];
				}
			}
			else
				convertResult += ch;
		}
		else
		{
			convertResult += ch;
		}
	}
}

void Ch2PyConvertor::Convert(TCHAR ch,std::wstring& convertResult)
{
	if( IsChChacracter( ch ))
	{
		std::map<TCHAR,std::wstring>::iterator it = m_pyTable.find( ch );
		if( it != m_pyTable.end())
		{
			convertResult = it->second;
			
		}
		else
			convertResult = ch;
	}
	else
	{
		convertResult = ch;
	}
}