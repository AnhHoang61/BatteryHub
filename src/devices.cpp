// Runs every driver in order. Vendor drivers go first and claim the devices they handle; the
// generic fallbacks then only look at what is left.
#include "devices.h"

#include <algorithm>
#include <cstdio>
#include <cwchar>
#include <cwctype>

#include "drivers/drivers.h"
#include "hid.h"

namespace {

// 为什么这么做：设备在插线充电（USB 有线 / 2.4G / 蓝牙并存）时常以不同驱动条目和连接后缀同时上报；
// 剥离连接方式噪点词后提取归一化基础名称，用于跨驱动去重与清除旧通道残影
std::wstring NormalizeDeviceBaseName(const std::wstring& name) {
    static const wchar_t* kNoise[] = {
        L"wired", L"wireless", L"dongle", L"receiver", L"hyperspeed", L"hyperpolling", L"omni", L"speednova",
        L"lightspeed", L"slipstream", L"8khz", L"8000hz", L"4k", L"4000hz", L"2.4g", L"2.4ghz", L"ghz",
        L"bluetooth", L"bt", L"ble", L"usb", L"cable", L"adapter",
        L"有线", L"无线", L"接收器", L"适配器", L"蓝牙",
    };
    auto isNoise = [&](const std::wstring& tok) {
        for (const wchar_t* w : kNoise) if (tok == w) return true;
        return false;
    };
    std::vector<std::wstring> tokens;
    std::wstring cur;
    for (wchar_t ch : name) {
        ch = towlower(ch);
        if (iswalnum(ch) || ch == L'.') {
            cur.push_back(ch);
        } else if (!cur.empty()) {
            if (!isNoise(cur)) tokens.push_back(cur);
            cur.clear();
        }
    }
    if (!cur.empty() && !isNoise(cur)) tokens.push_back(cur);
    std::wstring joined;
    for (size_t i = 0; i < tokens.size(); i++) {
        if (i) joined.push_back(L' ');
        joined += tokens[i];
    }
    return joined;
}

int DevicePriority(const DeviceStatus& s) {
    return (s.connected ? 8 : 0) + (!s.asleep ? 4 : 0) + (s.charging ? 2 : 0) + (s.percent >= 0 ? 1 : 0);
}

void DeduplicateByDevice(std::vector<DeviceStatus>& out) {
    std::vector<DeviceStatus> merged;
    merged.reserve(out.size());
    for (auto& s : out) {
        bool combined = false;
        for (auto& m : merged) {
            if (SamePhysicalDevice(m, s)) {
                if (DevicePriority(s) > DevicePriority(m)) {
                    bool chg = m.charging || s.charging;
                    std::wstring detail = !s.detail.empty() ? s.detail : m.detail;
                    int pct = s.percent >= 0 ? s.percent : m.percent;
                    m = s;
                    m.charging = chg;
                    m.detail = detail;
                    m.percent = pct;
                } else {
                    m.charging |= s.charging;
                    if (m.percent < 0 && s.percent >= 0) m.percent = s.percent;
                    if (m.detail.empty() && !s.detail.empty()) m.detail = s.detail;
                }
                combined = true;
                break;
            }
        }
        if (!combined) merged.push_back(std::move(s));
    }
    out = std::move(merged);
}

// HKCU\Software\WirelessBattery\IgnoreDevices (REG_SZ): "VID:PID" pairs in hex, separated by spaces,
// commas or semicolons (e.g. "28BD:0947, 046D:C547"). No driver ever opens those devices (issue #232).
std::vector<std::pair<USHORT, USHORT>> IgnoredDevices() {
    std::vector<std::pair<USHORT, USHORT>> out;
    wchar_t buf[1024];
    DWORD size = sizeof(buf);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\WirelessBattery", L"IgnoreDevices", RRF_RT_REG_SZ, nullptr, buf, &size) !=
        ERROR_SUCCESS)
        return out;
    for (const wchar_t* p = buf; *p;) {
        unsigned vid = 0, pid = 0;
        int used = 0;
        if (swscanf(p, L" %x:%x%n", &vid, &pid, &used) == 2 && used > 0) {
            out.push_back({ (USHORT)vid, (USHORT)pid });
            p += used;
        } else {
            p++;
        }
        while (*p == L',' || *p == L';' || *p == L' ') p++;
    }
    return out;
}

}  // namespace

std::vector<DeviceStatus> PollAllDevices() {
    PollContext ctx;
    ctx.colls = EnumHid();
    auto ignored = IgnoredDevices();
    if (!ignored.empty())
        ctx.colls.erase(std::remove_if(ctx.colls.begin(), ctx.colls.end(), [&](const HidColl& c) {
                            return std::find(ignored.begin(), ignored.end(), std::make_pair(c.vid, c.pid)) != ignored.end();
                        }),
                        ctx.colls.end());
    PollSinowealthReceiver(ctx);
    PollLeobogCable(ctx);  // before Glorious, which probes every unclaimed 258A product
    PollGlorious(ctx);
    PollAula(ctx);       // before Kysona (shares the 3554 vendor id) and SONiX (shares 05AC:024F)
    PollAulaMouse(ctx);
    PollEweadn(ctx);     // EWEADN S7 / S9 mice on 372E:103D / 1027 (AULA keeps 372E:106C)
    PollDareu(ctx);
    PollZenith(ctx);
    PollDarmoshark(ctx);
    PollDarmosharkNrf(ctx);
    PollAtkSerial(ctx);  // before Kysona, which shares the ATK vendor id
    PollAtkKeyboard(ctx);
    PollRedragonEvision(ctx);
    PollPulsar(ctx);     // before Kysona, which also scans the 3710 vendor id
    PollAtkBitmouse(ctx);  // before Kysona, which would probe the NK dongle with the wrong protocol
    PollChimera(ctx);      // Chimera V2 mouse (3554:F512), before Kysona which shares the 3554 vendor id
    PollKysona(ctx);
    PollKeychron(ctx);
    PollCherry(ctx);
    PollNuphy(ctx);
    PollArbiter(ctx);    // before Lamzu, which shares the 373E vendor id
    PollLamzu(ctx);
    PollEndgame(ctx);
    PollEndgameWe(ctx);
    PollVaxee(ctx);
    PollWlmouse(ctx);
    PollFinalmouse(ctx);
    PollScyrox(ctx);
    PollNinjutso(ctx);
    PollSprime(ctx);
    PollRawm(ctx);
    PollAkko(ctx);
    PollRongyuan(ctx);
    PollSonix(ctx);
    PollViaDongle(ctx);
    PollSemico(ctx);
    PollAjazzAk992(ctx);
    PollSonyInzone(ctx);
    PollFractal(ctx);
    PollJabra(ctx);
    PollJbl(ctx);
    PollRog(ctx);
    PollZowie(ctx);
    PollRoyalKludge(ctx);
    PollMchose(ctx);
    PollMchoseA5(ctx);
    PollRapoo(ctx);
    PollAreson(ctx);
    PollMchoseG3(ctx);
    PollMsi(ctx);
    PollMachenike(ctx);  // before Ajazz in PollMiscMice, which shares the L7's 248A vendor id
    PollNiceboy(ctx);    // exact 248A:FA02 match, also before the Ajazz / Delux pollers
    PollMchoseModern(ctx);
    PollMchoseHeadset(ctx);
    PollVgn(ctx);
    PollMchoseKeyboard(ctx);  // 3837:3033 receiver, before the generic fallbacks
    PollMiscMice(ctx);
    PollFantech(ctx);
    PollRaigor(ctx);
    PollRazerHeadsets(ctx);  // before the generic Razer driver, which shares the vendor id
    PollBarracudaPro(ctx);   // before the generic Razer driver as well
    PollHyperX(ctx);
    PollLogitechG435(ctx);  // before the HID++ driver, which shares the vendor id
    PollAstro(ctx);
    PollLogitech(ctx);
    PollRazer(ctx);
    PollSteelSeriesElite(ctx);
    PollSteelSeries(ctx);
    PollAudeze(ctx);
    PollEndorfy(ctx);
    PollTurtleBeach(ctx);
    PollAkg(ctx);
    PollNzxt(ctx);
    PollPlantronics(ctx);
    PollCorsair(ctx);
    PollCoolerMaster(ctx);
    PollPlayStation(ctx);
    PollSwitchPro(ctx);
    PollSteamController(ctx);
    PollGameSir(ctx);
    PollFlydigi(ctx);
    PollGameSirCyclone(ctx);
    PollGameSirSapjoy(ctx);  // Tarantula Pro / NovaPro: SAPJoy device info on FF A0 (Nova and Cyclone 2 above keep their ids)
    PollGameSirBle(ctx);  // Cyclone Pro: battery over BLE; claims its dongle before the XInput fallback
    Poll8BitDo(ctx);
    PollBeitong(ctx);  // before the XInput fallback, which only sees the receiver as wired
    PollAirPods(ctx);  // before the Bluetooth fallback, which has no level for AirPods
    PollPhoneLink(ctx);
    PollGalaxyBuds(ctx);  // before the Bluetooth fallback, which skips devices already listed by name
    PollNothingEar(ctx);  // before the Bluetooth fallback, which skips devices already listed by name
    PollMoondrop(ctx);
    PollDarkrim(ctx);
    PollAlienware(ctx);
    PollMaono(ctx);  // MAONO PD200W receiver (352F:040E), experimental battery query
    PollPressPlayIcarus(ctx);
    PollHeadsetControl(ctx);
    PollBluetooth(ctx);   // before the generic HID check: Windows' own level wins for the same device
    PollGenericHid(ctx);
    PollXInput(ctx);
    DeduplicateByDevice(ctx.out);
    return ctx.out;
}

bool SamePhysicalDevice(const DeviceStatus& a, const DeviceStatus& b) {
    if (a.kind != b.kind) return false;
    std::wstring na = NormalizeDeviceBaseName(a.name);
    if (na.empty()) return false;
    return na == NormalizeDeviceBaseName(b.name);
}

std::wstring DumpHidCollections() {
    std::wstring out = L"VID:PID    IF  UsagePg:Usage  In  Out Feat  Product\n";
    for (const auto& c : EnumHid()) {
        wchar_t line[160];
        swprintf(line, 160, L"%04X:%04X  %2d  %04X:%04X    %3u %3u %3u   ", c.vid, c.pid, c.iface, c.page(), c.usage(), c.inLen(), c.outLen(),
                 c.featLen());
        out += line + HidProduct(c) + L"\n";
    }
    // 为什么这么做：已精简离线反馈模块 report，不再追加 report::DumpExtras() 的诊断片段
    out += DumpXentaFrames();
    out += DumpAstroFrames();
    return out;
}
