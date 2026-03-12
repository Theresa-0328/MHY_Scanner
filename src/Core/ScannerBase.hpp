#pragma once

#include <string>
#include <string_view>

#include "ApiDefs.hpp"

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
             scanUrl = api::mhy::bh3::qrcode_scan;
             confirmUrl = api::mhy::bh3::qrcode_confirm;
         } },
        { "9E&", [this]() {
             gameType = GameType::Genshin;
             scanUrl = api::mhy::hk4e::qrcode_scan;
             confirmUrl = api::mhy::hk4e::qrcode_confirm;
         } },
        { "8F%", [this]() {
             gameType = GameType::HonkaiStarRail;
             scanUrl = api::mhy::hkrpg::qrcode_scan;
             confirmUrl = api::mhy::hkrpg::qrcode_confirm;
         } },
        { "%BA", [this]() {
             gameType = GameType::ZenlessZoneZero;
             scanUrl = api::mhy::nap::qrcode_scan;
             confirmUrl = api::mhy::nap::qrcode_confirm;
         } },
    };
};