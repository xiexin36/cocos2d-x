/****************************************************************************
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "cocostudio/CCComAudio.h"
#include "audio/include/SimpleAudioEngine.h"
#include "platform/CCFileUtils.h"

namespace cocostudio {

IMPLEMENT_CLASS_COMPONENT_INFO(ComAudio)

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

static std::string utf8Togbk(const char *src)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
	unsigned short * wszGBK = new unsigned short[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)src, -1, (LPWSTR)wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	if (strTemp.find('?') != std::string::npos)
	{
		strTemp.assign(src);
	}
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
}
#endif



ComAudio::ComAudio(void)
: _filePath("")
, _loop(false)
{
    _name = "CCComAudio";
}

ComAudio::~ComAudio(void)
{
    
}

bool ComAudio::init()
{
    return true;
}

void ComAudio::onEnter()
{
}

void ComAudio::onExit()
{
    stopBackgroundMusic(true);
    stopAllEffects();
}

bool ComAudio::isEnabled() const
{
    return _enabled;
}

void ComAudio::setEnabled(bool b)
{
    _enabled = b;
}


bool ComAudio::serialize(void* r)
{
    bool ret = false;
	do
	{
		CC_BREAK_IF(r == nullptr);
		SerData *serData = (SerData *)(r);
		const rapidjson::Value *v = serData->_rData;
		stExpCocoNode *cocoNode = serData->_cocoNode;
        CocoLoader *cocoLoader = serData->_cocoLoader;
		const char *className = nullptr;
		const char *comName = nullptr;
		const char *file = nullptr;
		std::string filePath;
		int resType = 0;
		bool loop = false;
		if (v != nullptr)
		{
			className = DICTOOL->getStringValue_json(*v, "classname");
			CC_BREAK_IF(className == nullptr);
			comName = DICTOOL->getStringValue_json(*v, "name");
			const rapidjson::Value &fileData = DICTOOL->getSubDictionary_json(*v, "fileData");
			CC_BREAK_IF(!DICTOOL->checkObjectExist_json(fileData));
			file = DICTOOL->getStringValue_json(fileData, "path");
			CC_BREAK_IF(file == nullptr);
			resType = DICTOOL->getIntValue_json(fileData, "resourceType", -1);
			CC_BREAK_IF(resType != 0);
			loop = DICTOOL->getIntValue_json(*v, "loop") != 0? true:false;
		}
		else if (cocoNode != nullptr)
		{
			className = cocoNode[1].GetValue(cocoLoader);
			CC_BREAK_IF(className == nullptr);
			comName = cocoNode[2].GetValue(cocoLoader);
			stExpCocoNode *pfileData = cocoNode[4].GetChildArray(cocoLoader);
			CC_BREAK_IF(!pfileData);
			file = pfileData[0].GetValue(cocoLoader);
			CC_BREAK_IF(file == nullptr);
			resType = atoi(pfileData[2].GetValue(cocoLoader));
			CC_BREAK_IF(resType != 0);
			loop = atoi(cocoNode[5].GetValue(cocoLoader)) != 0? true:false;
			ret = true;
		}
		if (comName != nullptr)
		{
			setName(comName);
		}
		else
		{
			setName(className);
		}
		if (file != nullptr)
		{
            if (strcmp(file, "") == 0)
            {
                continue;
            }
			filePath.assign(cocos2d::FileUtils::getInstance()->fullPathForFilename(file));
		}
		if (strcmp(className, "CCBackgroundAudio") == 0)
		{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
			// no MP3 support for CC_PLATFORM_WP8
			std::string::size_type pos = filePath.find(".mp3");
			if (pos  == filePath.npos)
			{
				continue;
			}
			filePath.replace(pos, filePath.length(), ".wav");
#endif
			preloadBackgroundMusic(filePath.c_str());
			setLoop(loop);
			playBackgroundMusic(filePath.c_str(), loop);
		}
		else if(strcmp(className, "CCComAudio") == 0)
		{
			preloadEffect(filePath.c_str());
		}
		else
		{
			CC_BREAK_IF(true);
		}
		ret = true;
	}while (0);
	return ret;
}

ComAudio* ComAudio::create(void)
{
    ComAudio * pRet = new ComAudio();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
	return pRet;
}

void ComAudio::end()
{
   CocosDenshion::SimpleAudioEngine::end();
}

void ComAudio::preloadBackgroundMusic(const char* pszFilePath)
{
	std::string filePath = utf8Togbk(pszFilePath);
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(filePath.c_str());
	setFile(filePath.c_str());
	setLoop(false);
}

void ComAudio::playBackgroundMusic(const char* pszFilePath, bool loop)
{
	std::string filePath = utf8Togbk(pszFilePath);
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(filePath.c_str(), loop);
}

void ComAudio::playBackgroundMusic(const char* pszFilePath)
{
	std::string filePath = utf8Togbk(pszFilePath);
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(filePath.c_str());
}

void ComAudio::playBackgroundMusic()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(_filePath.c_str(), _loop);
}

void ComAudio::stopBackgroundMusic(bool bReleaseData)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic(bReleaseData);
}

void ComAudio::stopBackgroundMusic()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void ComAudio::pauseBackgroundMusic()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

void ComAudio::resumeBackgroundMusic()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

void ComAudio::rewindBackgroundMusic()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->rewindBackgroundMusic();
}

bool ComAudio::willPlayBackgroundMusic()
{
    return CocosDenshion::SimpleAudioEngine::getInstance()->willPlayBackgroundMusic();
}

bool ComAudio::isBackgroundMusicPlaying()
{
    return CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
}

float ComAudio::getBackgroundMusicVolume()
{
    return CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicVolume();
}

void ComAudio::setBackgroundMusicVolume(float volume)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(volume);
}

float ComAudio::getEffectsVolume()
{
    return CocosDenshion::SimpleAudioEngine::getInstance()->getEffectsVolume();
}

void ComAudio::setEffectsVolume(float volume)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(volume);
}

unsigned int ComAudio::playEffect(const char* pszFilePath, bool loop)
{
	std::string filePath = utf8Togbk(pszFilePath);
	return CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(filePath.c_str(), loop);
}

unsigned int ComAudio::playEffect(const char* pszFilePath)
{
	std::string filePath = utf8Togbk(pszFilePath);
	return CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(filePath.c_str());
}

unsigned int ComAudio::playEffect()
{
	return CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(_filePath.c_str(), _loop);
}

void ComAudio::pauseEffect(unsigned int nSoundId)
{
    return CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(nSoundId);
}

void ComAudio::pauseAllEffects()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();
}

void ComAudio::resumeEffect(unsigned int nSoundId)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(nSoundId);
}

void ComAudio::resumeAllEffects()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();
}

void ComAudio::stopEffect(unsigned int nSoundId)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(nSoundId);
}

void ComAudio::stopAllEffects()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
}

void ComAudio::preloadEffect(const char* pszFilePath)
{
	std::string filePath = utf8Togbk(pszFilePath);
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(filePath.c_str());
	setFile(filePath.c_str());
    setLoop(false);
}

void ComAudio::unloadEffect(const char *pszFilePath)
{
	std::string filePath = utf8Togbk(pszFilePath);
	CocosDenshion::SimpleAudioEngine::getInstance()->unloadEffect(filePath.c_str());
}

void ComAudio::setFile(const char* pszFilePath)
{
	_filePath.assign(pszFilePath);
}

void ComAudio::setLoop(bool loop)
{
	_loop = loop;
}

const char* ComAudio::getFile()
{
	return _filePath.c_str();
}

bool ComAudio::isLoop()
{
	return _loop;
}

}
