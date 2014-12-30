#pragma once

#include <string>
#include "PlayerMacros.h"

PLAYER_NS_BEGIN

class Device
{
public:
    static Device *getInstance();

    std::string getCurrentUILangName();
    std::string getUserGUID();

private:
    Device();
    void init();
    void makeUILangName();
    std::string makeUserGUID();

    std::string _uiLangName;
    std::string _userGUID;
};

PLAYER_NS_END