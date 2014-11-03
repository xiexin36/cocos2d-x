/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the foll
 owing conditions:
 
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

#ifndef __ProtobufTest__GUIProtocolBuffers__
#define __ProtobufTest__GUIProtocolBuffers__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "json/document.h"
#include "tinyxml2/tinyxml2.h"
#include "cocostudio/CocosStudioExport.h"

namespace protocolbuffers
{
    class CSParseBinary;
    class NodeTree;
    class SpriteOptions;
    class TMXTiledMapOptions;
    class ParticleSystemOptions;
    class ComponentOptions;
    class ComAudioOptions;
    class WidgetOptions;
    class LayoutParameter;
    class ButtonOptions;
    class ResourceData;
    class CheckBoxOptions;
    class ImageViewOptions;
    class TextAtlasOptions;
    class TextBMFontOptions;
    class TextOptions;
    class LoadingBarOptions;
    class ListViewOptions;
    class PageViewOptions;
    class PanelOptions;
    class ScrollViewOptions;
    class SliderOptions;
    class TextFieldOptions;
    class ProjectNodeOptions;
    
    class NodeAction;
    class TimeLine;
    class Frame;
    class TimeLineBoolFrame;
    class TimeLineIntFrame;
    class TimeLineStringFrame;
    class TimeLinePointFrame;
    class TimeLineColorFrame;
    class TimeLineTextureFrame;
}

namespace cocos2d
{
    namespace ui
    {
        class Widget;
    }
}

namespace cocostudio
{
    namespace timeline
    {
        class ActionTimeline;
        class Timeline;
        class Frame;
        class VisibleFrame;
        class PositionFrame;
        class ScaleFrame;
        class RotationSkewFrame;
        class AnchorPointFrame;
        class ColorFrame;
        class TextureFrame;
        class EventFrame;
        class ZOrderFrame;
    }
}

class CC_STUDIO_DLL EtcProperties : cocos2d::Ref
{
public:
    EtcProperties();
    ~EtcProperties();
    
    static EtcProperties* getInstance();
    
    static void purge();
    
    void setVersion(const std::string& version) { _version = version; };
    const std::string& getVersion() const { return _version; };
    
    void setCocos2dVersion(const std::string& version) { _cocos2dVersion = version; };
    const std::string& getCocos2dVersion() const { return _cocos2dVersion; };
    
    void setEditorType(const std::string& type) { _editorType = type; };
    const std::string& getEditorType() const { return _editorType; };
    
    void setDesignWidth(int width) { _designWidth = width; };
    const int getDesignWidth() const { return _designWidth; };
    
    void setDesignHeight(int height) { _designHeight = height; };
    const int getDesignHeight() const { return _designHeight; };
    
    void setDataScale(int scale) { _dataScale = scale; };
    const int getDataScale() const { return _dataScale; };
    
    void addTexture(const std::string& texture);
    const std::vector<std::string>& getTextures() const { return _textures; };
    
    void addTexturePng(const std::string& png);
    const std::vector<std::string>& getTexturePngs() const { return _texturePngs; };
    
protected:
    std::string _version;
    std::string _cocos2dVersion;
    std::string _editorType;
    int _designWidth;
    int _designHeight;
    int _dataScale;
    std::vector<std::string> _textures;
    std::vector<std::string> _texturePngs;
};

class CC_STUDIO_DLL ProtocolBuffersSerialize : cocos2d::Ref
{
    
    
public:
    ProtocolBuffersSerialize();
    ~ProtocolBuffersSerialize();
    
    static ProtocolBuffersSerialize* getInstance();
    
    static void purge();
    
    /* serialize protocol buffers from XML */
    void XMLTest(const std::string& fileName);
    std::string serializeProtocolBuffersWithXMLFile(const std::string& protocolbuffersFileName, const std::string& xmlFileName, bool isSimulator = false);
//    void serializeProtocolBuffersWithXMLFile(const std::string& protocolbuffersFileName, const std::string& xmlFileName);
    
    // nodeTree
    void convertNodeTreeProtocolBuffersWithXML(protocolbuffers::NodeTree* nodetree,
                                               const tinyxml2::XMLElement* objectData,
                                               std::string classType);
    
    void setNodeOptions(protocolbuffers::WidgetOptions* nodeOptions,
                        const tinyxml2::XMLElement* nodeObjectData);
    void setSingleNodeOptions(protocolbuffers::WidgetOptions* nodeOptions,
                              const tinyxml2::XMLElement* nodeObjectData);
    void setSpriteOptions(protocolbuffers::SpriteOptions* spriteOptions,
                          protocolbuffers::WidgetOptions* nodeOptions,
                          const tinyxml2::XMLElement* spriteObjectData);
    void setTMXTiledMapOptions(protocolbuffers::TMXTiledMapOptions* tmxTiledMapOptions,
                               protocolbuffers::WidgetOptions* nodeOptions,
                               const tinyxml2::XMLElement* tmxTiledMapObjectData);
    void setParticleSystemOptions(protocolbuffers::ParticleSystemOptions* particleSystemOptions,
                                  protocolbuffers::WidgetOptions* nodeOptions,
                                  const tinyxml2::XMLElement* particleSystemObjectData);
    void setWidgetOptions(protocolbuffers::WidgetOptions* widgetOptions,
                          const tinyxml2::XMLElement* widgetObjectData);
    void setButtonOptions(protocolbuffers::ButtonOptions* buttonOptions,
                          protocolbuffers::WidgetOptions* widgetOptions,
                          const tinyxml2::XMLElement* buttonObjectData);
    void setCheckBoxOptions(protocolbuffers::CheckBoxOptions* checkBoxOptions,
                            protocolbuffers::WidgetOptions* widgetOptions,
                            const tinyxml2::XMLElement* checkBoxObjectData);
    void setImageViewOptions(protocolbuffers::ImageViewOptions* imageViewOptions,
                             protocolbuffers::WidgetOptions* widgetOptions,
                             const tinyxml2::XMLElement* imageViewObjectData);
    void setTextAtlasOptions(protocolbuffers::TextAtlasOptions* textAtlasOptions,
                             protocolbuffers::WidgetOptions* widgetOptions,
                             const tinyxml2::XMLElement* textAtlasObjectData);
    void setTextBMFontOptions(protocolbuffers::TextBMFontOptions* textBMFontOptions,
                              protocolbuffers::WidgetOptions* widgetOptions,
                              const tinyxml2::XMLElement* textBMFontObjectData);
    void setTextOptions(protocolbuffers::TextOptions* textOptions,
                        protocolbuffers::WidgetOptions* widgetOptions,
                        const tinyxml2::XMLElement* textObjectData);
    void setLoadingBarOptions(protocolbuffers::LoadingBarOptions* loadingBarOptions,
                              protocolbuffers::WidgetOptions* widgetOptions,
                              const tinyxml2::XMLElement* loadingObjectData);
    void setSliderOptions(protocolbuffers::SliderOptions* sliderOptions,
                          protocolbuffers::WidgetOptions* widgetOptions,
                          const tinyxml2::XMLElement* sliderObjectData);
    void setTextFieldOptions(protocolbuffers::TextFieldOptions* textFieldOptions,
                             protocolbuffers::WidgetOptions* widgetOptions,
                             const tinyxml2::XMLElement* textFieldObjectData);
    void setLayoutOptions(protocolbuffers::PanelOptions* layoutOptions,
                          protocolbuffers::WidgetOptions* widgetOptions,
                          const tinyxml2::XMLElement* layoutObjectData);
    void setPageViewOptions(protocolbuffers::PageViewOptions* pageViewOptions,
                            protocolbuffers::WidgetOptions* widgetOptions,
                            const tinyxml2::XMLElement* pageViewObjectData);
    void setScrollViewOptions(protocolbuffers::ScrollViewOptions* scrollViewOptions,
                              protocolbuffers::WidgetOptions* widgetOptions,
                              const tinyxml2::XMLElement* scrollViewObjectData);
    void setListViewOptions(protocolbuffers::ListViewOptions* listViewOptions,
                            protocolbuffers::WidgetOptions* widgetOptions,
                            const tinyxml2::XMLElement* listViewObjectData);
    void setProjectNodeOptions(protocolbuffers::ProjectNodeOptions* projectNodeOptions,
                               protocolbuffers::WidgetOptions* nodeOptions,
                               const tinyxml2::XMLElement* projectNodeObjectData);
    void setSimpleAudioOptions(protocolbuffers::WidgetOptions* nodeOptions,
                               const tinyxml2::XMLElement* nodeObjectData);
    
    // component
    void setComAudioOptions(protocolbuffers::ComAudioOptions* comAudioOptions,
                            const tinyxml2::XMLElement* audioObjectData);
    
    // action
    void convertActionProtocolBuffersWithXML(protocolbuffers::NodeAction* nodeAction,
                                             const tinyxml2::XMLElement* animation);
    void convertTimelineProtocolBuffers(protocolbuffers::TimeLine* timeLine,
                                        const tinyxml2::XMLElement* timelineElement);
    void setVisibleFrame(protocolbuffers::TimeLineBoolFrame* visibleFrame,
                         const tinyxml2::XMLElement* frameElement);
    void setPositionFrame(protocolbuffers::TimeLinePointFrame* positionFrame,
                          const tinyxml2::XMLElement* frameElement);
    void setScaleFrame(protocolbuffers::TimeLinePointFrame* scaleFrame,
                       const tinyxml2::XMLElement* frameElement);
    void setRotationSkewFrame(protocolbuffers::TimeLinePointFrame* rotationSkewFrame,
                              const tinyxml2::XMLElement* frameElement);
    void setAnchorFrame(protocolbuffers::TimeLinePointFrame* anchorPointframe,
                        const tinyxml2::XMLElement* frameElement);
    void setColorFrame(protocolbuffers::TimeLineColorFrame* colorFrame,
                       const tinyxml2::XMLElement* frameElement);
    void setTextureFrame(protocolbuffers::TimeLineTextureFrame* textureFrame,
                         const tinyxml2::XMLElement* frameElement);
    void setEventFrame(protocolbuffers::TimeLineStringFrame* eventFrame,
                       const tinyxml2::XMLElement* frameElement);
    void setZOrderFrame(protocolbuffers::TimeLineIntFrame* zorderFrame,
                        const tinyxml2::XMLElement* frameElement);
    /**/
    
    /* peterson json */
    /* serialize protocol buffers from json */
    void serializeProtocolBuffersWithJson(const std::string& protocolbuffersFileName,
                                                 const std::string& jsonFileName);
    // nodeTree
    void convertNodeTreeProtocolBuffersWithJson(protocolbuffers::NodeTree* nodetree,
                                               const rapidjson::Value& json);
    void setNodeOptions(protocolbuffers::WidgetOptions* nodeOptions,
                        const rapidjson::Value& optionsJson);
    void setSpriteOptions(protocolbuffers::SpriteOptions* spriteOptions,
                          protocolbuffers::WidgetOptions* nodeOptions,
                          const rapidjson::Value& optionsJson);
    void setWidgetOptions(protocolbuffers::WidgetOptions* widgetOptions,
                          const rapidjson::Value& optionsJson);
    void setButtonOptions(protocolbuffers::ButtonOptions* buttonOptions,
                          protocolbuffers::WidgetOptions* widgetOptions,
                          const rapidjson::Value& optionsJson);
    void setCheckBoxOptions(protocolbuffers::CheckBoxOptions* checkBoxOptions,
                            protocolbuffers::WidgetOptions* widgetOptions,
                            const rapidjson::Value& optionsJson);
    void setImageViewOptions(protocolbuffers::ImageViewOptions* imageViewOptions,
                             protocolbuffers::WidgetOptions* widgetOptions,
                             const rapidjson::Value& optionsJson);
    void setTextAtlasOptions(protocolbuffers::TextAtlasOptions* textAtlasOptions,
                             protocolbuffers::WidgetOptions* widgetOptions,
                             const rapidjson::Value& optionsJson);
    void setTextBMFontOptions(protocolbuffers::TextBMFontOptions* textBMFontOptions,
                              protocolbuffers::WidgetOptions* widgetOptions,
                              const rapidjson::Value& optionsJson);
    void setTextOptions(protocolbuffers::TextOptions* textOptions,
                        protocolbuffers::WidgetOptions* widgetOptions,
                        const rapidjson::Value& optionsJson);
    void setLoadingBarOptions(protocolbuffers::LoadingBarOptions* loadingBarOptions,
                              protocolbuffers::WidgetOptions* widgetOptions,
                              const rapidjson::Value& optionsJson);
    void setSliderOptions(protocolbuffers::SliderOptions* sliderOptions,
                          protocolbuffers::WidgetOptions* widgetOptions,
                          const rapidjson::Value& optionsJson);
    void setTextFieldOptions(protocolbuffers::TextFieldOptions* textFieldOptions,
                             protocolbuffers::WidgetOptions* widgetOptions,
                             const rapidjson::Value& optionsJson);
    void setLayoutOptions(protocolbuffers::PanelOptions* layoutOptions,
                          protocolbuffers::WidgetOptions* widgetOptions,
                          const rapidjson::Value& optionsJson);
    void setPageViewOptions(protocolbuffers::PageViewOptions* pageViewOptions,
                            protocolbuffers::WidgetOptions* widgetOptions,
                            const rapidjson::Value& optionsJson);
    void setScrollViewOptions(protocolbuffers::ScrollViewOptions* scrollViewOptions,
                              protocolbuffers::WidgetOptions* widgetOptions,
                              const rapidjson::Value& optionsJson);
    void setListViewOptions(protocolbuffers::ListViewOptions* listViewOptions,
                            protocolbuffers::WidgetOptions* widgetOptions,
                            const rapidjson::Value& optionsJson);
    /**/
    
    /* action */
    void convertActionProtocolBuffersWithJson(protocolbuffers::NodeAction* nodeAction,
                                              const rapidjson::Value& json);
    void convertTimelineProtocolBuffers(protocolbuffers::TimeLine* timeLine,
                                        const rapidjson::Value& json);
    void setVisibleFrame(protocolbuffers::TimeLineBoolFrame* visibleFrame,
                         const rapidjson::Value& json);
    void setPositionFrame(protocolbuffers::TimeLinePointFrame* positionFrame,
                          const rapidjson::Value& json);
    void setScaleFrame(protocolbuffers::TimeLinePointFrame* scaleFrame,
                       const rapidjson::Value& json);
    void setRotationSkewFrame(protocolbuffers::TimeLinePointFrame* rotationSkewFrame,
                              const rapidjson::Value& json);
    void setAnchorFrame(protocolbuffers::TimeLinePointFrame* anchorPointframe,
                        const rapidjson::Value& json);
    void setColorFrame(protocolbuffers::TimeLineColorFrame* colorFrame,
                       const rapidjson::Value& json);
    void setTextureFrame(protocolbuffers::TimeLineTextureFrame* textureFrame,
                         const rapidjson::Value& json);
    void setEventFrame(protocolbuffers::TimeLineStringFrame* eventFrame,
                       const rapidjson::Value& json);
    void setZOrderFrame(protocolbuffers::TimeLineIntFrame* zorderFrame,
                        const rapidjson::Value& json);
    /**/
    /**/
    
    /* serialize protocol buffers from Node */
//    void serializeProtocolBuffersWithFile(const std::string& fileName);
    
    void serializeProtocolBuffersFileWithNode(const std::string& outFileName,
                                              cocos2d::Node* node,
                                              cocostudio::timeline::ActionTimeline* action,
                                              const EtcProperties* etc);
    
    // NodeTree
    void convertNodeTreeProtocolBuffersFromNode(protocolbuffers::NodeTree* nodetree,
                                                cocos2d::Node* node,
                                                std::string classname);
    bool isUIWidget(std::string classname);
    
    void setNodeOptions(protocolbuffers::WidgetOptions* nodeOptions, cocos2d::Node* node);
    void setSpriteOptions(protocolbuffers::SpriteOptions* spriteOptions,
                          protocolbuffers::WidgetOptions* nodeOptions,
                          cocos2d::Node* node);
    void setTMXTiledMapOptions(protocolbuffers::TMXTiledMapOptions* tmxTiledMapOptions,
                               protocolbuffers::WidgetOptions* nodeOptions,
                               cocos2d::Node* node);
    void setParticleSystemOptions(protocolbuffers::ParticleSystemOptions* particleSystemOptions,
                                  protocolbuffers::WidgetOptions* nodeOptions,
                                  cocos2d::Node* node);
    void setWidgetOptions(protocolbuffers::WidgetOptions* widgetOptions, cocos2d::ui::Widget* widget);
    void setButtonOptions(protocolbuffers::ButtonOptions* buttonOptions,
                          protocolbuffers::WidgetOptions* widgetOptions,
                          cocos2d::ui::Widget* widget);
    void setCheckBoxOptions(protocolbuffers::CheckBoxOptions* checkBoxOptions,
                            protocolbuffers::WidgetOptions* widgetOptions,
                            cocos2d::ui::Widget* widget);
    void setImageViewOptions(protocolbuffers::ImageViewOptions* imageViewOptions,
                             protocolbuffers::WidgetOptions* widgetOptions,
                             cocos2d::ui::Widget* widget);
    void setLabelAtlasOptions(protocolbuffers::TextAtlasOptions* labelAtlasOptions,
                             protocolbuffers::WidgetOptions* widgetOptions,
                             cocos2d::ui::Widget* widget);
    void setLabelBMFontOptions(protocolbuffers::TextBMFontOptions* labelBMFontOptions,
                               protocolbuffers::WidgetOptions* widgetOptions,
                               cocos2d::ui::Widget* widget);
    void setLabelOptions(protocolbuffers::TextOptions* labelOptions,
                         protocolbuffers::WidgetOptions* widgetOptions,
                         cocos2d::ui::Widget* widget);
    void setLoadingBarOptions(protocolbuffers::LoadingBarOptions* loadingBarOptions,
                              protocolbuffers::WidgetOptions* widgetOptions,
                              cocos2d::ui::Widget* widget);
    void setSliderOptions(protocolbuffers::SliderOptions* sliderOptions,
                          protocolbuffers::WidgetOptions* widgetOptions,
                          cocos2d::ui::Widget* widget);
    void setTextFieldOptions(protocolbuffers::TextFieldOptions* textFieldOptions,
                             protocolbuffers::WidgetOptions* widgetOptions,
                             cocos2d::ui::Widget* widget);
    void setLayoutOptions(protocolbuffers::PanelOptions* layoutOptions,
                          protocolbuffers::WidgetOptions* widgetOptions,
                          cocos2d::ui::Widget* widget);
    void setPageViewOptions(protocolbuffers::PageViewOptions* pageViewOptions,
                            protocolbuffers::WidgetOptions* widgetOptions,
                            cocos2d::ui::Widget* widget);
    void setScrollViewOptions(protocolbuffers::ScrollViewOptions* scrollViewOptions,
                              protocolbuffers::WidgetOptions* widgetOptions,
                              cocos2d::ui::Widget* widget);
    void setListViewOptions(protocolbuffers::ListViewOptions* listViewOptions,
                            protocolbuffers::WidgetOptions* widgetOptions,
                            cocos2d::ui::Widget* widget);
    
    void convertComponentProtocolBuffers(protocolbuffers::ComponentOptions* componentOptions,
                                         cocos2d::Component* component);
    void setComAudioOptions(protocolbuffers::ComAudioOptions* comAudioOptions,
                            cocos2d::Component* component);
    /**/
    
    /* Action */
    void convertActionProtocolBuffersWithNode(protocolbuffers::NodeAction* nodeAction,
                                              cocostudio::timeline::ActionTimeline* action);
    void convertTimelineProtocolBuffers(protocolbuffers::TimeLine* protolBuftimeline,
                                        cocostudio::timeline::Timeline* timeline);
    void setVisibleFrame(protocolbuffers::TimeLineBoolFrame* protoBufFrame,
                         cocostudio::timeline::Frame* frame);
    void setPositionFrame(protocolbuffers::TimeLinePointFrame* protoBufFrame,
                          cocostudio::timeline::Frame* frame);
    void setScaleFrame(protocolbuffers::TimeLinePointFrame* protoBufFrame,
                       cocostudio::timeline::Frame* frame);
    void setRotationSkewFrame(protocolbuffers::TimeLinePointFrame* protoBufFrame,
                              cocostudio::timeline::Frame* frame);
    void setAnchorFrame(protocolbuffers::TimeLinePointFrame* protoBufFrame,
                        cocostudio::timeline::Frame* frame);
    void setColorFrame(protocolbuffers::TimeLineColorFrame* protoBufFrame,
                       cocostudio::timeline::Frame* frame);
    void setTextureFrame(protocolbuffers::TimeLineTextureFrame* protoBufFrame,
                         cocostudio::timeline::Frame* frame);
    void setEventFrame(protocolbuffers::TimeLineStringFrame* protoBufFrame,
                       cocostudio::timeline::Frame* frame);
    void setZOrderFrame(protocolbuffers::TimeLineIntFrame* protoBufFrame,
                        cocostudio::timeline::Frame* frame);
    /**/
    
    /* Scene */
    /**/
    
	int getResourceType(std::string key);

protected:
	bool _isSimulator;
    
    protocolbuffers::CSParseBinary* _protobuf;
};

#endif /* defined(__ProtobufTest__GUIProtocolBuffers__) */
