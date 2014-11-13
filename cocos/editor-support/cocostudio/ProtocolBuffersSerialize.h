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

namespace tinyxml2
{
    class XMLElement;
}

class CC_STUDIO_DLL ProtocolBuffersSerialize : cocos2d::Ref
{
    
    
public:
    ProtocolBuffersSerialize();
    ~ProtocolBuffersSerialize();
    
    static ProtocolBuffersSerialize* getInstance();
    
    static void purge();
    
    
    /* create protocol buffers from XML */
    protocolbuffers::CSParseBinary* createProtocolBuffersWithXMLFileForSimulator(const std::string& xmlFileName, bool isSimulator = true);
    void convertNodeTreeProtocolBuffersWithXMLForSimulator(protocolbuffers::NodeTree* nodetree,
                                                           const tinyxml2::XMLElement* objectData,
                                                           std::string classType);
    void setProjectNodeOptionsForSimulator(protocolbuffers::ProjectNodeOptions* projectNodeOptions,
                                           protocolbuffers::WidgetOptions* nodeOptions,
                                           const tinyxml2::XMLElement* projectNodeObjectData);
    /**/
    
    /* serialize protocol buffers from XML */
    std::string serializeProtocolBuffersWithXMLFile(const std::string& protocolbuffersFileName, const std::string& xmlFileName, bool isSimulator = false);
    
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
    
	int getResourceType(std::string key);

protected:
	bool _isSimulator;
    
    protocolbuffers::CSParseBinary* _protobuf;
};

#endif /* defined(__ProtobufTest__GUIProtocolBuffers__) */
