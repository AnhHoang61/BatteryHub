#include "brands.h"

#include <cwctype>

namespace {

// `dark`: the official colour; brightened (same hue) only for Xbox, PlayStation and Samsung, whose
// brand blues and greens are too dark on a dark taskbar. `light`: a deeper shade with at least 3:1
// contrast on the light taskbar (#F3F3F3); yellow and silver need the most darkening.
const BrandStyle kBrands[] = {
    { Accent::None,         L"",             { 255, 255, 255 }, {   0,   0,   0 } },
    { Accent::Razer,        L"Razer",        {  68, 214,  44 }, {  22, 140,  20 } },  // #44D62C
    { Accent::Logitech,     L"Logitech",     {   0, 184, 252 }, {   0, 120, 180 } },  // Logitech G #00B8FC
    { Accent::SteelSeries,  L"SteelSeries",  { 255,  82,   0 }, { 204,  65,   0 } },  // #FF5200
    { Accent::Corsair,      L"Corsair",      { 245, 228,  26 }, { 150, 122,   0 } },  // yellow #F5E41A
    { Accent::HyperX,       L"HyperX",       { 232,  35,  63 }, { 200,  16,  46 } },  // #E31837
    { Accent::Rog,          L"ROG",          { 255,  46,  77 }, { 208,   0,  42 } },  // ROG red #FF0029
    { Accent::Zowie,        L"ZOWIE",        { 240,  60,  50 }, { 192,  32,  26 } },  // #E2231A
    { Accent::PlayStation,  L"PlayStation",  {  31, 139, 255 }, {   0,  98, 196 } },  // #0070D1
    { Accent::Xbox,         L"Xbox",         {  34, 168,  34 }, {  16, 124,  16 } },  // #107C10
    { Accent::Galaxy,       L"Samsung",      {  91, 123, 255 }, {  20,  40, 160 } },  // #1428A0
    { Accent::Apple,        L"Apple",        { 199, 199, 204 }, { 110, 110, 115 } },  // silver / space grey
    { Accent::Jabra,        L"Jabra",        { 255, 200,   0 }, { 140, 106,   0 } },  // #FFC800
    { Accent::Jbl,          L"JBL",          { 255,  61,   0 }, { 204,  52,   0 } },  // #FF3D00
    { Accent::CoolerMaster, L"Cooler Master",{ 157, 107, 255 }, { 106,  43, 217 } },  // purple
    { Accent::Steam,        L"Steam",        {  26, 159, 255 }, {  11, 111, 191 } },  // #1A9FFF
    { Accent::Redragon,     L"Redragon",     { 230,   0,  18 }, { 192,   0,  15 } },  // #E60012
    // 为什么这么做：补充国内常用手机与外设品牌的专属高辨识度配色，分别针对深色/浅色任务栏调校对比度
    { Accent::Vivo,         L"vivo",         {  82, 122, 255 }, {  25,  60, 210 } },  // vivo Blue #527AFF
    { Accent::Iqoo,         L"iQOO",         { 255, 184,   0 }, { 194, 125,   0 } },  // iQOO Yellow #FFB800
    { Accent::Xiaomi,       L"Xiaomi",       { 255, 105,   0 }, { 204,  82,   0 } },  // Mi Orange #FF6900
    { Accent::Huawei,       L"Huawei",       { 255,  40,  75 }, { 207,  10,  44 } },  // Huawei Red #FF284B
    { Accent::Honor,        L"Honor",        {   0, 200, 255 }, {   0, 122, 163 } },  // Honor Cyan #00C8FF
    { Accent::Oppo,         L"OPPO",         {  26, 209, 103 }, {   4, 138,  62 } },  // OPPO Green #1AD167
    { Accent::OnePlus,      L"OnePlus",      { 255,  30,  60 }, { 200,   0,  30 } },  // OnePlus Red #FF1E3C
    { Accent::Realme,       L"realme",       { 255, 201,  21 }, { 181, 133,   0 } },  // realme Yellow #FFC915
    { Accent::Meizu,        L"Meizu",        {  38, 155, 255 }, {   0, 102, 204 } },  // Meizu Blue #269BFF
    { Accent::Rapoo,        L"Rapoo",        {   0, 210, 210 }, {   0, 130, 135 } },  // Rapoo Teal #00D2D2
    { Accent::Mchose,       L"MCHOSE",       { 140, 100, 255 }, {  90,  48, 200 } },  // MCHOSE Purple #8C64FF
    { Accent::Vgn,          L"VGN",          {   0, 224, 150 }, {   0, 140,  92 } },  // VGN/VXE/ATK Mint #00E096
    { Accent::Keychron,     L"Keychron",     { 255,  90,  38 }, { 196,  56,  12 } },  // Keychron Orange #FF5A26
    { Accent::Aula,         L"AULA",         {  56, 182, 255 }, {   0, 116, 184 } },  // AULA Blue #38B6FF
    { Accent::Ajazz,        L"Ajazz",        { 255,  77, 121 }, { 196,  30,  74 } },  // Ajazz Rose #FF4D79
    { Accent::Edifier,      L"Edifier",      {   0, 196, 180 }, {   0, 128, 118 } },  // Edifier Cyan #00C4B4
    { Accent::EightBitDo,   L"8BitDo",       { 240,  96,  72 }, { 198,  52,  32 } },  // 8BitDo Red-Orange
    { Accent::Flydigi,      L"Flydigi",      {  46, 178, 255 }, {   0, 128, 214 } },  // Flydigi Cyan-Blue
    { Accent::GameSir,      L"GameSir",      { 255,  72,  92 }, { 210,  34,  56 } },  // GameSir Crimson
    { Accent::Beitong,      L"Beitong",      {  36, 212, 245 }, {   0, 152, 188 } },  // Beitong Electric Cyan
    // 为什么这么做：扩展国际与国内主流及小众手机品牌色，分别针对深色与浅色任务栏调校对比度
    { Accent::Google,       L"Google",       {  66, 133, 244 }, {  26,  86, 196 } },  // Google Blue
    // 为什么这么做：Sony 音频（WH-/WF-/LinkBuds）与 Xperia 改用索尼铂金暖金色，避免与 PlayStation 蓝撞色并从原本的“无品牌色”升级为专属识别色
    { Accent::Sony,         L"Sony",         { 222, 184, 114 }, { 146, 108,  34 } },  // Sony Champagne Gold #DEB872
    { Accent::Motorola,     L"Motorola",     {  92, 146, 255 }, {  40,  84, 184 } },  // Motorola Blue
    { Accent::Nothing,      L"Nothing",      { 255,  59,  48 }, { 200,  30,  22 } },  // Nothing Dot Red
    { Accent::Nubia,        L"Nubia",        { 255,  46,  77 }, { 200,  20,  50 } },  // Nubia / RedMagic Red
    { Accent::Asus,         L"ASUS",         {   0, 168, 255 }, {   0, 110, 196 } },  // ASUS Blue
    { Accent::Lenovo,       L"Lenovo",       { 226,  35,  26 }, { 184,  24,  16 } },  // Lenovo Red
    { Accent::Nokia,        L"Nokia",        {  46, 139, 255 }, {   0,  82, 180 } },  // Nokia Blue
    { Accent::Htc,          L"HTC",          { 140, 198,  63 }, {  78, 130,  20 } },  // HTC Green
    { Accent::Lg,           L"LG",           { 217,  30,  99 }, { 165,   0,  52 } },  // LG Magenta
    { Accent::Tecno,        L"TECNO",        {  41, 121, 255 }, {   0,  78, 203 } },  // TECNO / Transsion Blue
    { Accent::Sharp,        L"Sharp",        { 230,   0,  18 }, { 180,   0,  14 } },  // Sharp Red
    { Accent::Fairphone,    L"Fairphone",    {  44, 168, 224 }, {   0, 110, 158 } },  // Fairphone Blue
    { Accent::BlackBerry,   L"BlackBerry",   { 122, 142, 168 }, {  59,  77,  99 } },  // BlackBerry Slate
    { Accent::Smartisan,    L"Smartisan",    { 214,  56,  56 }, { 166,  28,  28 } },  // Smartisan Brick Red
};

std::wstring Lower(std::wstring s) {
    for (auto& c : s) c = towlower(c);
    return s;
}

bool StartsWith(const std::wstring& s, const wchar_t* p) { return s.rfind(p, 0) == 0; }

// `w` as a whole word: "rog" matches "ROG Harpe" and "ASUS ROG Delta", not "Progress".
bool HasWord(const std::wstring& n, const std::wstring& w) {
    for (size_t at = n.find(w); at != std::wstring::npos; at = n.find(w, at + 1)) {
        bool startOk = at == 0 || !iswalnum(n[at - 1]);
        size_t end = at + w.size();
        bool endOk = end == n.size() || !iswalnum(n[end]);
        if (startOk && endOk) return true;
    }
    return false;
}

struct KeyRule { const wchar_t* prefix; Accent a; };
const KeyRule kKeys[] = {
    { L"razer:", Accent::Razer }, { L"blackshark:", Accent::Razer }, { L"barracuda:", Accent::Razer },
    { L"logi:", Accent::Logitech }, { L"g435:", Accent::Logitech },
    { L"steelseries:", Accent::SteelSeries },
    { L"corsair:", Accent::Corsair },
    { L"hyperx:", Accent::HyperX },
    { L"rog:", Accent::Rog },
    { L"zowie:", Accent::Zowie },
    { L"ps:", Accent::PlayStation }, { L"inzone:", Accent::PlayStation },
    { L"xbox:", Accent::Xbox }, { L"xinput:", Accent::Xbox },
    { L"galaxybuds:", Accent::Galaxy },
    { L"airpods:", Accent::Apple },
    { L"jabra:", Accent::Jabra },
    { L"jbl:", Accent::Jbl },
    { L"coolermaster:", Accent::CoolerMaster },
    { L"steam:", Accent::Steam },
    { L"redragon:", Accent::Redragon },
    // 为什么这么做：不将 "aula:" 加入 kKeys，避免拦截同协议下的 Redragon 等其它品牌设备
    { L"rapoo:", Accent::Rapoo },
    { L"mchose:", Accent::Mchose },
    { L"keychron:", Accent::Keychron },
    { L"ajazz:", Accent::Ajazz },
    { L"gamesir:", Accent::GameSir },
    { L"flydigi:", Accent::Flydigi },
    { L"8bitdo:", Accent::EightBitDo },
    { L"beitong:", Accent::Beitong },
};

// Vendor ids that only one of these brands uses. Shared ones are left to the name check:
// 03F0 (HP, also HyperX), 0B05 (ASUS, also ROG), 045E (Microsoft: Xbox pads but also Surface mice).
struct VidRule { unsigned short vid; Accent a; };
const VidRule kVids[] = {
    { 0x1532, Accent::Razer }, { 0x068E, Accent::Razer },  // 068E: Razer keyboards over Bluetooth LE
    { 0x046D, Accent::Logitech },
    { 0x1038, Accent::SteelSeries },
    { 0x1B1C, Accent::Corsair },
    { 0x0951, Accent::HyperX },     // Kingston-era HyperX
    { 0x04A5, Accent::Zowie },      // BenQ
    { 0x054C, Accent::PlayStation },
    { 0x04E8, Accent::Galaxy },
    { 0x05AC, Accent::Apple }, { 0x004C, Accent::Apple },  // USB / Bluetooth SIG company id
    { 0x0B0E, Accent::Jabra },
    { 0x0ECB, Accent::Jbl },
    { 0x2516, Accent::CoolerMaster },
    { 0x28DE, Accent::Steam },
    { 0x2717, Accent::Xiaomi },
    { 0x12D1, Accent::Huawei },
    { 0x22D9, Accent::Oppo },
    { 0x2D95, Accent::Vivo },
    { 0x2A70, Accent::OnePlus },
    { 0x2A45, Accent::Meizu },
    { 0x18D1, Accent::Google },
    { 0x22B8, Accent::Motorola },
    { 0x19D2, Accent::Nubia },
    { 0x17EF, Accent::Lenovo },
    { 0x0421, Accent::Nokia },
    { 0x0BB4, Accent::Htc },
    { 0x1004, Accent::Lg },
    { 0x0FCA, Accent::BlackBerry },
};

enum class How { Contains, Word, Prefix, Exact };
struct NameRule { const wchar_t* text; How how; Accent a; };
const NameRule kNames[] = {
    { L"razer", How::Contains, Accent::Razer },
    { L"logitech", How::Contains, Accent::Logitech }, { L"logi ", How::Prefix, Accent::Logitech },
    // Bluetooth names often leave the brand out: "MX Master 3S", "MX Keys S", "LIFT", "POP Mouse", "Pebble M350".
    { L"mx ", How::Prefix, Accent::Logitech }, { L"lift", How::Prefix, Accent::Logitech },
    { L"pop mouse", How::Prefix, Accent::Logitech }, { L"pop keys", How::Prefix, Accent::Logitech },
    { L"pebble", How::Prefix, Accent::Logitech }, { L"ergo k", How::Prefix, Accent::Logitech },
    { L"ergo m", How::Prefix, Accent::Logitech },
    { L"steelseries", How::Contains, Accent::SteelSeries }, { L"arctis", How::Word, Accent::SteelSeries },
    { L"aerox", How::Word, Accent::SteelSeries }, { L"rival ", How::Prefix, Accent::SteelSeries },
    { L"corsair", How::Contains, Accent::Corsair },
    { L"hyperx", How::Contains, Accent::HyperX }, { L"pulsefire", How::Word, Accent::HyperX },
    { L"rog", How::Word, Accent::Rog },
    { L"zowie", How::Contains, Accent::Zowie },
    { L"dualsense", How::Contains, Accent::PlayStation }, { L"dualshock", How::Contains, Accent::PlayStation },
    { L"playstation", How::Contains, Accent::PlayStation }, { L"inzone", How::Word, Accent::PlayStation },
    { L"wireless controller", How::Exact, Accent::PlayStation },  // DualSense / DualShock 4 Bluetooth name
    { L"xbox", How::Word, Accent::Xbox },
    { L"xinput", How::Word, Accent::Xbox },
    { L"无线控制器", How::Contains, Accent::Xbox },
    { L"galaxy ", How::Prefix, Accent::Galaxy }, { L"samsung", How::Word, Accent::Galaxy }, { L"三星", How::Contains, Accent::Galaxy },
    { L"airpods", How::Contains, Accent::Apple }, { L"magic mouse", How::Prefix, Accent::Apple },
    { L"magic keyboard", How::Prefix, Accent::Apple }, { L"magic trackpad", How::Prefix, Accent::Apple },
    { L"apple", How::Word, Accent::Apple }, { L"iphone", How::Prefix, Accent::Apple }, { L"ipad", How::Prefix, Accent::Apple },
    { L"jabra", How::Word, Accent::Jabra },
    { L"jbl", How::Word, Accent::Jbl },
    { L"cooler master", How::Contains, Accent::CoolerMaster }, { L"coolermaster", How::Contains, Accent::CoolerMaster },
    { L"steam controller", How::Contains, Accent::Steam },
    { L"redragon", How::Contains, Accent::Redragon },
    { L"vivo", How::Word, Accent::Vivo },
    { L"iqoo", How::Word, Accent::Iqoo },
    { L"xiaomi", How::Contains, Accent::Xiaomi }, { L"redmi", How::Contains, Accent::Xiaomi },
    { L"poco", How::Word, Accent::Xiaomi }, { L"mi ", How::Prefix, Accent::Xiaomi },
    { L"小米", How::Contains, Accent::Xiaomi }, { L"红米", How::Contains, Accent::Xiaomi },
    { L"huawei", How::Contains, Accent::Huawei }, { L"freebuds", How::Contains, Accent::Huawei },
    { L"matepad", How::Word, Accent::Huawei }, { L"matebook", How::Word, Accent::Huawei },
    { L"华为", How::Contains, Accent::Huawei }, { L"mate ", How::Prefix, Accent::Huawei }, { L"pura ", How::Prefix, Accent::Huawei },
    { L"honor", How::Word, Accent::Honor }, { L"荣耀", How::Contains, Accent::Honor },
    { L"oppo", How::Word, Accent::Oppo }, { L"enco ", How::Prefix, Accent::Oppo },
    { L"find x", How::Prefix, Accent::Oppo }, { L"find n", How::Prefix, Accent::Oppo }, { L"reno", How::Word, Accent::Oppo },
    { L"oneplus", How::Contains, Accent::OnePlus }, { L"一加", How::Contains, Accent::OnePlus },
    { L"realme", How::Contains, Accent::Realme }, { L"真我", How::Contains, Accent::Realme },
    { L"meizu", How::Contains, Accent::Meizu }, { L"魅族", How::Contains, Accent::Meizu },
    { L"rapoo", How::Contains, Accent::Rapoo },
    { L"mchose", How::Contains, Accent::Mchose },
    { L"vgn", How::Word, Accent::Vgn }, { L"vxe", How::Word, Accent::Vgn },
    { L"atk", How::Word, Accent::Vgn }, { L"dragonfly", How::Word, Accent::Vgn },
    { L"keychron", How::Contains, Accent::Keychron },
    { L"aula", How::Word, Accent::Aula },
    { L"ajazz", How::Contains, Accent::Ajazz },
    { L"edifier", How::Contains, Accent::Edifier },
    { L"8bitdo", How::Contains, Accent::EightBitDo }, { L"八位堂", How::Contains, Accent::EightBitDo },
    { L"flydigi", How::Contains, Accent::Flydigi }, { L"飞智", How::Contains, Accent::Flydigi },
    { L"gamesir", How::Contains, Accent::GameSir }, { L"小鸡", How::Contains, Accent::GameSir },
    { L"beitong", How::Contains, Accent::Beitong }, { L"北通", How::Contains, Accent::Beitong },
    { L"pixel", How::Word, Accent::Google }, { L"google", How::Word, Accent::Google },
    { L"xperia", How::Contains, Accent::Sony }, { L"sony", How::Word, Accent::Sony },
    { L"motorola", How::Contains, Accent::Motorola }, { L"moto ", How::Prefix, Accent::Motorola },
    { L"razr", How::Word, Accent::Motorola }, { L"摩托罗拉", How::Contains, Accent::Motorola },
    { L"nothing", How::Word, Accent::Nothing }, { L"cmf phone", How::Prefix, Accent::Nothing }, { L"cmf buds", How::Prefix, Accent::Nothing },
    { L"nubia", How::Contains, Accent::Nubia }, { L"努比亚", How::Contains, Accent::Nubia },
    { L"redmagic", How::Contains, Accent::Nubia }, { L"red magic", How::Contains, Accent::Nubia }, { L"红魔", How::Contains, Accent::Nubia },
    { L"zte", How::Word, Accent::Nubia }, { L"中兴", How::Contains, Accent::Nubia },
    { L"zenfone", How::Contains, Accent::Asus }, { L"asus", How::Word, Accent::Asus },
    { L"lenovo", How::Contains, Accent::Lenovo }, { L"联想", How::Contains, Accent::Lenovo }, { L"legion", How::Word, Accent::Lenovo },
    { L"nokia", How::Contains, Accent::Nokia }, { L"诺基亚", How::Contains, Accent::Nokia }, { L"hmd ", How::Prefix, Accent::Nokia },
    { L"htc", How::Word, Accent::Htc },
    { L"lg wing", How::Prefix, Accent::Lg }, { L"lg velvet", How::Prefix, Accent::Lg }, { L"lg v60", How::Prefix, Accent::Lg },
    { L"tecno", How::Word, Accent::Tecno }, { L"infinix", How::Word, Accent::Tecno },
    { L"传音", How::Contains, Accent::Tecno }, { L"itel ", How::Prefix, Accent::Tecno },
    { L"aquos", How::Contains, Accent::Sharp }, { L"sharp", How::Word, Accent::Sharp },
    { L"fairphone", How::Contains, Accent::Fairphone },
    { L"blackberry", How::Contains, Accent::BlackBerry },
    { L"smartisan", How::Contains, Accent::Smartisan }, { L"坚果", How::Contains, Accent::Smartisan }, { L"锤子", How::Contains, Accent::Smartisan },
};

bool Matches(const std::wstring& n, const NameRule& r) {
    switch (r.how) {
        case How::Contains: return n.find(r.text) != std::wstring::npos;
        case How::Word:     return HasWord(n, r.text);
        case How::Prefix:   return StartsWith(n, r.text);
        case How::Exact:    return n == r.text;
    }
    return false;
}

// Logitech names that are just a model number: "K380", "M720 Triathlon", "G915", "G502 X".
bool LogitechModel(const std::wstring& n) {
    return n.size() >= 4 && (n[0] == L'g' || n[0] == L'k' || n[0] == L'm') && iswdigit(n[1]) && iswdigit(n[2]) &&
           iswdigit(n[3]) && (n.size() == 4 || !iswalnum(n[4]));
}

// Sony headphones (WH-1000XM5, WF-1000XM4) share PlayStation's vendor id but are not PlayStation gear.
bool SonyAudio(const std::wstring& n) { return StartsWith(n, L"wh-") || StartsWith(n, L"wf-") || HasWord(n, L"linkbuds"); }

// 为什么这么做：Windows Phone Link 与部分蓝牙协议会上报手机内部入网/出厂型号代码（如 V2366GA、SM-S9280、24031PN0DC、RMX3888）；
// 仅在设备确定为手机或来自 phonelink 时启用该型号推断，防止 v2000/magic 等短前缀误伤键鼠外设
Accent PhoneModelBrand(const std::wstring& n) {
    if (StartsWith(n, L"sm-")) return Accent::Galaxy;
    if (n.size() >= 5 && n[0] == L'v' && iswdigit(n[1]) && iswdigit(n[2]) && iswdigit(n[3]) && iswdigit(n[4]))
        return Accent::Vivo;
    if (StartsWith(n, L"rmx") && n.size() >= 7 && iswdigit(n[3]) && iswdigit(n[4]) && iswdigit(n[5]) && iswdigit(n[6]))
        return Accent::Realme;
    if (StartsWith(n, L"nx") && n.size() >= 5 && iswdigit(n[2]) && iswdigit(n[3]) && iswdigit(n[4]))
        return Accent::Nubia;
    if (StartsWith(n, L"xt") && n.size() >= 6 && iswdigit(n[2]) && iswdigit(n[3]) && iswdigit(n[4]) && iswdigit(n[5]))
        return Accent::Motorola;
    if (StartsWith(n, L"xq-")) return Accent::Sony;
    if (StartsWith(n, L"cph") && n.size() >= 7 && iswdigit(n[3]) && iswdigit(n[4]) && iswdigit(n[5]) && iswdigit(n[6]))
        return Accent::Oppo;
    if (StartsWith(n, L"lg-") || StartsWith(n, L"lm-")) return Accent::Lg;
    if (StartsWith(n, L"nova ")) return Accent::Huawei;
    if (n.find(L"magic") != std::wstring::npos) return Accent::Honor;
    if (n.size() >= 9 && n[0] == L'2' && n[1] >= L'1' && n[1] <= L'6' &&
        iswdigit(n[2]) && iswdigit(n[3]) && iswalnum(n[4]))
        return Accent::Xiaomi;
    if (n.size() >= 7 && n[0] == L'm' && n[1] == L'2' && (n[2] == L'0' || n[2] == L'1') &&
        iswdigit(n[3]) && iswdigit(n[4]))
        return Accent::Xiaomi;
    return Accent::None;
}

}  // namespace

const BrandStyle* AllBrands(int& count) {
    count = (int)(sizeof(kBrands) / sizeof(kBrands[0]));
    return kBrands;
}

const BrandStyle& StyleOf(Accent a) {
    for (const auto& b : kBrands)
        if (b.accent == a) return b;
    return kBrands[0];
}

BrandColor AccentColor(Accent a, bool lightTaskbar) {
    const BrandStyle& b = StyleOf(a);
    return lightTaskbar ? b.light : b.dark;
}

Accent BrandOf(const DeviceStatus& s) {
    for (const auto& k : kKeys)
        if (StartsWith(s.key, k.prefix)) return k.a;
    std::wstring n = Lower(s.name);
    if (s.vid)
        for (const auto& v : kVids)
            if (v.vid == s.vid && !(v.a == Accent::PlayStation && SonyAudio(n))) return v.a;
    if (s.vid == 0x045E && s.kind == DeviceKind::Gamepad) return Accent::Xbox;
    // 为什么这么做：索尼音频（如 WF-1000XM6 / WH-1000XM5 / LinkBuds）虽然共用 0x054C VID，但不走 PlayStation 蓝，而使用专属 Sony 配色
    if (SonyAudio(n)) return Accent::Sony;
    for (const auto& r : kNames)
        if (Matches(n, r)) return r.a;
    if (s.kind == DeviceKind::Phone || StartsWith(s.key, L"phonelink")) {
        Accent phone = PhoneModelBrand(n);
        if (phone != Accent::None) return phone;
    }
    return LogitechModel(n) ? Accent::Logitech : Accent::None;
}
