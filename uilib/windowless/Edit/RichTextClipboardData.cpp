#include "stdafx.h"
#include "RichTextClipboardData.h"

#include "html/ParserDom.h"
#include "html/tree.h"
#include "html/hutils.h"
#include "common/MemoryLeakChecker.h"
char RichTextClipboardData::s_html_data_description[] = 
"Version:0.9\r\n"
"StartHTML:0000000000\r\n"
"EndHTML:0000000000\r\n"
"StartFragment:0000000000\r\n"
"EndFragment:0000000000\r\n"
;

char RichTextClipboardData::s_html_data_description_format[] =
"Version:0.9\r\n"
"StartHTML:%010d\r\n"
"EndHTML:%010d\r\n"
"StartFragment:%010d\r\n"
"EndFragment:%010d\r\n"
;

char RichTextClipboardData::s_html_data_head[] =
"<HTML><BODY>"
"<!--StartFragment-->"
;

char RichTextClipboardData::s_html_data_trail[] =
"<!--EndFragment-->"
"</BODY></HTML>"
;

RichTextClipboardData::RichTextClipboardData(bool pack_or_parse) 
: m_pack_or_parse(pack_or_parse)
{

}

RichTextClipboardData::~RichTextClipboardData() 
{

}

//////////////////////////////////////////////////////////////////////////
// pack text/image(s) to html fragment or jason

bool RichTextClipboardData::Append(Type type, std::wstring textorpath) 
{
    if (!m_pack_or_parse)
        return false ;

    Element e = {type, textorpath} ;
    m_elements.push_back(e) ;

    return true ;
}

void RichTextClipboardData::Clear() 
{
    if (!m_pack_or_parse)
        return ;

    m_elements.clear() ;
}

HGLOBAL RichTextClipboardData::GenerateHtmlFragment() const 
{
    if (!m_pack_or_parse)
        return NULL ;

    if (m_elements.empty())
        return NULL ;

    // 组合内容
    std::wstring content ;
    std::list<Element>::const_iterator iter ;
    for (iter = m_elements.begin() ; iter != m_elements.end() ; ++ iter)
    {
        if (iter->type == Type_Text)
        {
            // 遍历其中换行符，转换成<br>
            std::wstring::const_iterator pos ;
            for (pos = iter->data.begin() ; pos != iter->data.end() ; ++ pos)
            {
                wchar_t c = *pos ;
                if (c != L'\r' && c != L'\n')
                    content.append(&c) ;
                else
                {
                    content.append(L"<br>") ;

                    if (c == L'\r')     // 清除后面的\n
                    {
                        ++ pos ;
                        if (*pos != L'\n')
                            -- pos ;
                    }
                }
            }
        }
        else if (iter->type == Type_Image)
        {
            content.append(L"<img src=\"file://") ;     // 这里加 file:// 是为了让QQ能识别
            content.append(iter->data) ;
            content.append(L"\" />") ;
        }
    }

    if (content.empty())
        return NULL ;

    // 将内容转成utf-8格式
    int request_size = WideCharToMultiByte(
        CP_UTF8,
        0,
        content.c_str(),
        content.length(),
        NULL,
        0,
        NULL,
        NULL
        ) ;

    if (request_size == 0)
        return NULL ;

//     char * content_utf8 = new char[request_size + 1] ;
//     WideCharToMultiByte(CP_UTF8, 0, content.c_str(), content.length(), content_utf8, request_size, NULL, NULL) ;

    // 初始化Html Fragment头和尾
    int starthtml = sizeof(s_html_data_description) - 1 ;
    int startfragment = sizeof(s_html_data_description) - 1 + sizeof(s_html_data_head) - 1 ;
    int endfragment = sizeof(s_html_data_description) - 1 + sizeof(s_html_data_head) - 1 + request_size ;
    int endhtml = sizeof(s_html_data_description) - 1 + sizeof(s_html_data_head) - 1 + request_size + sizeof(s_html_data_trail) - 1 ;

    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, endhtml) ;
    if (memory == NULL)
        return NULL ;

    char * p = (char*)GlobalLock(memory) ;
    if (NULL == p)
    {
        GlobalUnlock(memory) ;
        GlobalFree(memory) ;
        return NULL ;
    }

    // 第二个参数总是要比总字符数多1，否则会引起崩溃
    sprintf_s(p, starthtml + 1, s_html_data_description_format, starthtml, endhtml, startfragment, endfragment) ;
    memcpy(p + starthtml, s_html_data_head, sizeof(s_html_data_head) - 1) ;
    p += startfragment ;

    WideCharToMultiByte(CP_UTF8, 0, content.c_str(), content.length(), p, request_size, NULL, NULL) ;

    memcpy(p + request_size, s_html_data_trail, sizeof(s_html_data_trail) - 1) ;

    GlobalUnlock(memory) ;

    return memory ;

//     sprintf(data_head, data_head_format, starthtml, endhtml, startfragment, endfragment) ;
}

// 接口尚未完成
HGLOBAL RichTextClipboardData::GenerateJason() const 
{
    if (!m_pack_or_parse)
        return NULL ;

    if (m_elements.empty())
        return NULL ;

    return NULL ;
}

//////////////////////////////////////////////////////////////////////////
// parse html fragment or jason to text/image(s)

bool RichTextClipboardData::FromHtmlFragment(HGLOBAL memory) 
{
    if (m_pack_or_parse)
        return false ;

    return false ;
}

bool RichTextClipboardData::FromJason(HGLOBAL memory) 
{
    if (m_pack_or_parse)
        return false ;

    return false ;
}

bool RichTextClipboardData::GenerateElements(std::list<Type> &elements) const 
{
    if (m_pack_or_parse)
        return false ;

    return false ;
}

namespace Clipboard
{
    //////////////////////////////////////////////////////////////////////////
    // Packer
    void Packer::Append(Element_Type type, std::wstring data) 
    {
        m_data.append(data) ;
    }

    void Packer::Clear() 
    {
        m_data.clear() ;
    }

    //////////////////////////////////////////////////////////////////////////
    // TextPacker
    HGLOBAL TextPacker::GenerateClipboardData() const 
    {
        if (m_data.empty())
            return NULL ;

        size_t data_size = (m_data.length() + 1) * sizeof(wchar_t) ;
        HGLOBAL data = ::GlobalAlloc(GMEM_MOVEABLE, data_size) ;
        if (data == NULL)
            return NULL ;
        BYTE * p = (BYTE*)::GlobalLock(data) ;
        memcpy(p, m_data.c_str(), data_size) ;

        ::GlobalUnlock(data) ;

        return data ;
    }

    //////////////////////////////////////////////////////////////////////////
    // HtmlPacker

    char HtmlPacker::s_description[] = 
        "Version:0.9\r\n"
        "StartHTML:0000000000\r\n"
        "EndHTML:0000000000\r\n"
        "StartFragment:0000000000\r\n"
        "EndFragment:0000000000\r\n"
        ;
    char HtmlPacker::s_description_format[] =
        "Version:0.9\r\n"
        "StartHTML:%010d\r\n"
        "EndHTML:%010d\r\n"
        "StartFragment:%010d\r\n"
        "EndFragment:%010d\r\n"
        ;

    char HtmlPacker::s_head[] =
        "<HTML><BODY>"
        "<!--StartFragment-->"
        ;

    char HtmlPacker::s_trail[] =
        "<!--EndFragment-->"
        "</BODY></HTML>"
        ;

    void HtmlPacker::Append(Element_Type type, std::wstring data) 
    {
        // 添加文本元素时要将文本中的转义字符转成实体名称
        if (type == Ele_Text)
        {
            std::wstring content ;
            std::wstring::const_iterator pos ;
            for (pos = data.begin() ; pos != data.end() ; ++ pos)
            {
                wchar_t c = *pos ;
                // 将\r\n转成<br>
                if (c == L'\r' || c == L'\n')
                {
                    content.append(L"<br>") ;

                    if (c == L'\r')     // 清除后面的\n
                    {
                        ++ pos ;
                        if (pos == data.end())
                            break ;

                        if (*pos != L'\n')
                            -- pos ;
                    }
                }
                else if (c == L'<')
                {
                    content.append(L"&lt;") ;
                }
                else if (c == L'>')
                {
                    content.append(L"&gt;") ;
                }
                else if (c == '\"')
                {
                    content.append(L"&quot;") ;
                }
                else if (c == '&')
                {
                    content.append(L"&amp;") ;
                }
                else
                {
                    content.append(1, c) ;
                }
            }
            m_data.append(content) ;
        }
        else if (type == Ele_Image)
        {
            m_data.append(data) ;
        }
    }

    HGLOBAL HtmlPacker::GenerateClipboardData() const
    {
        if (m_data.empty())
            return NULL ;

        // 将内容转成utf-8格式
        int content_utf8_size = WideCharToMultiByte(
            CP_UTF8,
            0,
            m_data.c_str(),
            m_data.length(),
            NULL,
            0,
            NULL,
            NULL
            ) ;

        if (content_utf8_size == 0)
            return NULL ;

        // 初始化Html Fragment头和尾
        int starthtml = sizeof(s_description) - 1 ;
        int startfragment = sizeof(s_description) - 1 + sizeof(s_head) - 1 ;
        int endfragment = sizeof(s_description) - 1 + sizeof(s_head) - 1 + content_utf8_size ;
        int endhtml = sizeof(s_description) - 1 + sizeof(s_head) - 1 + content_utf8_size + sizeof(s_trail) - 1 ;

        HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, endhtml) ;
        if (memory == NULL)
            return NULL ;

        char * p = (char*)GlobalLock(memory) ;
        if (NULL == p)
        {
            GlobalUnlock(memory) ;
            GlobalFree(memory) ;
            return NULL ;
        }

        // 第二个参数总是要比总字符数多1，否则会引起崩溃
        sprintf_s(p, starthtml + 1, s_description_format, starthtml, endhtml, startfragment, endfragment) ;
        memcpy(p + starthtml, s_head, sizeof(s_head) - 1) ;
        p += startfragment ;

        WideCharToMultiByte(CP_UTF8, 0, m_data.c_str(), m_data.length(), p, content_utf8_size, NULL, NULL) ;

        memcpy(p + content_utf8_size, s_trail, sizeof(s_trail) - 1) ;

        GlobalUnlock(memory) ;

        return memory ;
    }

    void ComputePrefix(char * p, int * pai, int length)
    {
        int status = 0 ;
        pai[0] = status ;

        int i ;
        for (i = 1 ; i < length ; ++ i)
        {
            if (p[i] == p[pai[i - 1]])
                pai[i] = pai[i - 1] + 1 ;
            else
                pai[i] = 0 ;
        }
    }

    char * KMPSearch(char * src, int src_length, char * pattern, int pattern_length)
    {
        int * pai = new int[pattern_length + 1] ;
        pai[0] = 0 ;
        ComputePrefix(pattern, pai + 1, pattern_length) ;

        int status = 0 ;
        int i ;
        for (i = 0 ; i < src_length ; )
        {
            if (src[i] == pattern[status])
            {
                ++ status ;
                ++ i ;
            }
            else
            {
                if (status == 0)
                    ++ i ;
                else
                    status = pai[status] ;
            }

            if (status == pattern_length)
            {
                delete [] pai ;
                return src + i - pattern_length ;
            }
        }

        delete [] pai ;

        return NULL ;
    }

    void Utf8ToUnicode(std::string const &utf8, std::wstring &unicode) 
    {
        if (utf8.empty())
            return ;

        int unicode_size = MultiByteToWideChar(
            CP_UTF8,
            0,
            utf8.c_str(),
            utf8.length(),
            NULL,
            0
            ) ;

        //unicode.resize(unicode_size) ;
        wchar_t * u = new wchar_t[unicode_size + 1] ;
        MultiByteToWideChar(
            CP_UTF8,
            0,
            utf8.c_str(),
            utf8.length(),
            u,
            unicode_size
            ) ;
        u[unicode_size] = 0 ;
        unicode.assign(u) ;
        delete [] u ;
    }

    //////////////////////////////////////////////////////////////////////////
    // 解析html fragment - static 
    BOOL HtmlParser::ParseFragment(HGLOBAL text_stream, std::list<Element> &element_group)
    {
        element_group.clear() ;

        char * html_fragment = (char*)::GlobalLock(text_stream) ;
        if (html_fragment == NULL)
        {
            ::GlobalUnlock(text_stream) ;
            return FALSE ;
        }

        //////////////////////////////////////////////////////////////////////////
        // 找到主体内容
        long start_fragment, end_fragment ;
        // 寻找StartFragment
        char * start_fragment_tag = "StartFragment:" ;
        char * end_fragment_tag = "EndFragment:" ;

        char * begin = KMPSearch(html_fragment, strlen(html_fragment), start_fragment_tag, strlen(start_fragment_tag)) ;
        begin += strlen(start_fragment_tag) ;
        start_fragment = atoi(begin) ;

        // 寻找EndFragment
        begin = KMPSearch(begin, strlen(begin), end_fragment_tag, strlen(end_fragment_tag)) ;
        begin += strlen(end_fragment_tag) ;
        end_fragment = atoi(begin) ;

        //////////////////////////////////////////////////////////////////////////
        // 解析html
        std::string content(html_fragment + start_fragment, end_fragment - start_fragment) ;
        ::GlobalUnlock(text_stream) ;

        htmlcxx::HTML::ParserDom parser ;
        tree<htmlcxx::HTML::Node> dom = parser.parseTree(content) ;

        std::string temp_text ;

        tree<htmlcxx::HTML::Node>::iterator it = dom.begin();
        tree<htmlcxx::HTML::Node>::iterator end = dom.end();
        for ( ; it != end ; ++ it)
        {
            if (it->isComment())
                continue ;

            if (it->isTag())
            {
                if (0 == _stricmp(it->tagName().c_str(), "br"))
                {
                    temp_text += "\r\n" ;
                }
                else if (0 == _stricmp(it->tagName().c_str(), "img"))
                {
                    // 打包之前的文本
                    if (!temp_text.empty())
                    {
                        std::string decode_text = htmlcxx::HTML::decode_entities(temp_text) ;
                        std::wstring text ;
                        Utf8ToUnicode(decode_text, text) ;
                        temp_text.clear() ;

                        Element e = {Ele_Text, text} ;
                        element_group.push_back(e) ;
                    }

                    it->parseAttributes() ;
                    std::pair<bool, std::string> attr = it->attribute("src") ;
                    if (attr.first)
                    {
                        std::string utf8_path = attr.second ;
                        // 除去路径前面的file:///等字符
                        // 算法是找到最后一个':'
                        std::string::size_type index = utf8_path.rfind(':') ;
                        if (index > 1)
                            utf8_path.erase(0, index - 1) ;

                        std::wstring path ;
                        Utf8ToUnicode(utf8_path, path) ;
                        Element e = {Ele_Image, path} ;
                        element_group.push_back(e) ;
                    }
                }
            }
            else
            {
                // 清除文本中的\r\n
                std::string t = it->text() ;
                std::string::iterator iter ;
                for (iter = t.begin() ; iter != t.end() ; )
                {
                    char c = *iter ;
                    if (c == '\r' || c == '\n')
                    {
                        iter = t.erase(iter) ;
                    }
                    else
                    {
                        ++ iter ;
                    }
                }
                temp_text += t ;
            }
        }

        if (!temp_text.empty())
        {
            std::string decode_text = htmlcxx::HTML::decode_entities(temp_text) ;
            std::wstring text ;
            Utf8ToUnicode(decode_text, text) ;
            temp_text.clear() ;

            Element e = {Ele_Text, text} ;
            element_group.push_back(e) ;
        }

        return TRUE ;
    }

} ;
