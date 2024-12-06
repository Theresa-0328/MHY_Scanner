#pragma once

#include <string>
#include <string_view>

#include "Common.h"

class ScannerBase
{
public:
    GameType gameType;
    std::string_view scanUrl{};
    std::string_view confirmUrl{};
    std::string lastTicket;
    std::string uid;
    std::string gameToken{};
    std::map<std::string_view, std::function<void()>> setGameType{
        { "8F3", [this]() {
             gameType = GameType::Honkai3;
             scanUrl = mhy_bh3_qrcode_scan;
             confirmUrl = mhy_bh3_qrcode_confirm;
         } },
        { "9E&", [this]() {
             gameType = GameType::Genshin;
             scanUrl = mhy_hk4e_qrcode_scan;
             confirmUrl = mhy_hk4e_qrcode_confirm;
         } },
        { "8F%", [this]() {
             gameType = GameType::HonkaiStarRail;
             scanUrl = mhy_hkrpg_qrcode_scan;
             confirmUrl = mhy_hkrpg_qrcode_confirm;
         } },
        { "%BA", [this]() {
             gameType = GameType::ZenlessZoneZero;
             scanUrl = mhy_nap_cn_qrcode_scan;
             confirmUrl = mhy_nap_cn_qrcode_confirm;
         } },
    };
};