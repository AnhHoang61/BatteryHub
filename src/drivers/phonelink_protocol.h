#pragma once
#include <cwchar>
#include <cwctype>
#include <map>
#include <string>
#include <vector>
#include <utility>
namespace PhoneLink {
struct Reading { std::wstring name; int percent=-1; bool charging=false; };
namespace Detail {
struct Value {
    wchar_t kind=0;
    std::wstring text;
    std::map<std::wstring, Value> object;
    std::vector<Value> array;
};
// The card is an undocumented format. Bound nesting and size, reject malformed JSON,
// and inspect fields on their owning object rather than searching notification text.
class Parser {
    const std::wstring& input;
    size_t at=0, nodes=0;
    void Space(){while(at<input.size() && (input[at]==L' '||input[at]==L'\r'||input[at]==L'\n'||input[at]==L'\t'))++at;}
    bool Take(wchar_t c){Space();if(at==input.size()||input[at]!=c)return false;++at;return true;}
    bool String(std::wstring& out){
        if(!Take(L'"'))return false;
        while(at<input.size()){
            wchar_t c=input[at++];if(c==L'"')return true;if(c<32)return false;
            if(c!=L'\\'){out+=c;continue;}
            if(at==input.size())return false;
            c=input[at++];
            if(c==L'u'){
                unsigned v=0;
                for(int i=0;i<4;++i){if(at==input.size())return false;wchar_t h=input[at++];unsigned n;
                    if(h>=L'0'&&h<=L'9')n=h-L'0';else if(h>=L'a'&&h<=L'f')n=h-L'a'+10;else if(h>=L'A'&&h<=L'F')n=h-L'A'+10;else return false;v=v*16+n;}
                out+=static_cast<wchar_t>(v);
            }else if(c==L'"'||c==L'\\'||c==L'/')out+=c;
            else if(c==L'n')out+=L'\n';else if(c==L't')out+=L'\t';else if(c==L'r')out+=L'\r';else if(c==L'b')out+=L'\b';else if(c==L'f')out+=L'\f';else return false;
        }return false;
    }
    bool Read(Value& v,int depth){
        Space();if(depth>64||++nodes>20000||at==input.size())return false;
        wchar_t c=input[at];v.kind=c;
        if(c==L'"')return String(v.text);
        if(c==L'{'){
            ++at;if(Take(L'}'))return true;
            do{std::wstring key;Value child;if(!String(key)||!Take(L':')||!Read(child,depth+1)||v.object.count(key))return false;v.object.emplace(std::move(key),std::move(child));if(Take(L'}'))return true;}while(Take(L','));return false;
        }
        if(c==L'['){++at;if(Take(L']'))return true;do{Value child;if(!Read(child,depth+1))return false;v.array.push_back(std::move(child));if(Take(L']'))return true;}while(Take(L','));return false;}
        for(const wchar_t* literal:{L"true",L"false",L"null"}){size_t n=wcslen(literal);if(input.compare(at,n,literal)==0){at+=n;return true;}}
        if(c==L'-')++at;
        if(at==input.size())return false;
        if(input[at]==L'0')++at;else {if(input[at]<L'1'||input[at]>L'9')return false;while(at<input.size()&&input[at]>=L'0'&&input[at]<=L'9')++at;}
        if(at<input.size()&&input[at]==L'.'){++at;size_t start=at;while(at<input.size()&&input[at]>=L'0'&&input[at]<=L'9')++at;if(at==start)return false;}
        if(at<input.size()&&(input[at]==L'e'||input[at]==L'E')){++at;if(at<input.size()&&(input[at]==L'+'||input[at]==L'-'))++at;size_t start=at;while(at<input.size()&&input[at]>=L'0'&&input[at]<=L'9')++at;if(at==start)return false;}
        return true;
    }
public:
    explicit Parser(const std::wstring& s):input(s){}
    bool Parse(Value& v){if(input.size()>4*1024*1024||!Read(v,0))return false;Space();return at==input.size();}
};
inline const std::wstring* Field(const Value& v,const wchar_t* key){auto i=v.object.find(key);return i!=v.object.end()&&i->second.kind==L'"'?&i->second.text:nullptr;}
} // namespace Detail

namespace Detail {

enum class CardLang { English, Japanese, Chinese };

struct SpeakSuffix {
    const wchar_t* text;
    CardLang lang;
};

inline bool ParseSpeak(const std::wstring& speak, std::wstring& name, CardLang& lang) {
    // 为什么这么做：Windows 简体中文版“手机连接”(Phone Link) 在 StartMenuCompanion.json 的 speak 字段中输出中文连接后缀，统一映射后可按 1% 精度读取手机电量
    static constexpr SpeakSuffix kSuffixes[] = {
        { L" - Connected to your PC", CardLang::English },
        { L" - PC に接続しました",   CardLang::Japanese },
        { L" - 已连接到你的电脑",     CardLang::Chinese },
        { L" - 已连接到电脑",         CardLang::Chinese },
        { L" - 已连接至你的电脑",     CardLang::Chinese },
        { L" - 已连接至电脑",         CardLang::Chinese },
    };
    for (const auto& s : kSuffixes) {
        const std::wstring ending(s.text);
        if (speak.size() > ending.size() &&
            speak.compare(speak.size() - ending.size(), ending.size(), ending) == 0) {
            name = speak.substr(0, speak.size() - ending.size());
            lang = s.lang;
            return !name.empty();
        }
    }
    return false;
}

inline int ParseTooltip(const std::wstring& tooltip, CardLang& lang, bool& charging) {
    struct PrefixRule {
        const wchar_t* prefix;
        CardLang lang;
    };
    // 为什么这么做：用表驱动方式统一解析英/日/简中卡片的电量提示文本，避免重复判断分支；匹配前缀按从长到短排列
    static constexpr PrefixRule kPrefixes[] = {
        { L"Battery ",              CardLang::English },
        { L"バッテリー 残量 ",      CardLang::Japanese },
        { L"电池电量剩余：",        CardLang::Chinese },
        { L"电池电量剩余:",         CardLang::Chinese },
        { L"电池电量剩余",          CardLang::Chinese },
        { L"电池剩余电量：",        CardLang::Chinese },
        { L"电池剩余电量:",         CardLang::Chinese },
        { L"电池剩余电量",          CardLang::Chinese },
        { L"电池电量：",            CardLang::Chinese },
        { L"电池电量:",             CardLang::Chinese },
        { L"电池电量",              CardLang::Chinese },
        { L"剩余电量：",            CardLang::Chinese },
        { L"剩余电量:",             CardLang::Chinese },
        { L"剩余电量",              CardLang::Chinese },
        { L"电池：",                CardLang::Chinese },
        { L"电池:",                 CardLang::Chinese },
        { L"电池 ",                 CardLang::Chinese },
        { L"电量：",                CardLang::Chinese },
        { L"电量:",                 CardLang::Chinese },
        { L"电量 ",                 CardLang::Chinese },
    };

    size_t start = 0;
    bool matched = false;
    for (const auto& rule : kPrefixes) {
        const size_t len = std::char_traits<wchar_t>::length(rule.prefix);
        if (tooltip.size() >= len && tooltip.compare(0, len, rule.prefix) == 0) {
            lang = rule.lang;
            start = len;
            matched = true;
            break;
        }
    }
    if (!matched) return -1;

    if (lang == CardLang::Chinese) {
        while (start < tooltip.size() && tooltip[start] == L' ') ++start;
    }

    size_t end = start;
    while (end < tooltip.size() &&
           tooltip[end] >= L'0' && tooltip[end] <= L'9' &&
           end - start < 3) {
        ++end;
    }

    if (end == start || end >= tooltip.size() || tooltip[end] != L'%') return -1;

    // 为什么这么做：直接累加数字字符替代 std::stoi，避免抛出 std::invalid_argument 异常以支持全局 -fno-exceptions 瘦身编译
    int level = 0;
    for (size_t i = start; i < end; ++i) level = level * 10 + (tooltip[i] - L'0');
    if (level > 100) return -1;

    const std::wstring remainder = tooltip.substr(end + 1);
    if (lang == CardLang::English) {
        if (remainder != L" remaining" && remainder != L" remaining, charging") return -1;
        charging = (remainder == L" remaining, charging");
    } else if (lang == CardLang::Japanese) {
        if (!remainder.empty()) return -1;
        charging = false;  // Charging state not established for Japanese cards.
    } else {
        size_t pos = 0;
        while (pos < remainder.size() &&
               (remainder[pos] == L' ' || remainder[pos] == L',' || remainder[pos] == L'，' ||
                remainder[pos] == L'(' || remainder[pos] == L'（')) {
            ++pos;
        }
        std::wstring tail = remainder.substr(pos);
        while (!tail.empty() && (tail.back() == L' ' || tail.back() == L')' || tail.back() == L'）')) {
            tail.pop_back();
        }
        if (tail.empty() || tail == L"剩余") {
            charging = false;
        } else if (tail == L"正在充电" || tail == L"充电中" || tail == L"已接通电源" ||
                   tail == L"剩余，正在充电" || tail == L"剩余, 正在充电" || tail == L"剩余，充电中") {
            charging = true;
        } else {
            return -1;
        }
    }
    return level;
}

} // namespace Detail

inline bool ConnectedPhoneName(const std::wstring& speak,
                               std::wstring& name) {
    Detail::CardLang lang = Detail::CardLang::English;
    return Detail::ParseSpeak(speak, name, lang);
}

inline int BatteryFromTooltip(const std::wstring& tooltip,
                              bool& charging) {
    Detail::CardLang lang = Detail::CardLang::English;
    return Detail::ParseTooltip(tooltip, lang, charging);
}



namespace Detail {
inline void Battery(const Value& value, CardLang expectedLang, int& percent, bool& charging, bool& ambiguous){
    if(value.kind==L'['){for(const auto& child:value.array)Battery(child,expectedLang,percent,charging,ambiguous);return;}
    if(value.kind!=L'{')return;
    const auto* type=Field(value,L"type");
    if(type && *type!=L"AdaptiveCard" && *type!=L"Container" && *type!=L"ColumnSet" && *type!=L"Column" && *type!=L"TextBlock" && *type!=L"Image")return;
    if(const auto* tooltip=Field(value,L"tooltip")){
        CardLang tipLang = CardLang::English;
        bool charge=false;
        int parsedLevel = ParseTooltip(*tooltip, tipLang, charge);
        int n = (parsedLevel >= 0 && tipLang == expectedLang) ? parsedLevel : -1;
        const auto* text=Field(value,L"text");
        if(n>=0 && (!text || *text==std::to_wstring(n)+L"%")){
            if(percent>=0 && (percent!=n||charging!=charge))ambiguous=true;
            percent=n;charging=charge;
        }
    }
    // Only traverse card layout fields, never actions, arbitrary metadata or notification payloads.
    for(const wchar_t* field:{L"body",L"items",L"columns"}){auto i=value.object.find(field);if(i!=value.object.end())Battery(i->second,expectedLang,percent,charging,ambiguous);}
}
} // namespace Detail
inline bool Parse(const std::wstring& json,Reading& out){
    Detail::Value root;
    if(!Detail::Parser(json).Parse(root)||root.kind!=L'{')return false;
    const auto* speak=Detail::Field(root,L"speak");Reading parsed;
    Detail::CardLang cardLang = Detail::CardLang::English;
    if(!speak||!Detail::ParseSpeak(*speak,parsed.name,cardLang))return false;
    bool ambiguous=false;
    Detail::Battery(root,cardLang,parsed.percent,parsed.charging,ambiguous);
    if(ambiguous||parsed.percent<0)return false;
    out=std::move(parsed);return true;
}
} // namespace PhoneLink
