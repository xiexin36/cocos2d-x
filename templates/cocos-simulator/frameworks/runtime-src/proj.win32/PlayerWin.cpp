﻿
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "stdafx.h"
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <Commdlg.h>
#include <Shlobj.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <shellapi.h>

#include "PlayerWin.h"

#include "glfw3.h"
#include "glfw3native.h"

#include "CCLuaEngine.h"
#include "AppEvent.h"
#include "AppLang.h"
#include "ConfigParser.h"

USING_NS_CC;

static WNDPROC g_oldWindowProc = NULL;
INT_PTR CALLBACK AboutDialogCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
void onHelpAbout()
{
    DialogBox(GetModuleHandle(NULL), 
        MAKEINTRESOURCE(IDD_DIALOG_ABOUT), 
        Director::getInstance()->getOpenGLView()->getWin32Window(), 
        AboutDialogCallback);
}

void shutDownApp()
{
    auto glview = dynamic_cast<GLViewImpl*> (Director::getInstance()->getOpenGLView());
    HWND hWnd = glview->getWin32Window();
    ::SendMessage(hWnd, WM_CLOSE, NULL, NULL);
}

std::string getCurAppPath(void)
{
    TCHAR szAppDir[MAX_PATH] = { 0 };
    if (!GetModuleFileName(NULL, szAppDir, MAX_PATH))
        return "";
    int nEnd = 0;
    for (int i = 0; szAppDir[i]; i++)
    {
        if (szAppDir[i] == '\\')
            nEnd = i;
    }
    szAppDir[nEnd] = 0;
    int iLen = 2 * wcslen(szAppDir);
    char* chRtn = new char[iLen + 1];
    wcstombs(chRtn, szAppDir, iLen + 1);
    std::string strPath = chRtn;
    delete[] chRtn;
    chRtn = NULL;
    char fuldir[MAX_PATH] = { 0 };
    _fullpath(fuldir, strPath.c_str(), MAX_PATH);
    return fuldir;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    auto player = player::PlayerWin::getInstance();
    return player->run();
}

PLAYER_NS_BEGIN

PlayerWin *PlayerWin::_instance = nullptr;

PlayerWin::PlayerWin()
    : _app(nullptr)
    , _hwnd(NULL)
    , _hwndConsole(NULL)
    , _writeDebugLogFile(nullptr)
    , _messageBoxService(nullptr)
    , _menuService(nullptr)
    , _editboxService(nullptr)
    , _taskService(nullptr)
{
}

PlayerWin::~PlayerWin()
{
    CC_SAFE_DELETE(_menuService);
    CC_SAFE_DELETE(_messageBoxService);
    CC_SAFE_DELETE(_fileDialogService);
    CC_SAFE_DELETE(_app);
    if (_writeDebugLogFile)
    {
        fclose(_writeDebugLogFile);
    }

    _instance = nullptr;
}

PlayerWin *PlayerWin::getInstance()
{
    if (!_instance)
    {
        _instance = new PlayerWin();
    }
    return _instance;
}

PlayerFileDialogServiceProtocol *PlayerWin::getFileDialogService()
{
    return _fileDialogService;
}

PlayerMessageBoxServiceProtocol *PlayerWin::getMessageBoxService()
{
    return _messageBoxService;
}

PlayerMenuServiceProtocol *PlayerWin::getMenuService()
{
    return _menuService;
}

PlayerEditBoxServiceProtocol *PlayerWin::getEditBoxService()
{
    return _editboxService;
}

PlayerTaskServiceProtocol *PlayerWin::getTaskService()
{
    return _taskService;
}

void PlayerWin::quit()
{
    Director::getInstance()->end();
}

void PlayerWin::relaunch()
{
    _project.setWindowOffset(Vec2(getPositionX(), getPositionY()));
    openNewPlayerWithProjectConfig(_project);

    quit();
}

void PlayerWin::openNewPlayer()
{
    openNewPlayerWithProjectConfig(_project);
}

void PlayerWin::openNewPlayerWithProjectConfig(const ProjectConfig &config)
{
    static long taskid = 100;
    stringstream buf;
    buf << taskid++;

    string commandLine;
    commandLine.append(getApplicationExePath());
    if (!config.isWelcome())
    {
        commandLine.append(" ");
        commandLine.append(config.makeCommandLine());
    }
    
    CCLOG("PlayerWin::openNewPlayerWithProjectConfig(): %s", commandLine.c_str());

    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms682499(v=vs.85).aspx
    SECURITY_ATTRIBUTES sa = {0};
    sa.nLength = sizeof(sa);

    PROCESS_INFORMATION pi = {0};
    STARTUPINFO si = {0};
    si.cb = sizeof(STARTUPINFO);

#define MAX_COMMAND 1024 // lenth of commandLine is always beyond MAX_PATH

    WCHAR command[MAX_COMMAND];
    memset(command, 0, sizeof(command));
    MultiByteToWideChar(CP_UTF8, 0, commandLine.c_str(), -1, command, MAX_COMMAND);

    BOOL success = CreateProcess(NULL,
                                 command,   // command line 
                                 NULL,      // process security attributes 
                                 NULL,      // primary thread security attributes 
                                 FALSE,     // handles are inherited 
                                 0,         // creation flags 
                                 NULL,      // use parent's environment 
                                 NULL,      // use parent's current directory 
                                 &si,       // STARTUPINFO pointer 
                                 &pi);      // receives PROCESS_INFORMATION 

    if (!success)
    {
        CCLOG("PlayerTaskWin::run() - create process failed, for execute %s", commandLine.c_str());
    }
}

void PlayerWin::openProjectWithProjectConfig(const ProjectConfig &config)
{
    openNewPlayerWithProjectConfig(config);
    quit();
}

int PlayerWin::getPositionX()
{
    RECT rect;
    GetWindowRect(_hwnd, &rect);
    return rect.left;
}

int PlayerWin::getPositionY()
{
    RECT rect;
    GetWindowRect(_hwnd, &rect);
    return rect.top;
}

int PlayerWin::run()
{
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    parseCocosProjectConfig(_project);

    // load project config from command line args
    vector<string> args;
    for (int i = 0; i < __argc; ++i)
    {
        wstring ws(__wargv[i]);
        string s;
        s.assign(ws.begin(), ws.end());
        args.push_back(s);
    }
    _project.parseCommandLine(args);

    if (_project.getProjectDir().empty())
    {
        if (args.size() == 2)
        {
            // for Code IDE before RC2
            _project.setProjectDir(args.at(1));
            _project.setDebuggerType(kCCLuaDebuggerCodeIDE);
        }
    }

    // create the application instance
    _app = new AppDelegate();
    _app->setProjectConfig(_project);

    // create console window
    if (_project.isShowConsole())
    {
        AllocConsole();
        _hwndConsole = GetConsoleWindow();
        if (_hwndConsole != NULL)
        {
            ShowWindow(_hwndConsole, SW_SHOW);
            BringWindowToTop(_hwndConsole);
            freopen("CONOUT$", "wt", stdout);
            freopen("CONOUT$", "wt", stderr);

            HMENU hmenu = GetSystemMenu(_hwndConsole, FALSE);
            if (hmenu != NULL)
            {
                DeleteMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
            }
        }
    }

    // log file
    if (_project.isWriteDebugLogToFile())
    {
        const string debugLogFilePath = _project.getDebugLogFilePath();
        _writeDebugLogFile = fopen(debugLogFilePath.c_str(), "w");
        if (!_writeDebugLogFile)
        {
            CCLOG("Cannot create debug log file %s", debugLogFilePath.c_str());
        }
    }

    // set environments
    SetCurrentDirectoryA(_project.getProjectDir().c_str());
    //FileUtils::getInstance()->setSearchRootPath(_project.getProjectDir().c_str());
    //FileUtils::getInstance()->setWritablePath(_project.getWritableRealPath().c_str());

    // check screen DPI
    HDC screen = GetDC(0);
    int dpi = GetDeviceCaps(screen, LOGPIXELSX);
    ReleaseDC(0, screen);

    // set scale with DPI
    //  96 DPI = 100 % scaling
    // 120 DPI = 125 % scaling
    // 144 DPI = 150 % scaling
    // 192 DPI = 200 % scaling
    // http://msdn.microsoft.com/en-us/library/windows/desktop/dn469266%28v=vs.85%29.aspx#dpi_and_the_desktop_scaling_factor
    //
    // enable DPI-Aware with DeclareDPIAware.manifest
    // http://msdn.microsoft.com/en-us/library/windows/desktop/dn469266%28v=vs.85%29.aspx#declaring_dpi_awareness
    float screenScale = 1.0f;
    if (dpi >= 120 && dpi < 144)
    {
        screenScale = 1.25f;
    }
    else if (dpi >= 144 && dpi < 192)
    {
        screenScale = 1.5f;
    }
    else if (dpi >= 192)
    {
        screenScale = 2.0f;
    }
    CCLOG("SCREEN DPI = %d, SCREEN SCALE = %0.2f", dpi, screenScale);

    // create opengl view
    Size frameSize = _project.getFrameSize();
    float frameScale = 1.0f;
    if (_project.isRetinaDisplay())
    {
        frameSize.width *= screenScale;
        frameSize.height *= screenScale;
    }
    else
    {
        frameScale = screenScale;
    }

    const Rect frameRect = Rect(0, 0, frameSize.width, frameSize.height);
    const bool isResize = _project.isResizeWindow();
    std::stringstream title;
    title << "Cocos Simulator - " << ConfigParser::getInstance()->getInitViewName();
	auto glview = GLViewImpl::createWithRectWithglfwInit(title.str(), frameRect, frameScale);
    _hwnd = glview->getWin32Window();
    //SendMessage(_hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
    //SendMessage(_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
    //FreeResource(icon);

    auto director = Director::getInstance();
    director->setOpenGLView(glview);

    director->setAnimationInterval(1.0 / 60.0);

    // set window position
    if (_project.getProjectDir().length())
    {
        setZoom(_project.getFrameScale()); 
    }
    Vec2 pos = _project.getWindowOffset();
    if (pos.x != 0 && pos.y != 0)
    {
        RECT rect;
        GetWindowRect(_hwnd, &rect);
        MoveWindow(_hwnd, pos.x, pos.y, rect.right - rect.left, rect.bottom - rect.top, FALSE);
    }

    // register event handlers
    auto eventDispatcher = director->getEventDispatcher();
    eventDispatcher->addCustomEventListener("APP.VIEW_SCALE", CC_CALLBACK_1(PlayerWin::onWindowScale, this));

    // path for looking Lang file, Studio Default images
    FileUtils::getInstance()->addSearchPath(getApplicationPath().c_str());

    // init player services
    initServices();
    setupUI();
    DrawMenuBar(_hwnd);

    // prepare
    FileUtils::getInstance()->setPopupNotify(false);
    _project.dump();
    auto app = Application::getInstance();

    g_oldWindowProc = (WNDPROC)SetWindowLong(_hwnd, GWL_WNDPROC, (LONG)PlayerWin::windowProc);

    const ProjectConfig &project = _project;

    // update window size
    RECT rect;
    GetWindowRect(_hwnd, &rect);
    MoveWindow(_hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top + GetSystemMetrics(SM_CYMENU), FALSE);



    // startup message loop
    return app->run();
}

// services
void PlayerWin::initServices()
{
    CCASSERT(_menuService == nullptr, "CAN'T INITIALIZATION SERVICES MORE THAN ONCE");
    _menuService = new PlayerMenuServiceWin(_hwnd);
    _messageBoxService = new PlayerMessageBoxServiceWin(_hwnd);
    _fileDialogService = new PlayerFileDialogServiceWin(_hwnd);
    _editboxService = new PlayerEditBoxServiceWin(_hwnd);
    _taskService = new PlayerTaskServiceWin(_hwnd);

    if (!_project.isAppMenu())
    {
        // remove menu
        SetMenu(_hwnd, NULL);
    }
}

void PlayerWin::onWindowScale(EventCustom* event)
{
    //float scale = atof(event->getDataString().c_str());
    //setZoom(scale);

    //if (_project.isAppMenu() && GetMenu(_hwnd))
    //{
    //    // update window size
    //    RECT rect;
    //    GetWindowRect(_hwnd, &rect);
    //    MoveWindow(_hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top + GetSystemMetrics(SM_CYMENU), FALSE);
    //}
}

void PlayerWin::setupUI()
{
    auto menuBar = player::PlayerProtocol::getInstance()->getMenuService();

    // FILE
    menuBar->addItem("FILE_MENU", tr("File"));
    menuBar->addItem("EXIT_MENU", tr("Exit"), "FILE_MENU");

    // VIEW
    menuBar->addItem("VIEW_MENU", tr("View"));
    SimulatorConfig *config = SimulatorConfig::getInstance();
    int current = config->checkScreenSize(_project.getFrameSize());
    for (int i = 0; i < config->getScreenSizeCount(); i++)
    {
        SimulatorScreenSize size = config->getScreenSize(i);
        std::stringstream menuId;
        menuId << "VIEWSIZE_ITEM_MENU_" << i;
        auto menuItem = menuBar->addItem(menuId.str(), size.title.c_str(), "VIEW_MENU");

        if (i == current)
        {
            menuItem->setChecked(true);
        }
    }

    menuBar->addItem("DIRECTION_MENU_SEP", "-", "VIEW_MENU");
    menuBar->addItem("DIRECTION_PORTRAIT_MENU", tr("Portrait"), "VIEW_MENU")
        ->setChecked(_project.isPortraitFrame());
    menuBar->addItem("DIRECTION_LANDSCAPE_MENU", tr("Landscape"), "VIEW_MENU")
        ->setChecked(_project.isLandscapeFrame());

    menuBar->addItem("VIEW_SCALE_MENU_SEP", "-", "VIEW_MENU");
    std::vector<player::PlayerMenuItem*> scaleMenuVector;
    auto scale100Menu = menuBar->addItem("VIEW_SCALE_MENU_100", tr("Zoom Out").append(" (100%)"), "VIEW_MENU");
    auto scale75Menu = menuBar->addItem("VIEW_SCALE_MENU_75", tr("Zoom Out").append(" (75%)"), "VIEW_MENU");
    auto scale50Menu = menuBar->addItem("VIEW_SCALE_MENU_50", tr("Zoom Out").append(" (50%)"), "VIEW_MENU");
    auto scale25Menu = menuBar->addItem("VIEW_SCALE_MENU_25", tr("Zoom Out").append(" (25%)"), "VIEW_MENU");
    int frameScale = int(_project.getFrameScale() * 100);
    if (frameScale == 100)
    {
        scale100Menu->setChecked(true);
    }
    else if (frameScale == 75)
    {
        scale75Menu->setChecked(true);
    }
    else if (frameScale == 50)
    {
        scale50Menu->setChecked(true);
    }
    else if (frameScale == 25)
    {
        scale25Menu->setChecked(true);
    }
    else
    {
        scale100Menu->setChecked(true);
    }

    scaleMenuVector.push_back(scale100Menu);
    scaleMenuVector.push_back(scale75Menu);
    scaleMenuVector.push_back(scale50Menu);
    scaleMenuVector.push_back(scale25Menu);

    menuBar->addItem("REFRESH_MENU_SEP", "-", "VIEW_MENU");
    menuBar->addItem("REFRESH_MENU", tr("Refresh"), "VIEW_MENU");

    HWND &hwnd = _hwnd;
    ProjectConfig &project = _project;
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithFixedPriority(EventListenerCustom::create("APP.EVENT", [&project, &hwnd, scaleMenuVector](EventCustom* event){
        auto menuEvent = dynamic_cast<AppEvent*>(event);
        if (menuEvent)
        {
            rapidjson::Document dArgParse;
            dArgParse.Parse<0>(menuEvent->getDataString().c_str());
            if (dArgParse.HasMember("name"))
            {
                string strcmd = dArgParse["name"].GetString();

                if (strcmd == "menuClicked")
                {
                    player::PlayerMenuItem *menuItem = static_cast<player::PlayerMenuItem*>(menuEvent->getUserData());
                    if (menuItem)
                    {
                        if (menuItem->isChecked())
                        {
                            return;
                        }

                        string data = dArgParse["data"].GetString();
                        auto player = player::PlayerProtocol::getInstance();

                        if ((data == "CLOSE_MENU") || (data == "EXIT_MENU"))
                        {
                            player->quit();
                        }
                        else if (data == "REFRESH_MENU")
                        {
                            player->relaunch();
                        }
                        else if (data.find("VIEW_SCALE_MENU_") == 0) // begin with VIEW_SCALE_MENU_
                        {
                            string tmp = data.erase(0, strlen("VIEW_SCALE_MENU_"));
                            float scale = atof(tmp.c_str()) / 100.0f;
                            project.setFrameScale(scale);

                            auto glview = static_cast<GLViewImpl*>(Director::getInstance()->getOpenGLView());
                            glview->setFrameZoomFactor(scale);

                            // update scale menu state
                            for (auto &it : scaleMenuVector)
                            {
                                it->setChecked(false);
                            }
                            menuItem->setChecked(true);

                            // update window size
                            RECT rect;
                            GetWindowRect(hwnd, &rect);
                            MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top + GetSystemMetrics(SM_CYMENU), FALSE);
                        
                            // fix: can not update window on some windows system 
                            ::SendMessage(hwnd, WM_MOVE, NULL, NULL);
                        }
                        else if (data.find("VIEWSIZE_ITEM_MENU_") == 0) // begin with VIEWSIZE_ITEM_MENU_
                        {
                            string tmp = data.erase(0, strlen("VIEWSIZE_ITEM_MENU_"));
                            int index = atoi(tmp.c_str());
                            SimulatorScreenSize size = SimulatorConfig::getInstance()->getScreenSize(index);

                            if (project.isLandscapeFrame())
                            {
                                std::swap(size.width, size.height);
                            }

                            project.setFrameSize(cocos2d::Size(size.width, size.height));
                            project.setWindowOffset(cocos2d::Vec2(player->getPositionX(), player->getPositionY()));
                            player->openProjectWithProjectConfig(project);
                        }
                        else if (data == "DIRECTION_PORTRAIT_MENU")
                        {
                            project.changeFrameOrientationToPortait();
                            player->openProjectWithProjectConfig(project);
                        }
                        else if (data == "DIRECTION_LANDSCAPE_MENU")
                        {
                            project.changeFrameOrientationToLandscape();
                            player->openProjectWithProjectConfig(project);
                        }
                    }
                }
            }
        }
    }), 1);

}

void PlayerWin::setZoom(float frameScale)
{
    _project.setFrameScale(frameScale);
    cocos2d::Director::getInstance()->getOpenGLView()->setFrameZoomFactor(frameScale);
}

// debug log
void PlayerWin::writeDebugLog(const char *log)
{
    if (!_writeDebugLogFile) return;

    fputs(log, _writeDebugLogFile);
    fputc('\n', _writeDebugLogFile);
    fflush(_writeDebugLogFile);
}

void PlayerWin::parseCocosProjectConfig(ProjectConfig &config)
{
    // get project directory
    ProjectConfig tmpConfig;
    // load project config from command line args
    vector<string> args;
    for (int i = 0; i < __argc; ++i)
    {
        wstring ws(__wargv[i]);
        string s;
        s.assign(ws.begin(), ws.end());
        args.push_back(s);
    }

    if (args.size() >= 2)
    {
        if (args.size() && args.at(1).at(0) == '/')
        {
            // FIXME:
            // for Code IDE before RC2
            tmpConfig.setProjectDir(args.at(1));
        }

        tmpConfig.parseCommandLine(args);
    }

    // set project directory as search root path
    //FileUtils::getInstance()->setSearchRootPath(tmpConfig.getProjectDir().c_str());

    // parse config.json
    auto parser = ConfigParser::getInstance();
    auto configPath = tmpConfig.getProjectDir().append(CONFIG_FILE);
    parser->readConfig(configPath);

    // set information
    config.setConsolePort(parser->getConsolePort());
    config.setFileUploadPort(parser->getUploadPort());
    config.setFrameSize(parser->getInitViewSize());
    if (parser->isLanscape())
    {
        config.changeFrameOrientationToLandscape();
    }
    config.setScriptFile(parser->getEntryFile());
}

//
// D:\aaa\bbb\ccc\ddd\abc.txt --> D:/aaa/bbb/ccc/ddd/abc.txt
//
std::string PlayerWin::convertPathFormatToUnixStyle(const std::string& path)
{
    std::string ret = path;
    int len = ret.length();
    for (int i = 0; i < len; ++i)
    {
        if (ret[i] == '\\')
        {
            ret[i] = '/';
        }
    }
    return ret;
}

//
// @return: C:/Users/win8/Documents/
//
std::string PlayerWin::getUserDocumentPath()
{
    TCHAR filePath[MAX_PATH];
    SHGetSpecialFolderPath(NULL, filePath, CSIDL_PERSONAL, FALSE);
    int length = 2 * wcslen(filePath);
    char* tempstring = new char[length + 1];
    wcstombs(tempstring, filePath, length + 1);
    string userDocumentPath(tempstring);
    free(tempstring);

    userDocumentPath = convertPathFormatToUnixStyle(userDocumentPath);
    userDocumentPath.append("/");

    return userDocumentPath;
}

//
// convert Unicode/LocalCode TCHAR to Utf8 char
//
char* PlayerWin::convertTCharToUtf8(const TCHAR* src)
{
#ifdef UNICODE
    WCHAR* tmp = (WCHAR*)src;
    size_t size = wcslen(src) * 3 + 1;
    char* dest = new char[size];
    memset(dest, 0, size);
    WideCharToMultiByte(CP_UTF8, 0, tmp, -1, dest, size, NULL, NULL);
    return dest;
#else
    char* tmp = (char*)src;
    uint32 size = strlen(tmp) + 1;
    WCHAR* dest = new WCHAR[size];
    memset(dest, 0, sizeof(WCHAR)*size);
    MultiByteToWideChar(CP_ACP, 0, src, -1, dest, (int)size); // convert local code to unicode.

    size = wcslen(dest) * 3 + 1;
    char* dest2 = new char[size];
    memset(dest2, 0, size);
    WideCharToMultiByte(CP_UTF8, 0, dest, -1, dest2, size, NULL, NULL); // convert unicode to utf8.
    delete[] dest;
    return dest2;
#endif
}

//
std::string PlayerWin::getApplicationExePath()
{
    TCHAR szFileName[MAX_PATH];
    GetModuleFileName(NULL, szFileName, MAX_PATH);
    std::u16string u16ApplicationName;
    char *applicationExePath = convertTCharToUtf8(szFileName);
    std::string path(applicationExePath);
    CC_SAFE_FREE(applicationExePath);

    return path;
}

std::string PlayerWin::getApplicationPath()
{
    std::string path = getApplicationExePath();
    size_t pos;
    while ((pos = path.find_first_of("\\")) != std::string::npos)
    {
        path.replace(pos, 1, "/");
    }
    size_t p = path.find_last_of("/");
    string workdir;
    if (p != path.npos)
    {
        workdir = path.substr(0, p);
    }

    return workdir;
}

LRESULT CALLBACK PlayerWin::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!_instance) return 0;

    switch (uMsg)
    {
    case WM_COMMAND:
    {
        if (HIWORD(wParam) == 0)
        {
            // menu
            WORD menuId = LOWORD(wParam);
            PlayerMenuItemWin *menuItem = _instance->_menuService->getItemByCommandId(menuId);
            if (menuItem)
            {
                AppEvent event("APP.EVENT", APP_EVENT_MENU);

                std::stringstream buf;
                buf << "{\"data\":\"" << menuItem->getMenuId().c_str() << "\"";
                buf << ",\"name\":" << "\"menuClicked\"" << "}";
                event.setDataString(buf.str());
                event.setUserData(menuItem);
                Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
            }

            if (menuId == ID_HELP_ABOUT)
            {
                onHelpAbout();
            }
        }
        break;
    }
    case WM_KEYDOWN:
    {
        if (wParam == VK_F5)
        {
            PlayerProtocol::getInstance()->relaunch();
        }
        break;
    }

    case WM_COPYDATA:
        {
            PCOPYDATASTRUCT pMyCDS = (PCOPYDATASTRUCT) lParam;
            if (pMyCDS->dwData == 1)
            {
                const char *szBuf = (const char*)(pMyCDS->lpData);
                PlayerWin::getInstance()->writeDebugLog(szBuf);
                break;
            }
        }
    }
    return g_oldWindowProc(hWnd, uMsg, wParam, lParam);
}

PLAYER_NS_END
