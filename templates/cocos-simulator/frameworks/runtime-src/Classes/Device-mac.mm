#include "Device.h"

#import "openudid/OpenUDIDMac.h"

using namespace std;
PLAYER_NS_BEGIN

Device *Device::getInstance()
{
    static Device *instance = NULL;
    if (!instance)
    {
        instance = new Device();
        instance->init();
    }
    return instance;
}

std::string Device::getCurrentUILangName()
{
    return _uiLangName;
}

std::string Device::getUserGUID()
{
    return _userGUID;
}


//////////  private  //////////

Device::Device()
    : _uiLangName("en")
{

}

void Device::init()
{
    makeUILangName();
    makeUserGUID();
}

void Device::makeUILangName()
{
    NSUserDefaults *defs = [NSUserDefaults standardUserDefaults];
    NSArray *languages = [defs objectForKey:@"AppleLanguages"];
    if ([languages count] > 0)
    {
        NSString *lang = [languages objectAtIndex:0];
        _uiLangName = lang.UTF8String;
    }
}

std::string Device::makeUserGUID()
{
    if (_userGUID.length() <= 0)
    {
        _userGUID = string([[OpenUDIDMac value] cStringUsingEncoding:NSUTF8StringEncoding]);
        
        if (_userGUID.length() <= 0)
        {
            _userGUID = "guid-fixed-1234567890";
        }
    }

    return _userGUID;
}

PLAYER_NS_END
