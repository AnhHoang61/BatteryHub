// LEOBOG 3-mode keyboards (Hi8 / Hi75 / Hi86 / A75 ...) over the USB cable (SinoWealth 258A:010C).
// On the cable the keyboard reports no battery level at all (the vendor app LEOBOG ONE ships
// ShowPower=0 for these models), but it is charging, so it is listed as a keyboard on USB power
// instead of not at all. Over 2.4 GHz it uses the 3554:FA09 receiver, which the AULA driver reads.
// Identified by the vendor configuration collection LEOBOG ONE talks to: interface 1, FF00:0001,
// 520-byte feature report.
#include "drivers.h"

void PollLeobogCable(PollContext& ctx) {
    if (ctx.IsClaimed(0x258A, 0x010C)) return;
    auto found = ctx.Find(0x258A, 0x010C, [](const HidColl& c) {
        return c.iface == 1 && c.page() == 0xFF00 && c.usage() == 1 && c.featLen() == 520;
    });
    if (found.empty()) return;
    DeviceStatus s;
    s.key = L"leobog:258A:010C";
    s.name = L"LEOBOG keyboard";
    s.kind = DeviceKind::Keyboard;
    s.via = L"USB cable";
    s.charging = true;     // on USB power; the level itself is not reported over the cable
    s.levelText = L"USB";  // shown instead of a percentage
    ctx.Add(s, 0x258A, 0x010C);
}
