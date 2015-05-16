//////////////////////////////////////////////////////////////////////////
//
//	描述:   根据给定内容组装Html Fragment，作为Html格式数据提供给剪切板；
//          解析剪切板中的Html格式数据，获得其中的文字和图片信息
//
//	
//	日期:   2011/11/26
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma  once
#include <string>
#include <list>

class RichTextClipboardData
{
public:
    RichTextClipboardData(bool pack_or_parse) ;
    ~RichTextClipboardData() ;

    enum Format
    {
        Fmt_Unknown = 0,
        Fmt_Text    = 1,
        Fmt_Html    = 2,
//         Fmt_Json    = 3,
    };

    enum Type
    {
        Type_Unknown    = 0,
        Type_Text       = 1,
        Type_Image      = 2,
    };

    struct Element 
    {
        Type type ;

        std::wstring data ;
    };

    //////////////////////////////////////////////////////////////////////////
    // pack text/image(s) to html fragment or jason

    bool Append(Type type, std::wstring textorpath) ;
    void Clear() ;
    HGLOBAL GenerateHtmlFragment() const ;
    HGLOBAL GenerateJason() const ;

    //////////////////////////////////////////////////////////////////////////
    // parse html fragment or jason to text/image(s)

    bool FromHtmlFragment(HGLOBAL memory) ;
    bool FromJason(HGLOBAL memory) ;
    bool GenerateElements(std::list<Type> &elements) const ;

private:
    bool m_pack_or_parse ;

    // 打包时保存元素用
    std::list<Element> m_elements ;

    static char s_html_data_description[] ;
    static char s_html_data_description_format[] ;
    static char s_html_data_head[] ;
    static char s_html_data_trail[] ;

};

namespace Clipboard
{
    enum Format
    {
        Unknown = 0,
        Text,
        Html,
    };

    enum Element_Type
    {
        Ele_Unknown    = 0,
        Ele_Text       = 1,
        Ele_Image      = 2,
    };

    class Packer
    {
    public:
        void Append(Element_Type type, std::wstring data) ;
        void Clear() ;
        Format Format() const { return Unknown ; }
        HGLOBAL GenerateClipboardData() const { return NULL ; }

    protected:
        std::wstring m_data ;
    };

    class TextPacker : public Packer
    {
    public:
        Clipboard::Format GetFormat() const { return Text ; }
        HGLOBAL GenerateClipboardData() const ; // unicode格式
    };

    class HtmlPacker : public Packer
    {
    public:
        void Append(Element_Type type, std::wstring data) ;
        Clipboard::Format GetFormat() const { return Html ; }
        HGLOBAL GenerateClipboardData() const ; // utf-8格式

    private:
        static char s_description[] ;
        static char s_description_format[] ;
        static char s_head[] ;
        static char s_trail[] ;
    };

    class HtmlParser 
    {
    public:
        struct Element 
        {
            Element_Type type ;
            std::wstring data ;
        };

        static BOOL ParseFragment(HGLOBAL text_stream, std::list<Element> &element_group) ;

    private:
        //std::list<Element> m_element_group ;
    };
} ;