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

#include "ProtocolBuffersSerialize.h"
#include "../ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "base/CCMap.h"

#include "tinyxml2/tinyxml2.h"

#include "cocostudio/CSParseBinary.pb.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace cocostudio;
using namespace cocostudio::timeline;
using namespace protocolbuffers;


static const char* FrameType_VisibleFrame       = "VisibleFrame";
static const char* FrameType_PositionFrame      = "PositionFrame";
static const char* FrameType_ScaleFrame         = "ScaleFrame";
static const char* FrameType_RotationSkewFrame  = "RotationSkewFrame";
static const char* FrameType_AnchorFrame        = "AnchorPointFrame";
static const char* FrameType_ColorFrame         = "ColorFrame";
static const char* FrameType_TextureFrame       = "TextureFrame";
static const char* FrameType_EventFrame         = "EventFrame";
static const char* FrameType_ZOrderFrame        = "ZOrderFrame";


static ProtocolBuffersSerialize* sharedProtocolBuffersSerialize = nullptr;

ProtocolBuffersSerialize::ProtocolBuffersSerialize()
: _isSimulator(false)
, _protobuf(nullptr)
{
    
}

ProtocolBuffersSerialize::~ProtocolBuffersSerialize()
{
    
}

ProtocolBuffersSerialize* ProtocolBuffersSerialize::getInstance()
{
    if (!sharedProtocolBuffersSerialize)
    {
        sharedProtocolBuffersSerialize = new ProtocolBuffersSerialize();
    }
    return sharedProtocolBuffersSerialize;
}

void ProtocolBuffersSerialize::purge()
{
    CC_SAFE_DELETE(sharedProtocolBuffersSerialize);
}

/* create protocol buffers from XML */
CSParseBinary* ProtocolBuffersSerialize::createProtocolBuffersWithXMLFileForSimulator(const std::string &xmlFileName,
                                                                          bool isSimulator/* = true*/)
{
    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(xmlFileName).c_str();
    
    // xml read
    if (!FileUtils::getInstance()->isFileExist(fullpath))
    {
//        CCLOG("file doesn not exists ");
        return nullptr;
    }
    
    ssize_t size;
    std::string content =(char*)FileUtils::getInstance()->getFileData(fullpath, "r", &size);
    
    // xml parse
    tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
    document->Parse(content.c_str());
    
    const tinyxml2::XMLElement* rootElement = document->RootElement();// Root
//    CCLOG("rootElement name = %s", rootElement->Name());
    
    const tinyxml2::XMLElement* element = rootElement->FirstChildElement();
    
    bool serializeEnabled = false;
    std::string rootType = "";
    
    _isSimulator = isSimulator;
    
    while (element)
    {
//        CCLOG("entity name = %s", element->Name());
        
        if (strcmp("Content", element->Name()) == 0)
        {
            const tinyxml2::XMLAttribute* attribute = element->FirstAttribute();
            
            // cheat
            if (!attribute)
            {
                serializeEnabled = true;
                rootType = "NodeObjectData";
            }
            //
            
            //
            //            while (attribute)
            //            {
            //                std::string name = attribute->Name();
            //                std::string value = attribute->Value();
            //                CCLOG("attribute name = %s, value = %s", name, value);
            //                if (name == "")
            //                {
            //                    serializeEnabled = true;
            //                    rootType = (strcmp("", value) == 0) ? "Node" : value;
            //                }
            //
            //                if (serializeEnabled)
            //                {
            //                    break;
            //                }
            //
            //                attribute = attribute->Next();
            //            }
            //
        }
        
        if (serializeEnabled)
        {
            break;
        }
        
        const tinyxml2::XMLElement* child = element->FirstChildElement();
        if (child)
        {
            element = child;
        }
        else
        {
            element = element->NextSiblingElement();
        }
    }
    
    
    // serialize
    if (serializeEnabled)
    {
        CSParseBinary* protobufPtf = new CSParseBinary;
        _protobuf = protobufPtf;
        
        const tinyxml2::XMLElement* child = element->FirstChildElement();
        
        while (child)
        {
            std::string name = child->Name();
            
            if (name == "Animation") // action
            {
                protocolbuffers::NodeAction* nodeAction = protobufPtf->mutable_action();
                const tinyxml2::XMLElement* animation = child;
                convertActionProtocolBuffersWithXML(nodeAction, animation);
            }
            else if (name == "ObjectData") // nodeTree
            {
                protocolbuffers::NodeTree* nodeTreeRoot = protobufPtf->mutable_nodetree();
                const tinyxml2::XMLElement* objectData = child;
                convertNodeTreeProtocolBuffersWithXMLForSimulator(nodeTreeRoot, objectData, rootType);
            }
            
            child = child->NextSiblingElement();
        }
        
        return protobufPtf;        
    }
    
    return nullptr;
}

void ProtocolBuffersSerialize::convertNodeTreeProtocolBuffersWithXMLForSimulator(protocolbuffers::NodeTree *nodetree,
                                                                                 const tinyxml2::XMLElement *objectData,
                                                                                 std::string classType)
{
    std::string classname = classType.substr(0, classType.find("ObjectData"));
//    CCLOG("classname = %s", classname.c_str());
    
    nodetree->set_classname(classname);
    
    if (classname == "Node")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        setNodeOptions(nodeOptions, objectData);
    }
    else if (classname == "SingleNode")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        setSingleNodeOptions(nodeOptions, objectData);
    }
    else if (classname == "Sprite")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        SpriteOptions* options = nodetree->mutable_spriteoptions();
        setSpriteOptions(options, nodeOptions, objectData);
    }
    else if (classname == "GameMap")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        TMXTiledMapOptions* options = nodetree->mutable_tmxtiledmapoptions();
        setTMXTiledMapOptions(options, nodeOptions, objectData);
    }
    else if (classname == "Particle")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        ParticleSystemOptions* options = nodetree->mutable_particlesystemoptions();
        setParticleSystemOptions(options, nodeOptions, objectData);
    }
    else if (classname == "Button")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        ButtonOptions* options = nodetree->mutable_buttonoptions();
        setButtonOptions(options, widgetOptions, objectData);
    }
    else if (classname == "CheckBox")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        CheckBoxOptions* options = nodetree->mutable_checkboxoptions();
        setCheckBoxOptions(options, widgetOptions, objectData);
    }
    else if (classname == "ImageView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        ImageViewOptions* options = nodetree->mutable_imageviewoptions();
        setImageViewOptions(options, widgetOptions, objectData);
    }
    else if (classname == "TextAtlas")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        TextAtlasOptions* options = nodetree->mutable_textatlasoptions();
        setTextAtlasOptions(options, widgetOptions, objectData);
    }
    else if (classname == "TextBMFont")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        TextBMFontOptions* options = nodetree->mutable_textbmfontoptions();
        setTextBMFontOptions(options, widgetOptions, objectData);
    }
    else if (classname == "Text")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        TextOptions* options = nodetree->mutable_textoptions();
        setTextOptions(options, widgetOptions, objectData);
    }
    else if (classname == "LoadingBar")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        LoadingBarOptions* options = nodetree->mutable_loadingbaroptions();
        setLoadingBarOptions(options, widgetOptions, objectData);
    }
    else if (classname == "Slider")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        SliderOptions* options = nodetree->mutable_slideroptions();
        setSliderOptions(options, widgetOptions, objectData);
    }
    else if (classname == "TextField")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        TextFieldOptions* options = nodetree->mutable_textfieldoptions();
        setTextFieldOptions(options, widgetOptions, objectData);
    }
    else if (classname == "Panel")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        PanelOptions* layoutOptions = nodetree->mutable_paneloptions();
        setLayoutOptions(layoutOptions, widgetOptions, objectData);
    }
    else if (classname == "PageView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        PageViewOptions* options = nodetree->mutable_pageviewoptions();
        setPageViewOptions(options, widgetOptions, objectData);
    }
    else if (classname == "ScrollView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        ScrollViewOptions* options = nodetree->mutable_scrollviewoptions();
        setScrollViewOptions(options, widgetOptions, objectData);
    }
    else if (classname == "ListView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        ListViewOptions* options = nodetree->mutable_listviewoptions();
        setListViewOptions(options, widgetOptions, objectData);
    }
    else if (classname == "ProjectNode")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        ProjectNodeOptions* options = nodetree->mutable_projectnodeoptions();
        setProjectNodeOptionsForSimulator(options, nodeOptions, objectData);
    }
    else if (classname == "SimpleAudio")
    {
        // process as component options
        
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        setSimpleAudioOptions(nodeOptions, objectData);
        
        protocolbuffers::ComponentOptions* componentOptions = nodeOptions->add_componentoptions();
        protocolbuffers::ComAudioOptions* options = componentOptions->mutable_comaudiooptions();
        
        componentOptions->set_type("ComAudio");
        
        setComAudioOptions(options, objectData);
    }
    
    
    // children
    bool containChildrenElement = false;
    objectData = objectData->FirstChildElement();
    
    while (objectData)
    {
//        CCLOG("objectData name = %s", objectData->Name());
        
        if (strcmp("Children", objectData->Name()) == 0)
        {
            containChildrenElement = true;
            break;
        }
        
        objectData = objectData->NextSiblingElement();
    }
    
    if (containChildrenElement)
    {
        objectData = objectData->FirstChildElement();
//        CCLOG("element name = %s", objectData->Name());
        
        while (objectData)
        {
            protocolbuffers::NodeTree* subNodeTree = nodetree->add_children();
            
            const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
            bool bHasType = false;
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ctype")
                {
                    convertNodeTreeProtocolBuffersWithXMLForSimulator(subNodeTree, objectData, value);
                    bHasType = true;
                    break;
                }
                
                attribute = attribute->Next();
            }
            
            if(!bHasType)
            {
                convertNodeTreeProtocolBuffersWithXMLForSimulator(subNodeTree, objectData, "NodeObjectData");
            }
            
            objectData = objectData->NextSiblingElement();
        }
    }
    //
    
    /*
     while (element)
     {
     CCLOG("entity name = %s", element->Name());
     
     const tinyxml2::XMLAttribute* attribute = element->FirstAttribute();
     
     while (attribute)
     {
     const char* name = attribute->Name();
     const char* value = attribute->Value();
     CCLOG("attribute name = %s, value = %s", name, value);
     
     attribute = attribute->Next();
     }
     
     
     const tinyxml2::XMLElement* child = element->FirstChildElement();
     
     if (child)
     {
     element = child;
     }
     else
     {
     element = element->NextSiblingElement();
     }
     }
     */
}

void ProtocolBuffersSerialize::setProjectNodeOptionsForSimulator(protocolbuffers::ProjectNodeOptions *projectNodeOptions,
                                                                 protocolbuffers::WidgetOptions *nodeOptions,
                                                                 const tinyxml2::XMLElement *projectNodeObjectData)
{
    setNodeOptions(nodeOptions, projectNodeObjectData);
    
    ProjectNodeOptions* options = projectNodeOptions;
    
    // FileData
    const tinyxml2::XMLElement* child = projectNodeObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "FileData")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    options->set_filename(value);
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
}
/**/


std::string ProtocolBuffersSerialize::serializeProtocolBuffersWithXMLFile(const std::string &protocolbuffersFileName,
                                                              const std::string &xmlFileName, bool isSimulator/* = false*/)
{
    std::string result = "";    
    
//    CCLOG("protocolbuffersFileName = %s", protocolbuffersFileName.c_str());

    
	std::string fullpath = FileUtils::getInstance()->fullPathForFilename(xmlFileName).c_str();

    // xml read
    if (!FileUtils::getInstance()->isFileExist(fullpath))
    {
        return "file doesn not exists ";
    }
    
    ssize_t size;
    std::string content =(char*)FileUtils::getInstance()->getFileData(fullpath, "r", &size);
    
    // xml parse
    tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
    document->Parse(content.c_str());
    
    const tinyxml2::XMLElement* rootElement = document->RootElement();// Root
//    CCLOG("rootElement name = %s", rootElement->Name());
    
    const tinyxml2::XMLElement* element = rootElement->FirstChildElement();
    
    bool serializeEnabled = false;
    std::string rootType = "";
    
	_isSimulator = isSimulator;

    while (element)
    {
//        CCLOG("entity name = %s", element->Name());
        
        if (strcmp("Content", element->Name()) == 0)
        {
            const tinyxml2::XMLAttribute* attribute = element->FirstAttribute();
            
            // cheat
            if (!attribute)
            {
                serializeEnabled = true;
                rootType = "NodeObjectData";
            }
            //
            
            //
            //            while (attribute)
            //            {
            //                std::string name = attribute->Name();
            //                std::string value = attribute->Value();
            //                CCLOG("attribute name = %s, value = %s", name, value);
            //                if (name == "")
            //                {
            //                    serializeEnabled = true;
            //                    rootType = (strcmp("", value) == 0) ? "Node" : value;
            //                }
            //
            //                if (serializeEnabled)
            //                {
            //                    break;
            //                }
            //
            //                attribute = attribute->Next();
            //            }
            //
        }
        
        if (serializeEnabled)
        {
            break;
        }
        
        const tinyxml2::XMLElement* child = element->FirstChildElement();
        if (child)
        {
            element = child;
        }
        else
        {
            element = element->NextSiblingElement();
        }
    }
    
    
    // serialize
    if (serializeEnabled)
    {
        CSParseBinary protobuf;
        _protobuf = &protobuf;
        
        const tinyxml2::XMLElement* child = element->FirstChildElement();
        
        while (child)
        {
            std::string name = child->Name();
            
            if (name == "Animation") // action
            {
                protocolbuffers::NodeAction* nodeAction = protobuf.mutable_action();
                const tinyxml2::XMLElement* animation = child;
                convertActionProtocolBuffersWithXML(nodeAction, animation);
            }
            else if (name == "ObjectData") // nodeTree
            {
                protocolbuffers::NodeTree* nodeTreeRoot = protobuf.mutable_nodetree();
                const tinyxml2::XMLElement* objectData = child;
                convertNodeTreeProtocolBuffersWithXML(nodeTreeRoot, objectData, rootType);
            }
            
            child = child->NextSiblingElement();
        }
        
        
        
        // out, in for stream        
		std::string outFullPath = FileUtils::getInstance()->fullPathForFilename(protocolbuffersFileName);
        size_t pos = outFullPath.find_last_of('.');
        std::string convert = outFullPath.substr(0, pos).append(".csb");
        const char* temp = convert.c_str();
        FILE* file = fopen(temp, "wb");
        if (nullptr == file)
        {
            return " file cannot be opened ";
        }
        std::string serialString = "";
        if (!protobuf.SerializePartialToString(&serialString))
        {
            return " protocol buffers serialize incorrectly ";
        }
        fwrite(serialString.c_str(), sizeof(char), protobuf.ByteSize(), file);
        if (file)
        {
            fclose(file);
        }
    }
    
    return result;
}

/*
void ProtocolBuffersSerialize::serializeProtocolBuffersWithXMLFile(const std::string &protocolbuffersFileName,
                                                                   const std::string &xmlFileName)
{
    size_t pos = protocolbuffersFileName.find_last_of('/');
    _protocolbuffersDir = protocolbuffersFileName.substr(0, pos + 1);
    
    CCLOG("protocolbuffersFileName = %s", protocolbuffersFileName.c_str());
    
    // xml read
    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(xmlFileName).c_str();
    ssize_t size;
    std::string content =(char*)FileUtils::getInstance()->getFileData(fullpath, "r", &size);
    
    // xml parse
    tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
    document->Parse(content.c_str());
    
    const tinyxml2::XMLElement* rootElement = document->RootElement();// Root
    CCLOG("rootElement name = %s", rootElement->Name());
    
    const tinyxml2::XMLElement* element = rootElement->FirstChildElement();
    
    bool serializeEnabled = false;
    std::string rootType = "";
    
    while (element)
    {
        CCLOG("entity name = %s", element->Name());
        
        if (strcmp("Content", element->Name()) == 0)
        {
            const tinyxml2::XMLAttribute* attribute = element->FirstAttribute();
         
            // cheat
            if (!attribute)
            {
                serializeEnabled = true;
                rootType = "NodeObjectData";
            }
            //
            
            //
//            while (attribute)
//            {
//                std::string name = attribute->Name();
//                std::string value = attribute->Value();
//                CCLOG("attribute name = %s, value = %s", name, value);
//                if (name == "")
//                {
//                    serializeEnabled = true;
//                    rootType = (strcmp("", value) == 0) ? "Node" : value;
//                }
//                
//                if (serializeEnabled)
//                {
//                    break;
//                }
//                
//                attribute = attribute->Next();
//            }
             //
        }
        
        if (serializeEnabled)
        {
            break;
        }
        
        const tinyxml2::XMLElement* child = element->FirstChildElement();
        if (child)
        {
            element = child;
        }
        else
        {            
            element = element->NextSiblingElement();
        }
    }
    
    
    // serialize
    if (serializeEnabled)
    {
        CSParseBinary protobuf;
        
        const tinyxml2::XMLElement* child = element->FirstChildElement();
        
        while (child)
        {
            std::string name = child->Name();
            
            if (name == "Animation") // action
            {
                protocolbuffers::NodeAction* nodeAction = protobuf.mutable_action();
                const tinyxml2::XMLElement* animation = child;
                convertActionProtocolBuffersWithXML(nodeAction, animation);
            }
            else if (name == "ObjectData") // nodeTree
            {
                protocolbuffers::NodeTree* nodeTreeRoot = protobuf.mutable_nodetree();
                const tinyxml2::XMLElement* objectData = child;
                convertNodeTreeProtocolBuffersWithXML(nodeTreeRoot, objectData, rootType);
            }
            
            child = child->NextSiblingElement();
        }
        
        
        // out, in for stream
        const char* temp = protocolbuffersFileName.c_str();
        FILE* file = fopen(temp, "wb");
        if (nullptr == file)
        {
            return;
        }
        std::string serialString = "";
        if (!protobuf.SerializePartialToString(&serialString))
        {
            return;
        }
        fwrite(serialString.c_str(), sizeof(char), protobuf.ByteSize(), file);
        if (file)
        {
            fclose(file);
        }
    }
}
 */

void ProtocolBuffersSerialize::convertNodeTreeProtocolBuffersWithXML(protocolbuffers::NodeTree *nodetree,
                                                                     const tinyxml2::XMLElement *objectData,
                                                                     std::string classType)
{
    std::string classname = classType.substr(0, classType.find("ObjectData"));
//    CCLOG("classname = %s", classname.c_str());
    
    nodetree->set_classname(classname);
    
    if (classname == "Node")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        setNodeOptions(nodeOptions, objectData);
    }
    else if (classname == "SingleNode")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        setSingleNodeOptions(nodeOptions, objectData);
    }
    else if (classname == "Sprite")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        SpriteOptions* options = nodetree->mutable_spriteoptions();
        setSpriteOptions(options, nodeOptions, objectData);
    }
    else if (classname == "GameMap")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        TMXTiledMapOptions* options = nodetree->mutable_tmxtiledmapoptions();
        setTMXTiledMapOptions(options, nodeOptions, objectData);
    }
    else if (classname == "Particle")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        ParticleSystemOptions* options = nodetree->mutable_particlesystemoptions();
        setParticleSystemOptions(options, nodeOptions, objectData);
    }
    else if (classname == "Button")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        ButtonOptions* options = nodetree->mutable_buttonoptions();
        setButtonOptions(options, widgetOptions, objectData);
    }
    else if (classname == "CheckBox")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        CheckBoxOptions* options = nodetree->mutable_checkboxoptions();
        setCheckBoxOptions(options, widgetOptions, objectData);
    }
    else if (classname == "ImageView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        ImageViewOptions* options = nodetree->mutable_imageviewoptions();
        setImageViewOptions(options, widgetOptions, objectData);
    }
    else if (classname == "TextAtlas")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        TextAtlasOptions* options = nodetree->mutable_textatlasoptions();
        setTextAtlasOptions(options, widgetOptions, objectData);
    }
    else if (classname == "TextBMFont")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        TextBMFontOptions* options = nodetree->mutable_textbmfontoptions();
        setTextBMFontOptions(options, widgetOptions, objectData);
    }
    else if (classname == "Text")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        TextOptions* options = nodetree->mutable_textoptions();
        setTextOptions(options, widgetOptions, objectData);
    }
    else if (classname == "LoadingBar")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        LoadingBarOptions* options = nodetree->mutable_loadingbaroptions();
        setLoadingBarOptions(options, widgetOptions, objectData);
    }
    else if (classname == "Slider")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        SliderOptions* options = nodetree->mutable_slideroptions();
        setSliderOptions(options, widgetOptions, objectData);
    }
    else if (classname == "TextField")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        TextFieldOptions* options = nodetree->mutable_textfieldoptions();
        setTextFieldOptions(options, widgetOptions, objectData);
    }
    else if (classname == "Panel")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        PanelOptions* layoutOptions = nodetree->mutable_paneloptions();
        setLayoutOptions(layoutOptions, widgetOptions, objectData);
    }
    else if (classname == "PageView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        PageViewOptions* options = nodetree->mutable_pageviewoptions();
        setPageViewOptions(options, widgetOptions, objectData);
    }
    else if (classname == "ScrollView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        ScrollViewOptions* options = nodetree->mutable_scrollviewoptions();
        setScrollViewOptions(options, widgetOptions, objectData);
    }
    else if (classname == "ListView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        ListViewOptions* options = nodetree->mutable_listviewoptions();
        setListViewOptions(options, widgetOptions, objectData);
    }
    else if (classname == "ProjectNode")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        ProjectNodeOptions* options = nodetree->mutable_projectnodeoptions();
        setProjectNodeOptions(options, nodeOptions, objectData);
    }
    else if (classname == "SimpleAudio")
    {
        // process as component options
        
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        setSimpleAudioOptions(nodeOptions, objectData);
        
        protocolbuffers::ComponentOptions* componentOptions = nodeOptions->add_componentoptions();
        protocolbuffers::ComAudioOptions* options = componentOptions->mutable_comaudiooptions();
        
        componentOptions->set_type("ComAudio");
        
        setComAudioOptions(options, objectData);
    }
    
    
    // children
    bool containChildrenElement = false;
    objectData = objectData->FirstChildElement();
    
    while (objectData)
    {
//        CCLOG("objectData name = %s", objectData->Name());
        
        if (strcmp("Children", objectData->Name()) == 0)
        {
            containChildrenElement = true;
            break;
        }
        
        objectData = objectData->NextSiblingElement();
    }
    
    if (containChildrenElement)
    {
        objectData = objectData->FirstChildElement();
//        CCLOG("element name = %s", objectData->Name());
        
        while (objectData)
        {
            protocolbuffers::NodeTree* subNodeTree = nodetree->add_children();
            
            const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
			bool bHasType = false;
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ctype")
                {
                    convertNodeTreeProtocolBuffersWithXML(subNodeTree, objectData, value);
					bHasType = true;
                    break;
                }
                
                attribute = attribute->Next();
            }
            
			if(!bHasType)
			{
				convertNodeTreeProtocolBuffersWithXML(subNodeTree, objectData, "NodeObjectData");
			}
//            convertNodeTreeProtocolBuffersWithXML(subNodeTree, objectData, objectData->Name());
            
            objectData = objectData->NextSiblingElement();
        }
    }
    //
    
    /*
    while (element)
    {
        CCLOG("entity name = %s", element->Name());
        
        const tinyxml2::XMLAttribute* attribute = element->FirstAttribute();
     
        while (attribute)
        {
            const char* name = attribute->Name();
            const char* value = attribute->Value();
            CCLOG("attribute name = %s, value = %s", name, value);
     
            attribute = attribute->Next();
        }
        
     
        const tinyxml2::XMLElement* child = element->FirstChildElement();
        
        if (child)
        {
            element = child;
        }
        else
        {            
            element = element->NextSiblingElement();
        }
    }
     */
}

void ProtocolBuffersSerialize::setNodeOptions(protocolbuffers::WidgetOptions *nodeOptions,
                                              const tinyxml2::XMLElement *nodeObjectData)
{
    WidgetOptions* options = nodeOptions;

	options->set_visible(true);
	options->set_scalex(1.0f);
	options->set_scaley(1.0f);
    options->set_alpha(255);
    
    std::string name = nodeObjectData->Name();
//    CCLOG("entity name = %s", name.c_str());
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = nodeObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "Name")
        {
            options->set_name(value);
        }
        else if (name == "ActionTag")
        {
            options->set_actiontag(atol(value.c_str()));
        }
        else if (name == "RotationSkewX")
        {
            options->set_rotationskewx(atof(value.c_str()));
        }
        else if (name == "RotationSkewY")
        {
            options->set_rotationskewy(atof(value.c_str()));
        }
        else if (name == "Rotation")
        {
//            options->set_rotation(atoi(value.c_str()));
        }
        else if (name == "ZOrder")
        {
            options->set_zorder(atoi(value.c_str()));
        }
        else if (name == "Visible")
        {
//            options->set_visible((value == "True") ? true : false);
        }
        else if (name == "VisibleForFrame")
        {
            options->set_visible((value == "True") ? true : false);
        }
        else if (name == "Alpha")
        {
            options->set_alpha(atoi(value.c_str()));
        }
        else if (name == "Tag")
        {
            options->set_tag(atoi(value.c_str()));
        }
        
        attribute = attribute->Next();
    }
    
    const tinyxml2::XMLElement* child = nodeObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        if (name == "Children")
        {
            break;
        }
        else if (name == "Position")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    options->set_x(atof(value.c_str()));
                }
                else if (name == "Y")
                {
                    options->set_y(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "Scale")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ScaleX")
                {
                    options->set_scalex(atof(value.c_str()));
                }
                else if (name == "ScaleY")
                {
                    options->set_scaley(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "AnchorPoint")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ScaleX")
                {
                    options->set_anchorpointx(atof(value.c_str()));
                }
                else if (name == "ScaleY")
                {
                    options->set_anchorpointy(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "CColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "A")
                {
                    options->set_opacity(atoi(value.c_str()));
                }
                else if (name == "R")
                {
                    options->set_colorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_colorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_colorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "Size")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    options->set_width(atof(value.c_str()));
                }
                else if (name == "Y")
                {
                    options->set_height(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setSingleNodeOptions(protocolbuffers::WidgetOptions *nodeOptions,
                                                    const tinyxml2::XMLElement *nodeObjectData)
{
    setNodeOptions(nodeOptions, nodeObjectData);
}

void ProtocolBuffersSerialize::setSpriteOptions(protocolbuffers::SpriteOptions *spriteOptions,
                                                protocolbuffers::WidgetOptions *nodeOptions,
                                                const tinyxml2::XMLElement *spriteObjectData)
{
    setNodeOptions(nodeOptions, spriteObjectData);
    
    SpriteOptions* options = spriteOptions;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = spriteObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "FlipX" || name == "FilpX" )
        {
            options->set_flippedx((value == "True") ? true : false);
        }
        else if (name == "FlipY" || name == "FilpY")
        {
            options->set_flippedy((value == "True") ? true : false);
        }
        
        attribute = attribute->Next();
    }
    
    
    // FileData
    const tinyxml2::XMLElement* child = spriteObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "FileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* resourceData = options->mutable_filenamedata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setTMXTiledMapOptions(protocolbuffers::TMXTiledMapOptions *tmxTiledMapOptions,
                                                     protocolbuffers::WidgetOptions *nodeOptions,
                                                     const tinyxml2::XMLElement *tmxTiledMapObjectData)
{
    setNodeOptions(nodeOptions, tmxTiledMapObjectData);
    
    TMXTiledMapOptions* options = tmxTiledMapOptions;
    
    /*
    // attributes
    const tinyxml2::XMLAttribute* attribute = tmxTiledMapObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "TMXFile")
        {
            options->set_tmxfile(value);
        }
        else if (name == "TMXString")
        {
            options->set_tmxstring(value);
        }
        else if (name == "ResourcePath")
        {
            options->set_resourcepath(value);
        }
        
        attribute = attribute->Next();
    }
     */
    
    // child elements
    const tinyxml2::XMLElement* child = tmxTiledMapObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "FileData")
        {
            ResourceData* resourceData = options->mutable_filenamedata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    int resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setParticleSystemOptions(protocolbuffers::ParticleSystemOptions *particleSystemOptions,
                                                        protocolbuffers::WidgetOptions *nodeOptions,
                                                        const tinyxml2::XMLElement *particleSystemObjectData)
{
    setNodeOptions(nodeOptions, particleSystemObjectData);
    
    ParticleSystemOptions* options = particleSystemOptions;
    
    /*
    // attributes
    const tinyxml2::XMLAttribute* attribute = particleSystemObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "Plist")
        {
            options->set_plistfile(value);
        }
        else if (name == "TotalParticles")
        {
            options->set_totalparticles(atoi(value.c_str()));
        }
        
        attribute = attribute->Next();
    }
     */
    
    // child elements
    const tinyxml2::XMLElement* child = particleSystemObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "FileData")
        {
            ResourceData* resourceData = options->mutable_filenamedata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    int resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setWidgetOptions(protocolbuffers::WidgetOptions *widgetOptions,
                                                const tinyxml2::XMLElement *widgetObjectData)
{
    WidgetOptions* options = widgetOptions;
    
    std::string name = widgetObjectData->Name();
    
    options->set_visible(true);
	options->set_scalex(1.0f);
	options->set_scaley(1.0f);
    options->set_alpha(255);
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = widgetObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "Name")
        {
            options->set_name(value);
        }
        else if (name == "ActionTag")
        {
            options->set_actiontag(atol(value.c_str()));
        }
        else if (name == "RotationSkewX")
        {
            options->set_rotationskewx(atof(value.c_str()));
        }
        else if (name == "RotationSkewY")
        {
            options->set_rotationskewy(atof(value.c_str()));
        }
        else if (name == "Rotation")
        {
//            options->set_rotation(atoi(value.c_str()));
        }
        else if (name == "ZOrder")
        {
            options->set_zorder(atoi(value.c_str()));
        }
        else if (name == "Visible")
        {
//            options->set_visible((value == "True") ? true : false);
        }
        else if (name == "VisibleForFrame")
        {
            options->set_visible((value == "True") ? true : false);
        }
        else if (name == "Alpha")
        {
			options->set_alpha(atoi(value.c_str()));
        }
        else if (name == "Tag")
        {
            options->set_tag(atoi(value.c_str()));
        }
        else if (name == "FlipX")
        {
            options->set_flipx((value == "True") ? true : false);
        }
        else if (name == "FlipY")
        {
            options->set_flipy((value == "True") ? true : false);
        }
        else if (name == "TouchEnable")
        {
            options->set_touchable((value == "True") ? true : false);
        }
        else if (name == "ControlSizeType")
        {
            options->set_ignoresize((value == "Auto") ? true : false);
        }
        
        attribute = attribute->Next();
    }
    
    const tinyxml2::XMLElement* child = widgetObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        if (name == "Children")
        {
            break;
        }
        else if (name == "Position")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    options->set_x(atof(value.c_str()));
                }
                else if (name == "Y")
                {
                    options->set_y(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "Scale")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ScaleX")
                {
                    options->set_scalex(atof(value.c_str()));
                }
                else if (name == "ScaleY")
                {
                    options->set_scaley(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "AnchorPoint")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ScaleX")
                {
                    options->set_anchorpointx(atof(value.c_str()));
                }
                else if (name == "ScaleY")
                {
                    options->set_anchorpointy(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "CColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "A")
                {
                    options->set_opacity(atoi(value.c_str()));
                }
                else if (name == "R")
                {
                    options->set_colorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_colorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_colorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "Size")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    options->set_width(atof(value.c_str()));
                }
                else if (name == "Y")
                {
                    options->set_height(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setButtonOptions(protocolbuffers::ButtonOptions *buttonOptions,
                                                protocolbuffers::WidgetOptions *widgetOptions,
                                                const tinyxml2::XMLElement *buttonObjectData)
{
    setWidgetOptions(widgetOptions, buttonObjectData);
    
    ButtonOptions* options = buttonOptions;
    
    options->set_displaystate(true);
    
    bool scale9Enabled = false;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = buttonObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "Scale9Enable")
        {
            if (value == "True")
            {
                scale9Enabled = true;
            }
            options->set_scale9enable((value == "True") ? true : false);
        }
        else if (name == "Scale9OriginX")
        {
            options->set_capinsetsx(atof(value.c_str()));
        }
        else if (name == "Scale9OriginY")
        {
            options->set_capinsetsy(atof(value.c_str()));
        }
        else if (name == "Scale9Width")
        {
            options->set_capinsetswidth(atof(value.c_str()));
        }
        else if (name == "Scale9Height")
        {
            options->set_capinsetsheight(atof(value.c_str()));
        }
        else if (name == "ButtonText")
        {
            options->set_text(value);
        }
        else if (name == "FontSize")
        {
            options->set_fontsize(atoi(value.c_str()));
        }
        else if (name == "FontName")
        {
            options->set_fontname(value);
        }
		else if (name == "DisplayState")
		{
			options->set_displaystate((value == "True") ? true : false);
		}
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = buttonObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "Size" && scale9Enabled)
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    options->set_scale9width(atof(value.c_str()));
                }
                else if (name == "Y")
                {
                    options->set_scale9height(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "TextColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    options->set_textcolorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_textcolorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_textcolorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "DisabledFileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* disabledFileData = options->mutable_disableddata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    disabledFileData->set_path(value);
                }
                else if (name == "Type")
                {
					resourceType = getResourceType(value);
                    disabledFileData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    disabledFileData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        else if (name == "PressedFileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* pressedFileData = options->mutable_presseddata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    pressedFileData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    pressedFileData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    pressedFileData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        else if (name == "NormalFileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* normalFileData = options->mutable_normaldata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    normalFileData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    normalFileData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    normalFileData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        else if (name == "FontResource")
        {
            ResourceData* resourceData = options->mutable_fontresource();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    int resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setCheckBoxOptions(protocolbuffers::CheckBoxOptions *checkBoxOptions,
                                                  protocolbuffers::WidgetOptions *widgetOptions,
                                                  const tinyxml2::XMLElement *checkBoxObjectData)
{
    setWidgetOptions(widgetOptions, checkBoxObjectData);
    
    CheckBoxOptions* options = checkBoxOptions;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = checkBoxObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "CheckedState")
        {
            options->set_selectedstate((value == "True") ? true : false);
        }
        else if (name == "DiplayState" || name == "DisplayState")
        {
            options->set_displaystate((value == "True") ? true : false);
        }
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = checkBoxObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "NormalBackFileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* backgroundboxData = options->mutable_backgroundboxdata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    backgroundboxData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    backgroundboxData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    backgroundboxData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        else if (name == "PressedBackFileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* backGroundBoxSelectedData = options->mutable_backgroundboxselecteddata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    backGroundBoxSelectedData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    backGroundBoxSelectedData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    backGroundBoxSelectedData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        else if (name == "NodeNormalFileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* frontCrossData = options->mutable_frontcrossdata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    frontCrossData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    frontCrossData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    frontCrossData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        else if (name == "DisableBackFileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* backGroundBoxDisabledData = options->mutable_backgroundboxdisableddata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    backGroundBoxDisabledData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    backGroundBoxDisabledData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    backGroundBoxDisabledData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        else if (name == "NodeDisableFileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* frontCrossDisabledData = options->mutable_frontcrossdisableddata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    frontCrossDisabledData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    frontCrossDisabledData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    frontCrossDisabledData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setImageViewOptions(protocolbuffers::ImageViewOptions *imageViewOptions,
                                                   protocolbuffers::WidgetOptions *widgetOptions,
                                                   const tinyxml2::XMLElement *imageViewObjectData)
{
    setWidgetOptions(widgetOptions, imageViewObjectData);
    
    ImageViewOptions* options = imageViewOptions;
    
    bool scale9Enabled = false;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = imageViewObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "Scale9Enable")
        {
            if (value == "True")
            {
                scale9Enabled = true;
            }
            options->set_scale9enable((value == "True") ? true : false);
        }
        else if (name == "Scale9OriginX")
        {
            options->set_capinsetsx(atof(value.c_str()));
        }
        else if (name == "Scale9OriginY")
        {
            options->set_capinsetsy(atof(value.c_str()));
        }
        else if (name == "Scale9Width")
        {
            options->set_capinsetswidth(atof(value.c_str()));
        }
        else if (name == "Scale9Height")
        {
            options->set_capinsetsheight(atof(value.c_str()));
        }
        else if (name == "FontSize")
        {
            options->set_scale9height(atof(value.c_str()));
        } 
		else if (name == "FlipX" || name == "FilpX" )
        {
            options->set_flippedx((value == "True") ? true : false);
        }
        else if (name == "FlipY" || name == "FilpY")
        {
            options->set_flippedy((value == "True") ? true : false);
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = imageViewObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "Size" && scale9Enabled)
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    options->set_scale9width(atof(value.c_str()));
                }
                else if (name == "Y")
                {
                    options->set_scale9height(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* resourceData = options->mutable_filenamedata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setTextAtlasOptions(protocolbuffers::TextAtlasOptions *textAtlasOptions,
                                                   protocolbuffers::WidgetOptions *widgetOptions,
                                                   const tinyxml2::XMLElement *textAtlasObjectData)
{
    setWidgetOptions(widgetOptions, textAtlasObjectData);
    
    TextAtlasOptions* options = textAtlasOptions;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = textAtlasObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "LabelText")
        {
            options->set_stringvalue(value);
        }
        else if (name == "CharWidth")
        {
            options->set_itemwidth(atoi(value.c_str()));
        }
        else if (name == "CharHeight")
        {
            options->set_itemheight(atoi(value.c_str()));
        }
        else if (name == "StartChar")
        {
            options->set_startcharmap(value);
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = textAtlasObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "LabelAtlasFileImage_CNB")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* resourceData = options->mutable_charmapfiledata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setTextBMFontOptions(protocolbuffers::TextBMFontOptions *textBMFontOptions,
                                                    protocolbuffers::WidgetOptions *widgetOptions,
                                                    const tinyxml2::XMLElement *textBMFontObjectData)
{
    setWidgetOptions(widgetOptions, textBMFontObjectData);
    
    TextBMFontOptions* options = textBMFontOptions;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = textBMFontObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "LabelText")
        {
            options->set_text(value);
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = textBMFontObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "LabelBMFontFile_CNB")
        {
            ResourceData* resourceData = options->mutable_filenamedata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    int resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setTextOptions(protocolbuffers::TextOptions *textOptions,
                                              protocolbuffers::WidgetOptions *widgetOptions,
                                              const tinyxml2::XMLElement *textObjectData)
{
    setWidgetOptions(widgetOptions, textObjectData);
    
    TextOptions* options = textOptions;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = textObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "TouchScaleChangeAble")
        {
            options->set_touchscaleenable((value == "True") ? true : false);
        }
        else if (name == "LabelText")
        {
            options->set_text(value);
        }
        else if (name == "FontSize")
        {
            options->set_fontsize(atoi(value.c_str()));
        }
        else if (name == "FontName")
        {
            options->set_fontname(value);
        }
        else if (name == "AreaWidth")
        {
            options->set_areawidth(atoi(value.c_str()));
        }
        else if (name == "AreaHeight")
        {
            options->set_areaheight(atoi(value.c_str()));
        }
        else if (name == "HorizontalAlignmentType")
        {
            if (value == "HT_Left")
            {
                options->set_halignment(0);
            }
            else if (value == "HT_Center")
            {
                options->set_halignment(1);
            }
            else if (value == "HT_Right")
            {
                options->set_halignment(2);
            }
        }
        else if (name == "VerticalAlignmentType")
        {
            if (value == "VT_Top")
            {
                options->set_valignment(0);
            }
            else if (value == "VT_Center")
            {
                options->set_valignment(1);
            }
            else if (value == "VT_Bottom")
            {
                options->set_valignment(2);
            }
        }
        else if (name == "IsCustomSize")
        {
			options->set_iscustomsize((value == "True") ? true : false);
		}
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = textObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "FontResource")
        {
            ResourceData* resourceData = options->mutable_fontresource();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    int resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setLoadingBarOptions(protocolbuffers::LoadingBarOptions *loadingBarOptions,
                                                    protocolbuffers::WidgetOptions *widgetOptions,
                                                    const tinyxml2::XMLElement *loadingObjectData)
{
    setWidgetOptions(widgetOptions, loadingObjectData);
    
    LoadingBarOptions* options = loadingBarOptions;
    
    options->set_percent(0);
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = loadingObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "ProgressType")
        {
            options->set_direction((value == "Left_To_Right") ? 0 : 1);
        }
        else if (name == "ProgressInfo")
        {
            options->set_percent(atoi(value.c_str()));
        }
        else if (name == "Scale9Enable")
        {
            options->set_scale9enable((value == "True") ? true : false);
        }
        else if (name == "Scale9OriginX")
        {
            options->set_capinsetsx(atof(value.c_str()));
        }
        else if (name == "Scale9OriginY")
        {
            options->set_capinsetsy(atof(value.c_str()));
        }
        else if (name == "Scale9Width")
        {
            options->set_capinsetswidth(atof(value.c_str()));
        }
        else if (name == "Scale9Height")
        {
            options->set_capinsetsheight(atof(value.c_str()));
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = loadingObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "ImageFileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* resourceData = options->mutable_texturedata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setSliderOptions(protocolbuffers::SliderOptions *sliderOptions,
                                                protocolbuffers::WidgetOptions *widgetOptions,
                                                const tinyxml2::XMLElement *sliderObjectData)
{
    setWidgetOptions(widgetOptions, sliderObjectData);
    
    SliderOptions* options = sliderOptions;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = sliderObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "Scale9Enable")
        {
            options->set_scale9enable((value == "True") ? true : false);
        }
        else if (name == "Scale9OriginX")
        {
            options->set_capinsetsx(atof(value.c_str()));
        }
        else if (name == "Scale9OriginY")
        {
            options->set_capinsetsy(atof(value.c_str()));
        }
        else if (name == "Scale9Width")
        {
            options->set_capinsetswidth(atof(value.c_str()));
        }
        else if (name == "Scale9Height")
        {
            options->set_capinsetsheight(atof(value.c_str()));
        }
        else if (name == "Length")
        {
            
        }
        else if (name == "PercentInfo")
        {
            options->set_percent(atoi(value.c_str()));
        }
        else if (name == "DiplayState" || name == "DisplayState")
        {
            options->set_displaystate((value == "True") ? true : false);
            if (value == "False")
            {
                widgetOptions->set_touchable(false);
            }
        }
        
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = sliderObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "BackGroundData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* barFileNameData = options->mutable_barfilenamedata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    barFileNameData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    barFileNameData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    barFileNameData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        else if (name == "BallNormalData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* ballNormalData = options->mutable_ballnormaldata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    ballNormalData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    ballNormalData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    ballNormalData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        else if (name == "BallPressedData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* ballPressedData = options->mutable_ballpresseddata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    ballPressedData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    ballPressedData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    ballPressedData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        else if (name == "BallDisabledData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* ballDisabledData = options->mutable_balldisableddata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    ballDisabledData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    ballDisabledData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    ballDisabledData->set_plistfile(value);
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        else if (name == "ProgressBarData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* progressBarData = options->mutable_progressbardata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    progressBarData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    progressBarData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    progressBarData->set_plistfile(value);
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setTextFieldOptions(protocolbuffers::TextFieldOptions *textFieldOptions,
                                                   protocolbuffers::WidgetOptions *widgetOptions,
                                                   const tinyxml2::XMLElement *textFieldObjectData)
{
    setWidgetOptions(widgetOptions, textFieldObjectData);
    
    TextFieldOptions* options = textFieldOptions;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = textFieldObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "PlaceHolderText")
        {
            options->set_placeholder(value);
        }
        else if (name == "LabelText")
        {
            options->set_text(value);
        }
        else if (name == "FontSize")
        {
            options->set_fontsize(atoi(value.c_str()));
        }
        else if (name == "FontName")
        {
            options->set_fontname(value);
        }
        else if (name == "MaxLengthEnable")
        {
            options->set_maxlengthenable((value == "True") ? true : false);
            options->set_maxlength(0);
        }
        else if (name == "MaxLengthText")
        {
            options->set_maxlength(atoi(value.c_str()));
        }
        else if (name == "PasswordEnable")
        {
            options->set_passwordenable((value == "True") ? true : false);
        }
        else if (name == "PasswordStyleText")
        {
            options->set_passwordstyletext(value);
        }
//        else if (name == "ControlSizeType")
//        {
//            if (value == "Custom")
//            {
//                float areaWidth = 0.0f;
//                textFieldObjectData->QueryFloatAttribute("Width", &areaWidth);
//                options->set_areawidth(areaWidth);
//                
//                float areaHeight = 0.0f;
//                textFieldObjectData->QueryFloatAttribute("Height", &areaHeight);
//                options->set_areaheight(areaHeight);
//            }
//        }
        else if (name == "IsCustomSize")
        {
            options->set_iscustomsize((value == "True") ? true : false);
        }
        
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = textFieldObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "FontResource")
        {
            ResourceData* resourceData = options->mutable_fontresource();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    int resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                }
				else if (name == "AnchorPoint")
				{
					const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
					while (attribute)
					{
						std::string name = attribute->Name();
						std::string value = attribute->Value();
                
						if (name == "ScaleX")
						{
							options->set_anchorpointx(atof(value.c_str()));
						}
						else if (name == "ScaleY")
						{
							options->set_anchorpointy(atof(value.c_str()));
						}
                
						attribute = attribute->Next();
					}
				}
                
                attribute = attribute->Next();
            }
        }


        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setLayoutOptions(protocolbuffers::PanelOptions *layoutOptions,
                                                protocolbuffers::WidgetOptions *widgetOptions,
                                                const tinyxml2::XMLElement *layoutObjectData)
{
    setWidgetOptions(widgetOptions, layoutObjectData);
    
    PanelOptions* options = layoutOptions;
    
    options->set_colortype(0);
    options->set_bgcoloropacity(255);
    
	bool scale9Enabled = false;

    // attributes
    const tinyxml2::XMLAttribute* attribute = layoutObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "ClipAble")
        {
            options->set_clipable((value == "True") ? true : false);
        }
        else if (name == "ComboBoxIndex")
        {
            options->set_colortype(atoi(value.c_str()));
        }
        else if (name == "BackColorAlpha")
        {
            options->set_bgcoloropacity(atoi(value.c_str()));
        }
        else if (name == "Scale9Enable")
        {
            if (value == "True")
            {
                scale9Enabled = true;
            }
            options->set_backgroundscale9enable((value == "True") ? true : false);
        }
        else if (name == "Scale9OriginX")
        {
            options->set_capinsetsx(atof(value.c_str()));
        }
        else if (name == "Scale9OriginY")
        {
            options->set_capinsetsy(atof(value.c_str()));
        }
        else if (name == "Scale9Width")
        {
            options->set_capinsetswidth(atof(value.c_str()));
        }
        else if (name == "Scale9Height")
        {
            options->set_capinsetsheight(atof(value.c_str()));
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = layoutObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
       if (name == "Size" && scale9Enabled)
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    options->set_scale9width(atof(value.c_str()));
                }
                else if (name == "Y")
                {
                    options->set_scale9height(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "SingleColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    options->set_bgcolorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_bgcolorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_bgcolorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "EndColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    options->set_bgendcolorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_bgendcolorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_bgendcolorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FirstColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    options->set_bgstartcolorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_bgstartcolorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_bgstartcolorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "ColorVector")
        {
			const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ScaleX")
                {
                    options->set_vectorx(atof(value.c_str()));
                }
                else if (name == "ScaleY")
                {
                    options->set_vectory(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* resourceData = options->mutable_backgroundimagedata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setPageViewOptions(protocolbuffers::PageViewOptions *pageViewOptions,
                                                  protocolbuffers::WidgetOptions *widgetOptions,
                                                  const tinyxml2::XMLElement *pageViewObjectData)
{
    setWidgetOptions(widgetOptions, pageViewObjectData);
    
    PageViewOptions* options = pageViewOptions;
    
    options->set_colortype(0);
    options->set_bgcoloropacity(255);
    
    bool scale9Enabled = false;

    // attributes
    const tinyxml2::XMLAttribute* attribute = pageViewObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "ClipAble")
        {
            options->set_clipable((value == "True") ? true : false);
        }
        else if (name == "ComboBoxIndex")
        {
            options->set_colortype(atoi(value.c_str()));
        }
        else if (name == "BackColorAlpha")
        {
            options->set_bgcoloropacity(atoi(value.c_str()));
        }
        else if (name == "Scale9Enable")
        {
            if (value == "True")
            {
                scale9Enabled = true;
            }
            options->set_backgroundscale9enable((value == "True") ? true : false);
        }
        else if (name == "Scale9OriginX")
        {
            options->set_capinsetsx(atof(value.c_str()));
        }
        else if (name == "Scale9OriginY")
        {
            options->set_capinsetsy(atof(value.c_str()));
        }
        else if (name == "Scale9Width")
        {
            options->set_capinsetswidth(atof(value.c_str()));
        }
        else if (name == "Scale9Height")
        {
            options->set_capinsetsheight(atof(value.c_str()));
        }
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = pageViewObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "SingleColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    options->set_bgcolorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_bgcolorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_bgcolorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "Size" && scale9Enabled)
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    options->set_scale9width(atof(value.c_str()));
                }
                else if (name == "Y")
                {
                    options->set_scale9height(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "EndColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    options->set_bgendcolorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_bgendcolorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_bgendcolorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FirstColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    options->set_bgstartcolorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_bgstartcolorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_bgstartcolorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "ColorVector")
        {
			const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ScaleX")
                {
                    options->set_vectorx(atof(value.c_str()));
                }
                else if (name == "ScaleY")
                {
                    options->set_vectory(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* resourceData = options->mutable_backgroundimagedata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setScrollViewOptions(protocolbuffers::ScrollViewOptions *scrollViewOptions,
                                                    protocolbuffers::WidgetOptions *widgetOptions,
                                                    const tinyxml2::XMLElement *scrollViewObjectData)
{
    setWidgetOptions(widgetOptions, scrollViewObjectData);
    
    ScrollViewOptions* options = scrollViewOptions;
    
    options->set_colortype(0);
    options->set_bgcoloropacity(255);
    
    bool scale9Enabled = false;

    // attributes
    const tinyxml2::XMLAttribute* attribute = scrollViewObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "ClipAble")
        {
            options->set_clipable((value == "True") ? true : false);
        }
        else if (name == "ComboBoxIndex")
        {
            options->set_colortype(atoi(value.c_str()));
        }
        else if (name == "BackColorAlpha")
        {
            options->set_bgcoloropacity(atoi(value.c_str()));
        }
        else if (name == "Scale9Enable")
        {
            if (value == "True")
            {
                scale9Enabled = true;
            }
            options->set_backgroundscale9enable((value == "True") ? true : false);
        }
        else if (name == "Scale9OriginX")
        {
            options->set_capinsetsx(atof(value.c_str()));
        }
        else if (name == "Scale9OriginY")
        {
            options->set_capinsetsy(atof(value.c_str()));
        }
        else if (name == "Scale9Width")
        {
            options->set_capinsetswidth(atof(value.c_str()));
        }
        else if (name == "Scale9Height")
        {
            options->set_capinsetsheight(atof(value.c_str()));
        }
        else if (name == "ScrollDirectionType")
        {
            if (value == "Vertical")
            {
                options->set_direction(1);
            }
            else if (value == "Horizontal")
            {
                options->set_direction(2);
            }
            else if (value == "Vertical_Horizontal")
            {
                options->set_direction(3);
            }
        }
        else if (name == "IsBounceEnabled")
        {
            options->set_bounceenable((value == "True") ? true : false);
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = scrollViewObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "InnerNodeSize")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Width")
                {
                    options->set_innerwidth(atof(value.c_str()));
                }
                else if (name == "Height")
                {
                    options->set_innerheight(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "Size" && scale9Enabled)
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    options->set_scale9width(atof(value.c_str()));
                }
                else if (name == "Y")
                {
                    options->set_scale9height(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "SingleColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    options->set_bgcolorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_bgcolorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_bgcolorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "EndColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    options->set_bgendcolorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_bgendcolorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_bgendcolorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FirstColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    options->set_bgstartcolorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_bgstartcolorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_bgstartcolorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "ColorVector")
        {
			const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ScaleX")
                {
                    options->set_vectorx(atof(value.c_str()));
                }
                else if (name == "ScaleY")
                {
                    options->set_vectory(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* resourceData = options->mutable_backgroundimagedata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setListViewOptions(protocolbuffers::ListViewOptions *listViewOptions,
                                                  protocolbuffers::WidgetOptions *widgetOptions,
                                                  const tinyxml2::XMLElement *listViewObjectData)
{
    setWidgetOptions(widgetOptions, listViewObjectData);
    
    ListViewOptions* options = listViewOptions;
    
    options->set_colortype(0);
    options->set_bgcoloropacity(255);
    
    bool scale9Enabled = false;

    // attributes
    const tinyxml2::XMLAttribute* attribute = listViewObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "ClipAble")
        {
            options->set_clipable((value == "True") ? true : false);
        }
        else if (name == "ComboBoxIndex")
        {
            options->set_colortype(atoi(value.c_str()));
        }
        else if (name == "BackColorAlpha")
        {
            options->set_bgcoloropacity(atoi(value.c_str()));
        }
        else if (name == "Scale9Enable")
        {
            if (value == "True")
            {
                scale9Enabled = true;
            }
            options->set_backgroundscale9enable((value == "True") ? true : false);
        }
        else if (name == "Scale9OriginX")
        {
            options->set_capinsetsx(atof(value.c_str()));
        }
        else if (name == "Scale9OriginY")
        {
            options->set_capinsetsy(atof(value.c_str()));
        }
        else if (name == "Scale9Width")
        {
            options->set_capinsetswidth(atof(value.c_str()));
        }
        else if (name == "Scale9Height")
        {
            options->set_capinsetsheight(atof(value.c_str()));
        }
        else if (name == "DirectionType")
        {
            if (value == "Vertical")
            {
                options->set_direction(1);
                
                const tinyxml2::XMLAttribute* attribute = listViewObjectData->FirstAttribute();
                while (attribute)
                {
                    std::string name = attribute->Name();
                    std::string value = attribute->Value();
                    
                    if (name == "HorizontalType")
                    {
                        if (value == "HORIZONTAL_LEFT")
                        {
                            options->set_gravity(0);
                        }
                        else if (value == "HORIZONTAL_RIGHT")
                        {
                            options->set_gravity(1);
                        }
                        else if (value == "HORIZONTAL_CENTER")
                        {
                            options->set_gravity(2);
                        }
                    }

					attribute = attribute->Next();
                }
            }
            else if (value == "Horizontal")
            {
                options->set_direction(2);
                
                const tinyxml2::XMLAttribute* attribute = listViewObjectData->FirstAttribute();
                while (attribute)
                {
                    std::string name = attribute->Name();
                    std::string value = attribute->Value();
                    
                    if (name == "VerticalType")
                    {
                        if (value == "VERTICAL_TOP")
                        {
                            options->set_gravity(3);
                        }
                        else if (value == "VERTICAL_BOTTOM")
                        {
                            options->set_gravity(4);
                        }
                        else if (value == "VERTICAL_CENTER")
                        {
                            options->set_gravity(5);
                        }
                    }

					attribute = attribute->Next();
                }
            }
        }
        else if (name == "IsBounceEnabled")
        {
            options->set_bounceenable((value == "True") ? true : false);
        }
        else if (name == "ItemMargin")
        {
            options->set_itemmargin(atoi(value.c_str()));
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = listViewObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "InnerNodeSize")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Width")
                {
                    options->set_innerwidth(atof(value.c_str()));
                }
                else if (name == "Height")
                {
                    options->set_innerheight(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "Size" && scale9Enabled)
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    options->set_scale9width(atof(value.c_str()));
                }
                else if (name == "Y")
                {
                    options->set_scale9height(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "SingleColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    options->set_bgcolorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_bgcolorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_bgcolorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "EndColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    options->set_bgendcolorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_bgendcolorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_bgendcolorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FirstColor")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    options->set_bgstartcolorr(atoi(value.c_str()));
                }
                else if (name == "G")
                {
                    options->set_bgstartcolorg(atoi(value.c_str()));
                }
                else if (name == "B")
                {
                    options->set_bgstartcolorb(atoi(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "ColorVector")
        {
			const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ScaleX")
                {
                    options->set_vectorx(atof(value.c_str()));
                }
                else if (name == "ScaleY")
                {
                    options->set_vectory(atof(value.c_str()));
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FileData")
        {
            int resourceType = 0;
            std::string texture = "";
            std::string texturePng = "";
            
            ResourceData* resourceData = options->mutable_backgroundimagedata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _protobuf->add_textures(texture);
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _protobuf->add_texturespng(texturePng);
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setProjectNodeOptions(protocolbuffers::ProjectNodeOptions *projectNodeOptions,
                                                     protocolbuffers::WidgetOptions *nodeOptions,
                                                     const tinyxml2::XMLElement *projectNodeObjectData)
{
    setNodeOptions(nodeOptions, projectNodeObjectData);
    
    ProjectNodeOptions* options = projectNodeOptions;
    
    // FileData
    const tinyxml2::XMLElement* child = projectNodeObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "FileData")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    size_t pos = value.find_last_of('.');
                    std::string convert = value.substr(0, pos).append(".csb");
                    options->set_filename(convert);
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setSimpleAudioOptions(protocolbuffers::WidgetOptions *nodeOptions,
                                                     const tinyxml2::XMLElement *nodeObjectData)
{
    setNodeOptions(nodeOptions, nodeObjectData);
}

// component
void ProtocolBuffersSerialize::setComAudioOptions(protocolbuffers::ComAudioOptions *comAudioOptions,
                                                  const tinyxml2::XMLElement *audioObjectData)
{
    ComAudioOptions* options = comAudioOptions;
    
    options->set_enabled(true);
    
    const tinyxml2::XMLAttribute* attribute = audioObjectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "Loop")
        {
            options->set_loop((value == "True") ? true : false);
        }
        else if (name == "Volume")
        {
            options->set_volume(atoi(value.c_str()));
        }
        else if (name == "Name")
        {
            options->set_name(value);
        }
        
        attribute = attribute->Next();
    }
    
    // FileData
    const tinyxml2::XMLElement* child = audioObjectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "FileData")
        {
            ResourceData* resourceData = options->mutable_filenamedata();
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                std::string name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    resourceData->set_path(value);
                }
                else if (name == "Type")
                {
                    int resourceType = getResourceType(value);
                    resourceData->set_resourcetype(resourceType);
                }
                else if (name == "Plist")
                {
                    resourceData->set_plistfile(value);
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
}

// action
void ProtocolBuffersSerialize::convertActionProtocolBuffersWithXML(protocolbuffers::NodeAction *nodeAction,
                                                                   const tinyxml2::XMLElement *animation)
{
//    CCLOG("animation name = %s", animation->Name());
    
    // ActionTimeline
    const tinyxml2::XMLAttribute* attribute = animation->FirstAttribute();
    
    // attibutes
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "Duration")
        {
            nodeAction->set_duration(atoi(value.c_str()));
        }
        else if (name == "Speed")
        {
            nodeAction->set_speed(atof(value.c_str()));
        }
        
        attribute = attribute->Next();
    }
    
    // all Timeline
    const tinyxml2::XMLElement* timelineElement = animation->FirstChildElement();
    while (timelineElement)
    {
        protocolbuffers::TimeLine* timeLine = nodeAction->add_timelines();
        convertTimelineProtocolBuffers(timeLine, timelineElement);
        
        timelineElement = timelineElement->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::convertTimelineProtocolBuffers(protocolbuffers::TimeLine *timeLine,
                                                              const tinyxml2::XMLElement *timelineElement)
{
    // TimelineData attrsibutes
    std::string frameType = "";
    const tinyxml2::XMLAttribute* attribute = timelineElement->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "ActionTag")
        {
            timeLine->set_actiontag(atol(value.c_str()));
        }
        else if (name == "FrameType")
        {
            timeLine->set_frametype(value);
            frameType = value;
        }
        
        attribute = attribute->Next();
    }
    
    // all Frame
    const tinyxml2::XMLElement* frameElement = timelineElement->FirstChildElement();
    while (frameElement)
    {
        protocolbuffers::Frame* frame = timeLine->add_frames();
        
        if (frameType == FrameType_VisibleFrame)
        {
            protocolbuffers::TimeLineBoolFrame* visibleFrame = frame->mutable_visibleframe();
            setVisibleFrame(visibleFrame, frameElement);
        }
        else if (frameType == FrameType_PositionFrame)
        {
            protocolbuffers::TimeLinePointFrame* positionFrame = frame->mutable_positionframe();
            setPositionFrame(positionFrame, frameElement);
        }
        else if (frameType == FrameType_ScaleFrame)
        {
            protocolbuffers::TimeLinePointFrame* scaleFrame = frame->mutable_scaleframe();
            setScaleFrame(scaleFrame, frameElement);
        }
        else if (frameType == FrameType_RotationSkewFrame)
        {
            protocolbuffers::TimeLinePointFrame* rotationSkeFrame = frame->mutable_rotationskewframe();
            setRotationSkewFrame(rotationSkeFrame, frameElement);
        }
        else if (frameType == FrameType_AnchorFrame)
        {
            protocolbuffers::TimeLinePointFrame* anchorPointFrame = frame->mutable_anchorpointframe();
            setAnchorFrame(anchorPointFrame, frameElement);
        }
        else if (frameType == FrameType_ColorFrame)
        {
            protocolbuffers::TimeLineColorFrame* colorFrame = frame->mutable_colorframe();
            setColorFrame(colorFrame, frameElement);
        }
        else if (frameType == FrameType_TextureFrame)
        {
            protocolbuffers::TimeLineTextureFrame* textureFrame = frame->mutable_textureframe();
            setTextureFrame(textureFrame, frameElement);
        }
        else if (frameType == FrameType_EventFrame)
        {
            protocolbuffers::TimeLineStringFrame* eventFrame = frame->mutable_eventframe();
            setEventFrame(eventFrame, frameElement);
        }
        else if (frameType == FrameType_ZOrderFrame)
        {
            protocolbuffers::TimeLineIntFrame* zorderFrame = frame->mutable_zorderframe();
            setZOrderFrame(zorderFrame, frameElement);
        }
        
        frameElement = frameElement->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setVisibleFrame(protocolbuffers::TimeLineBoolFrame *visibleFrame,
                                               const tinyxml2::XMLElement *frameElement)
{
    visibleFrame->set_tween(true);
    
    const tinyxml2::XMLAttribute* attribute = frameElement->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "Value")
        {
            visibleFrame->set_value((value == "True") ? true : false);
        }
        else if (name == "FrameIndex")
        {
            visibleFrame->set_frameindex(atoi(value.c_str()));
        }
        else if (name == "Tween")
        {
            visibleFrame->set_tween((value == "True") ? true : false);
        }
        
        attribute = attribute->Next();
    }
}

void ProtocolBuffersSerialize::setPositionFrame(protocolbuffers::TimeLinePointFrame *positionFrame,
                                                const tinyxml2::XMLElement *frameElement)
{
    positionFrame->set_tween(true);
    
    const tinyxml2::XMLAttribute* attribute = frameElement->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "X")
        {
            positionFrame->set_x(atof(value.c_str()));
        }
        else if (name == "Y")
        {
            positionFrame->set_y(atof(value.c_str()));
        }
        else if (name == "FrameIndex")
        {
            positionFrame->set_frameindex(atoi(value.c_str()));
        }
        else if (name == "Tween")
        {
            positionFrame->set_tween((value == "True") ? true : false);
        }
        
        attribute = attribute->Next();
    }
}

void ProtocolBuffersSerialize::setScaleFrame(protocolbuffers::TimeLinePointFrame *scaleFrame,
                                             const tinyxml2::XMLElement *frameElement)
{
    scaleFrame->set_tween(true);
    
    const tinyxml2::XMLAttribute* attribute = frameElement->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "X")
        {
            scaleFrame->set_x(atof(value.c_str()));
        }
        else if (name == "Y")
        {
            scaleFrame->set_y(atof(value.c_str()));
        }
        else if (name == "FrameIndex")
        {
            scaleFrame->set_frameindex(atoi(value.c_str()));
        }
        else if (name == "Tween")
        {
            scaleFrame->set_tween((value == "True") ? true : false);
        }
        
        attribute = attribute->Next();
    }
}

void ProtocolBuffersSerialize::setRotationSkewFrame(protocolbuffers::TimeLinePointFrame *rotationSkewFrame,
                                                    const tinyxml2::XMLElement *frameElement)
{
    rotationSkewFrame->set_tween(true);
    
    const tinyxml2::XMLAttribute* attribute = frameElement->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "X")
        {
            rotationSkewFrame->set_x(atof(value.c_str()));
        }
        else if (name == "Y")
        {
            rotationSkewFrame->set_y(atof(value.c_str()));
        }
        else if (name == "FrameIndex")
        {
            rotationSkewFrame->set_frameindex(atoi(value.c_str()));
        }
        else if (name == "Tween")
        {
            rotationSkewFrame->set_tween((value == "True") ? true : false);
        }
        
        attribute = attribute->Next();
    }
}

void ProtocolBuffersSerialize::setAnchorFrame(protocolbuffers::TimeLinePointFrame *anchorPointframe,
                                              const tinyxml2::XMLElement *frameElement)
{
    anchorPointframe->set_tween(true);
    
    const tinyxml2::XMLAttribute* attribute = frameElement->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "X")
        {
            anchorPointframe->set_x(atof(value.c_str()));
        }
        else if (name == "Y")
        {
            anchorPointframe->set_y(atof(value.c_str()));
        }
        else if (name == "FrameIndex")
        {
            anchorPointframe->set_frameindex(atoi(value.c_str()));
        }
        else if (name == "Tween")
        {
            anchorPointframe->set_tween((value == "True") ? true : false);
        }
        
        attribute = attribute->Next();
    }
}

void ProtocolBuffersSerialize::setColorFrame(protocolbuffers::TimeLineColorFrame *colorFrame,
                                             const tinyxml2::XMLElement *frameElement)
{
    colorFrame->set_tween(true);
    
    const tinyxml2::XMLAttribute* attribute = frameElement->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "FrameIndex")
        {
            colorFrame->set_frameindex(atoi(value.c_str()));
        }
		else if (name == "Alpha")
		{
			colorFrame->set_alpha(atoi(value.c_str()));
		}		
        else if (name == "Tween")
        {
            colorFrame->set_tween((value == "True") ? true : false);
        }
        
        attribute = attribute->Next();
    }

	// color
	const tinyxml2::XMLElement* child = frameElement->FirstChildElement();
	while (child)
	{
		const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
		while (attribute)
		{
			std::string name = attribute->Name();
			std::string value = attribute->Value();

			if (name == "R")
			{
				colorFrame->set_red(atoi(value.c_str()));
			}
			else if (name == "G")
			{
				colorFrame->set_green(atoi(value.c_str()));
			}
			else if (name == "B")
			{
				colorFrame->set_blue(atoi(value.c_str()));
			}			

			attribute = attribute->Next();
		}

		child = child->NextSiblingElement();
	}
}

void ProtocolBuffersSerialize::setTextureFrame(protocolbuffers::TimeLineTextureFrame *textureFrame,
                                               const tinyxml2::XMLElement *frameElement)
{
    textureFrame->set_tween(true);
    
    const tinyxml2::XMLAttribute* attribute = frameElement->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "Value") // to be gonna modify
        {
            textureFrame->set_name(value);
        }
        else if (name == "FrameIndex")
        {
            textureFrame->set_frameindex(atoi(value.c_str()));
        }
        else if (name == "Tween")
        {
            textureFrame->set_tween((value == "True") ? true : false);
        }
        
        attribute = attribute->Next();
    }
    
    const tinyxml2::XMLElement* child = frameElement->FirstChildElement();
    while (child)
    {
        attribute = child->FirstAttribute();
        while (attribute)
        {
            std::string name = attribute->Name();
            std::string value = attribute->Value();
            
            if (name == "Path")
            {
                textureFrame->set_filepath(value);
            }
            
            attribute = attribute->Next();
        }
        
        child = child->NextSiblingElement();
    }
}

void ProtocolBuffersSerialize::setEventFrame(protocolbuffers::TimeLineStringFrame *eventFrame,
                                             const tinyxml2::XMLElement *frameElement)
{
    eventFrame->set_tween(true);
    
    const tinyxml2::XMLAttribute* attribute = frameElement->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "Value") // to be gonna modify
        {
            eventFrame->set_value(value);
        }
        else if (name == "FrameIndex")
        {
            eventFrame->set_frameindex(atoi(value.c_str()));
        }
        else if (name == "Tween")
        {
            eventFrame->set_tween((value == "True") ? true : false);
        }
        
        attribute = attribute->Next();
    }
}

void ProtocolBuffersSerialize::setZOrderFrame(protocolbuffers::TimeLineIntFrame *zorderFrame,
                                              const tinyxml2::XMLElement *frameElement)
{
    zorderFrame->set_tween(true);
    
    const tinyxml2::XMLAttribute* attribute = frameElement->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "Value") // to be gonna modify
        {
            zorderFrame->set_value(atoi(value.c_str()));
        }
        else if (name == "FrameIndex")
        {
            zorderFrame->set_frameindex(atoi(value.c_str()));
        }
        else if (name == "Tween")
        {
            zorderFrame->set_tween((value == "True") ? true : false);
        }
        
        attribute = attribute->Next();
    }
}
/**/

int ProtocolBuffersSerialize::getResourceType(std::string key)
{
	if(key == "Normal" || key == "Default")
	{
		return 	0;	
	}
	
	if(_isSimulator)
	{
		if(key == "MarkedSubImage")
		{
			return 0;
		}
	}
	return 1;
}
