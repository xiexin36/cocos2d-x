//
//  MttGameEngine.mm
//  CocosJSRuntime
//
//  Created by WenhaiLin on 15/10/19.
//
#import "MttGameEngine.h"

#include "ScriptingCore.h"
#import "cocos2d.h"
#import "CocosDelegate.h"
#import "platform/ios/CCEAGLView-ios.h"
#include "audio/include/SimpleAudioEngine.h"
#include "audio/include/AudioEngine.h"

#import "CocosRuntime.h"
#import "ChannelConfig.h"
#import "CocosRuntimeGroup.h"
#import "FileUtil.h"
#import "LoadingAdapter4Tencent.h"

#import "Wrapper.h"
#import "NetworkController.h"

using namespace CocosDenshion;

static CocosAppDelegate* s_application = nullptr;
static id s_gameEngineProtocol;

@interface CocosRuntimeBridge : NSObject<CocosRuntimeSDKDelegate>

@end

@implementation CocosRuntimeBridge

- (NSMutableArray*)getSupportForPlugins
{
    NSMutableArray* plugins = [NSMutableArray array];
    [plugins addObject:@"UserQQBrowser"];
    [plugins addObject:@"IAPQQBrowser"];
    [plugins addObject:@"SocialQQBrowser"];
    [plugins addObject:@"ShareQQBrowser"];
    
    return plugins;
}

- (NSMutableDictionary*)getPluginParams
{
    NSMutableDictionary* pluginParams = [NSMutableDictionary dictionary];
    
    return pluginParams;
}

@end

@interface MttGameEngine()

@property (nonatomic, weak) id<MttGameEngineDelegate> x5Delegate;

@end

@implementation MttGameEngine

//初始化游戏引擎
- (void)game_engine_init:(NSString*)gameInfoJson
{
    if (self.x5Delegate == nil) {
        return;
    }
    
    if (s_application == nullptr) {
        s_application = new (std::nothrow) CocosAppDelegate;
    }
    
    [ChannelConfig setChannelID:@"100115"];
    
    // 从浏览器配置的CacheDir、LibDir，获取失败直接返回
    NSString* cacheDir = [self.x5Delegate x5GamePlayer_get_value:@"CacheDir"];
    NSString* libDir = [self.x5Delegate x5GamePlayer_get_value:@"LibDir"];
    if (cacheDir != nil && libDir != nil) {
        [ChannelConfig setCocosRuntimeRootPath:cacheDir];
        s_application->setEngineResDir([libDir cStringUsingEncoding:NSUTF8StringEncoding]);
    }
    else {
        NSString* errorMsg = [NSString stringWithFormat:@"game_engine_init:get value failed! CacheDir:%@, LibDir:%@", cacheDir, libDir];
        [self.x5Delegate x5GamePlayer_send_msg:
         [NSDictionary dictionaryWithObjectsAndKeys:errorMsg, @"error", nil]];
        return;
    }
    
    //解析game info取得game key
    NSString* gameKey;
    if (gameInfoJson != nil) {
        bool gameInfoInitFlag = false;
        do {
            NSDictionary * gameInfo = [NSJSONSerialization JSONObjectWithData:[gameInfoJson dataUsingEncoding:NSUTF8StringEncoding] options:NSJSONReadingMutableContainers error:nil];
            if (gameInfo == nil) { break;}
            NSNumber* isDebugMode = [gameInfo objectForKey:@"localDebug"];
            if (isDebugMode && [isDebugMode intValue] == 1) {
                [NetworkController setRTServerAddress:RTServerAddress_STAGING];
            }
            
            NSString* gameExt = [gameInfo objectForKey:@"ext"];
            if (gameExt == nil) { break;}
            
            NSDictionary* gameExtInfo = [NSJSONSerialization JSONObjectWithData:[gameExt dataUsingEncoding:NSUTF8StringEncoding] options:NSJSONReadingMutableContainers error:nil];
            if (gameExtInfo == nil) { break;}
            
            gameKey = [gameExtInfo objectForKey:@"gameKey"];
            gameInfoInitFlag = true;
            
            //NSString *channel = [gameExtInfo objectForKey:@"channel"];
            //if (channel == nil) { break;}
        } while (false);
        
        if (gameInfoInitFlag == false) {
            NSString* errorMsg = [NSString stringWithFormat:@"game_engine_init:get game info fail!gameKey:%@", gameKey];
            
            [self.x5Delegate x5GamePlayer_send_msg:
             [NSDictionary dictionaryWithObjectsAndKeys:MSG_ON_RUNTIME_CHECK_FAIL,@"type",errorMsg,@"error", nil]];
            return;
        }
    }
    else {
        gameKey = [self.x5Delegate x5GamePlayer_get_value:@"gameKey"];
        [NetworkController setRTServerAddress:RTServerAddress_STAGING];
    }
    
    [Wrapper setCocosRuntimeSDKVersionCode:1];
    [Wrapper setCocosRuntimeSDKProxy:[[CocosRuntimeBridge alloc] init]];
    
    //从服务器获取游戏配置并下载第一个boot分组
    [CocosRuntime startPreRuntime:gameKey delegate:[[LoadingAdapter4Tencent alloc] initWith:^(int progress, bool isFailed) {
        if (isFailed) {
            [self.x5Delegate x5GamePlayer_send_msg:
             [NSDictionary dictionaryWithObjectsAndKeys:MSG_ON_NETWORK_ERR,@"type", nil]];
        } else {
            long fixProgress = (long)progress;
            fixProgress = fixProgress > 100 ? 100 : fixProgress;
            NSString* progressText = [NSString stringWithFormat:@"%ld",fixProgress];
            [self.x5Delegate x5GamePlayer_send_msg:
             [NSDictionary dictionaryWithObjectsAndKeys:MSG_ON_GAME_LOADING_PROGRESS,@"type",
              progressText, @"progress", @"102400", @"size", nil]];
            
            if (fixProgress >= 100) {
                [self.x5Delegate x5GamePlayer_send_msg:
                 [NSDictionary dictionaryWithObjectsAndKeys:MSG_ON_LOAD_GAME_END,@"type", nil]];
            }
        }
    }]];
    
    //通知浏览器已经开始下载
    [self.x5Delegate x5GamePlayer_send_msg:
    [NSDictionary dictionaryWithObjectsAndKeys:MSG_ON_LOAD_GAME_START,@"type", nil]];
    
    auto gameResRoot = [[FileUtil getGameRootPathByGameKey:gameKey] cStringUsingEncoding:NSUTF8StringEncoding];
    s_application->setGameResRoot(gameResRoot);
}

//得到用于显示的view
- (UIView*)game_engine_get_view;
{
    /*
    if ([orientationConfig  isEqual: @"d"]) {
        NSNumber *value = [NSNumber numberWithInt:UIInterfaceOrientationLandscapeLeft];
        [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
    } else {
        NSNumber *value = [NSNumber numberWithInt:UIInterfaceOrientationPortrait];
        [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
    }*/
    
    auto orientation = [UIApplication sharedApplication].statusBarOrientation;
    auto screenSize = [UIScreen mainScreen].bounds.size;
    
    if (UIInterfaceOrientationIsLandscape(orientation)) {
        CCLOG("Interface orientation is landscape");
    } else {
        CCLOG("Interface orientation is portrait");
    }
    
    CCEAGLView *eaglView = [CCEAGLView viewWithFrame: CGRectMake(0, 0, screenSize.width, screenSize.height)
                                         pixelFormat: kEAGLColorFormatRGBA8
                                         depthFormat: GL_DEPTH24_STENCIL8_OES
                                  preserveBackbuffer: NO
                                          sharegroup: nil
                                       multiSampling: NO
                                     numberOfSamples: 0];
    [eaglView setMultipleTouchEnabled:YES];
    
    auto glview = cocos2d::GLViewImpl::createWithEAGLView((__bridge void*)eaglView);
    if(glview != nullptr)
    {
        cocos2d::Director::getInstance()->setOpenGLView(glview);
        cocos2d::Application::getInstance()->run();
    }
    
    return eaglView;
}

//暂停游戏
- (void)game_engine_onPause
{
    CCLOG("game_engine_onPause");
    cocos2d::Application::getInstance()->applicationDidEnterBackground();
}

//恢复游戏
- (void)game_engine_onResume
{
    CCLOG("game_engine_onResume");
    cocos2d::Application::getInstance()->applicationWillEnterForeground();
}

//退出游戏
- (void)game_engine_onStop
{
    CCLOG("game_engine_onStop");
    
    cocos2d::Director::getInstance()->end();
    
    SimpleAudioEngine::getInstance()->stopAllEffects();
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    
    s_application = nullptr;
    s_gameEngineProtocol = nil;
    
    [CocosRuntime reset];
    [Wrapper setCocosRuntimeSDKProxy:nil];
}

//设置GameEngineRuntimeProxy对象
- (void)game_engine_set_runtime_proxy:(id<MttGameEngineDelegate>)proxy
{
    s_gameEngineProtocol = nil;
    s_gameEngineProtocol = self;
    self.x5Delegate = proxy;
    
    if (proxy == nil) {
        CCLOG("%s error, proxy is nil", __FUNCTION__);
    }
}

//调用某个方法， method为方法名， bundle存有方法所用的参数
- (void)game_engine_invoke_method:(NSString*)method bundle:(NSDictionary*)bundle
{
}

- (id<MttGameEngineDelegate>)getX5Delegate
{
    return self.x5Delegate;
}

+ (id<MttGameEngineDelegate>)getEngineDelegate
{
    return [s_gameEngineProtocol getX5Delegate];
}

//获取游戏引擎key所对应的的值
- (id)game_engine_get_value:(NSString*)key
{
    return nil;
}

//x5通过这个接口发送消息给game engine
- (void)game_engine_send_msg:(NSDictionary*)jsonObj
{
}

@end