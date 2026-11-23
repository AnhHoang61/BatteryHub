// Brand recognition: driver key, vendor id, name, and the look-alikes that must stay plain.
#include <cstdio>

#include "../src/brands.h"

static int failures = 0;

static void Expect(const wchar_t* key, const wchar_t* name, unsigned short vid, DeviceKind kind, Accent want) {
    DeviceStatus s;
    s.key = key;
    s.name = name;
    s.vid = vid;
    s.kind = kind;
    Accent got = BrandOf(s);
    if (got != want) {
        failures++;
        fwprintf(stderr, L"FAIL: %ls / %ls / %04X: got '%ls', want '%ls'\n", key, name, vid, StyleOf(got).name,
                 StyleOf(want).name);
    }
}

int main() {
    using K = DeviceKind;
    // Driver keys.
    Expect(L"razer:00b7", L"Razer DeathAdder V3 Pro", 0x1532, K::Mouse, Accent::Razer);
    Expect(L"blackshark:0001", L"BlackShark V2 Pro", 0, K::Headset, Accent::Razer);
    Expect(L"logi:4099", L"G502 X PLUS", 0x046D, K::Mouse, Accent::Logitech);
    Expect(L"steelseries:12e0", L"Arctis Nova 7", 0, K::Headset, Accent::SteelSeries);
    Expect(L"corsair:0a6b", L"HS80 RGB Wireless", 0, K::Headset, Accent::Corsair);
    Expect(L"hyperx:1718", L"Cloud II Wireless", 0, K::Headset, Accent::HyperX);
    Expect(L"rog:1a85", L"Azoth", 0, K::Keyboard, Accent::Rog);
    Expect(L"zowie:0001", L"EC2-CW", 0, K::Mouse, Accent::Zowie);
    Expect(L"ps:0ce6", L"DualSense", 0, K::Gamepad, Accent::PlayStation);
    Expect(L"inzone:0001", L"INZONE H9", 0, K::Headset, Accent::PlayStation);
    Expect(L"xinput:0:1", L"Xbox controller 1", 0, K::Gamepad, Accent::Xbox);
    Expect(L"galaxybuds:x", L"Buds3 Pro", 0, K::Headset, Accent::Galaxy);
    Expect(L"airpods:x", L"Pods", 0, K::Headset, Accent::Apple);
    Expect(L"jabra:x", L"Evolve2 65", 0, K::Headset, Accent::Jabra);
    Expect(L"jbl:x", L"Quantum 910", 0, K::Headset, Accent::Jbl);
    Expect(L"coolermaster:x", L"MM712", 0, K::Mouse, Accent::CoolerMaster);
    Expect(L"steam:x", L"Controller", 0, K::Gamepad, Accent::Steam);
    Expect(L"redragon:fa7e", L"Invader Pro", 0, K::Mouse, Accent::Redragon);
    // Vendor ids (HeadsetControl and plain Bluetooth / HID devices).
    Expect(L"hsc:1038:12e0", L"Arctis Nova 7", 0x1038, K::Headset, Accent::SteelSeries);
    Expect(L"hsc:1b1c:0a6b", L"Void Wireless", 0x1B1C, K::Headset, Accent::Corsair);
    Expect(L"bt:{x}", L"DSV2 Pro", 0x068E, K::Keyboard, Accent::Razer);
    Expect(L"bt:{x}", L"Wireless Controller", 0x054C, K::Gamepad, Accent::PlayStation);
    Expect(L"bt:{x}", L"Magic Mouse", 0x004C, K::Mouse, Accent::Apple);
    Expect(L"bt:{x}", L"Xbox Wireless Controller", 0x045E, K::Gamepad, Accent::Xbox);
    // Names only.
    Expect(L"bt:{x}", L"MX Master 3S", 0, K::Mouse, Accent::Logitech);
    Expect(L"bt:{x}", L"K380", 0, K::Keyboard, Accent::Logitech);
    Expect(L"bt:{x}", L"HyperX Cloud Mix", 0, K::Headset, Accent::HyperX);
    Expect(L"bt:{x}", L"ASUS ROG Keris II Ace", 0, K::Mouse, Accent::Rog);
    Expect(L"bt:{x}", L"Galaxy Buds2 Pro", 0, K::Headset, Accent::Galaxy);
    Expect(L"bt:{x}", L"JBL Tune 770NC", 0, K::Headset, Accent::Jbl);
    Expect(L"bt:{x}", L"Jabra Elite 8 Active", 0, K::Headset, Accent::Jabra);
    Expect(L"bt:{x}", L"Aerox 5 Wireless", 0, K::Mouse, Accent::SteelSeries);
    Expect(L"aula:3554:fa09", L"Redragon Horus Pro", 0x3554, K::Keyboard, Accent::Redragon);
    Expect(L"phonelink:vivo", L"vivo X100 Ultra", 0, K::Phone, Accent::Vivo);
    Expect(L"bt:{x}", L"vivo X100 Ultra", 0, K::Phone, Accent::Vivo);
    Expect(L"phonelink:iqoo", L"iQOO 13", 0, K::Phone, Accent::Iqoo);
    Expect(L"phonelink:xiaomi", L"Xiaomi 15 Pro", 0, K::Phone, Accent::Xiaomi);
    Expect(L"bt:{x}", L"Redmi Buds 6 Pro", 0, K::Headset, Accent::Xiaomi);
    Expect(L"bt:{x}", L"Mi Portable Mouse", 0, K::Mouse, Accent::Xiaomi);
    Expect(L"bt:{x}", L"POCO F6", 0, K::Phone, Accent::Xiaomi);
    Expect(L"phonelink:hw", L"HUAWEI Mate 70", 0, K::Phone, Accent::Huawei);
    Expect(L"bt:{x}", L"HUAWEI FreeBuds Pro 4", 0, K::Headset, Accent::Huawei);
    Expect(L"bt:{x}", L"MatePad Pro", 0, K::Other, Accent::Huawei);
    Expect(L"bt:{x}", L"HONOR Earbuds", 0, K::Headset, Accent::Honor);
    Expect(L"phonelink:oppo", L"OPPO Find X8", 0, K::Phone, Accent::Oppo);
    Expect(L"bt:{x}", L"Enco Air3", 0, K::Headset, Accent::Oppo);
    Expect(L"phonelink:op", L"OnePlus 13", 0, K::Phone, Accent::OnePlus);
    Expect(L"phonelink:rm", L"realme GT7 Pro", 0, K::Phone, Accent::Realme);
    Expect(L"phonelink:mz", L"Meizu 21", 0, K::Phone, Accent::Meizu);
    Expect(L"rapoo:0001", L"VT9 Pro", 0, K::Mouse, Accent::Rapoo);
    Expect(L"mchose:0001", L"A7 Ultra", 0, K::Mouse, Accent::Mchose);
    Expect(L"vgn:0001", L"VGN F1 Pro", 0, K::Mouse, Accent::Vgn);
    Expect(L"bt:{x}", L"VXE Dragonfly R1", 0, K::Mouse, Accent::Vgn);
    Expect(L"bt:{x}", L"ATK F1 Ultimate", 0, K::Mouse, Accent::Vgn);
    Expect(L"keychron:0001", L"K2 Pro", 0, K::Keyboard, Accent::Keychron);
    Expect(L"aula:0001", L"AULA F75", 0, K::Keyboard, Accent::Aula);
    Expect(L"ajazz:0001", L"AJ159 Pro", 0, K::Mouse, Accent::Ajazz);
    Expect(L"bt:{x}", L"EDIFIER W820NB", 0, K::Headset, Accent::Edifier);
    Expect(L"phonelink:vivo", L"V2366GA", 0, K::Phone, Accent::Vivo);
    Expect(L"bt:{x}", L"Xbox Wireless Controller", 0, K::Gamepad, Accent::Xbox);
    Expect(L"bt:{x}", L"无线控制器", 0, K::Gamepad, Accent::Xbox);
    Expect(L"bt:{x}", L"8BitDo Ultimate 2C", 0, K::Gamepad, Accent::EightBitDo);
    Expect(L"bt:{x}", L"八位堂猎户座", 0, K::Gamepad, Accent::EightBitDo);
    Expect(L"bt:{x}", L"Flydigi Vader 4 Pro", 0, K::Gamepad, Accent::Flydigi);
    Expect(L"bt:{x}", L"飞智冰原狼", 0, K::Gamepad, Accent::Flydigi);
    Expect(L"gamesir:0001", L"Nova Lite", 0, K::Gamepad, Accent::GameSir);
    Expect(L"bt:{x}", L"小鸡启明星", 0, K::Gamepad, Accent::GameSir);
    Expect(L"bt:{x}", L"Beitong KP40", 0, K::Gamepad, Accent::Beitong);
    Expect(L"bt:{x}", L"北通鲲鹏40", 0, K::Gamepad, Accent::Beitong);
    // Expanded phone brands, Chinese names & internal model codes.
    Expect(L"phonelink", L"Pixel 9 Pro", 0, K::Phone, Accent::Google);
    Expect(L"phonelink", L"Sony Xperia 1 VI", 0, K::Phone, Accent::Sony);
    Expect(L"phonelink", L"XQ-DQ72", 0, K::Phone, Accent::Sony);
    Expect(L"phonelink", L"Motorola Razr 50 Ultra", 0, K::Phone, Accent::Motorola);
    Expect(L"phonelink", L"XT2401-2", 0, K::Phone, Accent::Motorola);
    Expect(L"phonelink", L"Nothing Phone (2a)", 0, K::Phone, Accent::Nothing);
    Expect(L"phonelink", L"nubia Z70 Ultra", 0, K::Phone, Accent::Nubia);
    Expect(L"phonelink", L"NX729J", 0, K::Phone, Accent::Nubia);
    Expect(L"phonelink", L"红魔 9S Pro", 0, K::Phone, Accent::Nubia);
    Expect(L"phonelink", L"ASUS Zenfone 11 Ultra", 0, K::Phone, Accent::Asus);
    Expect(L"phonelink", L"联想拯救者 Y90", 0, K::Phone, Accent::Lenovo);
    Expect(L"phonelink", L"Nokia XR21", 0, K::Phone, Accent::Nokia);
    Expect(L"phonelink", L"HTC U24 pro", 0, K::Phone, Accent::Htc);
    Expect(L"phonelink", L"LG Velvet", 0, K::Phone, Accent::Lg);
    Expect(L"phonelink", L"LM-G900N", 0, K::Phone, Accent::Lg);
    Expect(L"phonelink", L"TECNO CAMON 30", 0, K::Phone, Accent::Tecno);
    Expect(L"phonelink", L"SHARP AQUOS R9", 0, K::Phone, Accent::Sharp);
    Expect(L"phonelink", L"Fairphone 5", 0, K::Phone, Accent::Fairphone);
    Expect(L"phonelink", L"BlackBerry Key2", 0, K::Phone, Accent::BlackBerry);
    Expect(L"phonelink", L"坚果 R2", 0, K::Phone, Accent::Smartisan);
    Expect(L"phonelink", L"SM-S9280", 0, K::Phone, Accent::Galaxy);
    Expect(L"phonelink", L"V2405A", 0, K::Phone, Accent::Vivo);
    Expect(L"phonelink", L"24031PN0DC", 0, K::Phone, Accent::Xiaomi);
    Expect(L"phonelink", L"RMX3888", 0, K::Phone, Accent::Realme);
    Expect(L"phonelink", L"CPH2581", 0, K::Phone, Accent::Oppo);
    Expect(L"phonelink", L"Magic6 Pro", 0, K::Phone, Accent::Honor);
    Expect(L"phonelink", L"nova 13 Pro", 0, K::Phone, Accent::Huawei);
    Expect(L"phonelink", L"三星 Galaxy S24 Ultra", 0, K::Phone, Accent::Galaxy);
    Expect(L"phonelink", L"小米 15 Ultra", 0, K::Phone, Accent::Xiaomi);
    Expect(L"phonelink", L"荣耀 Magic7", 0, K::Phone, Accent::Honor);
    Expect(L"phonelink", L"一加 13", 0, K::Phone, Accent::OnePlus);
    Expect(L"bt:{x}", L"Pixel Buds Pro", 0, K::Headset, Accent::Google);
    Expect(L"bt:{x}", L"Device 2717", 0x2717, K::Phone, Accent::Xiaomi);
    Expect(L"bt:{x}", L"Device 12D1", 0x12D1, K::Phone, Accent::Huawei);
    Expect(L"bt:{x}", L"Device 22D9", 0x22D9, K::Phone, Accent::Oppo);
    // Look-alikes that must stay plain.
    Expect(L"bt:{x}", L"HP 430 Multi-Device Mouse", 0x03F0, K::Mouse, Accent::None);
    Expect(L"bt:{x}", L"Surface Precision Mouse", 0x045E, K::Mouse, Accent::None);
    Expect(L"bt:{x}", L"WH-1000XM5", 0x054C, K::Headset, Accent::Sony);
    Expect(L"bt:{x}", L"WH-1000XM3", 0, K::Headset, Accent::Sony);
    Expect(L"bt:{x}", L"WF-1000XM6", 0, K::Headset, Accent::Sony);
    Expect(L"bt:{x}", L"LinkBuds S", 0, K::Headset, Accent::Sony);
    Expect(L"bt:{x}", L"Progress Mouse", 0, K::Mouse, Accent::None);
    Expect(L"bt:{x}", L"Bose QC Ultra Headphones", 0, K::Headset, Accent::None);
    Expect(L"bt:{x}", L"Nova Lite", 0, K::Gamepad, Accent::None);
    Expect(L"bt:{x}", L"V2366 Mouse", 0, K::Mouse, Accent::None);

    if (failures) {
        fwprintf(stderr, L"%d brand test(s) failed\n", failures);
        return 1;
    }
    puts("brands: all passed");
    return 0;
}
