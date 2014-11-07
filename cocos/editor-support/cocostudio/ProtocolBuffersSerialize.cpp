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
#include "cocostudio/ActionTimeline/CCNodeReader.h"
#include "../ui/CocosGUI.h"
#include "cocostudio/CSParseBinary.pb.h"
#include "cocostudio/CocoStudio.h"
#include "base/CCMap.h"

#include "tinyxml2/tinyxml2.h"

#include <fstream>

USING_NS_CC;
using namespace cocos2d::ui;
using namespace cocostudio;
using namespace cocostudio::timeline;
using namespace protocolbuffers;

static EtcProperties* sharedEtcProperties = nullptr;

EtcProperties::EtcProperties()
: _version("")
, _cocos2dVersion("")
, _editorType("")
, _designWidth(0)
, _designHeight(0)
, _dataScale(0)
{
    
}

EtcProperties::~EtcProperties()
{
    
}

EtcProperties* EtcProperties::getInstance()
{
    if (!sharedEtcProperties)
    {
        sharedEtcProperties = new EtcProperties();
    }
    return sharedEtcProperties;
}

void EtcProperties::purge()
{
    CC_SAFE_DELETE(sharedEtcProperties);
}

void EtcProperties::addTexture(const std::string &texture)
{
    _textures.push_back(texture);
}

void EtcProperties::addTexturePng(const std::string &png)
{
    _texturePngs.push_back(png);
}


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

/* serialize protocol buffers from XML */
void ProtocolBuffersSerialize::XMLTest(const std::string &fileName)
{
    const char* file_path = FileUtils::getInstance()->fullPathForFilename(fileName).c_str();
    ssize_t size;
    const char *pFileContent =(char*)FileUtils::getInstance()->getFileData(file_path, "r", &size);
    
    tinyxml2::XMLDocument* myDocument = new tinyxml2::XMLDocument();
    myDocument->Parse(pFileContent);
    
    const tinyxml2::XMLElement* rootElement = myDocument->RootElement();// Root
    CCLOG("rootElement name = %s", rootElement->Name());
    
    const tinyxml2::XMLElement* entity = rootElement->FirstChildElement();
    
    while (entity)
    {
        CCLOG("entity name = %s", entity->Name());
        
        const tinyxml2::XMLAttribute* attribute = entity->FirstAttribute();
        
        while (attribute)
        {
            const char* name = attribute->Name();
            const char* value = attribute->Value();
            CCLOG("attribute name = %s, value = %s", name, value);
            
            attribute = attribute->Next();
        }
        
        
        const tinyxml2::XMLElement* child = entity->FirstChildElement();
        
        if (child)
        {
            entity = child;
        }
        else
        {
            
            entity = entity->NextSiblingElement();
        }
    }
    
    
    
    
    
    /*
    // xml file path
    std::string filePath = FileUtils::getInstance()->fullPathForFilename(fileName);
    // xmlDoc
    tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
    pDoc->LoadFile(filePath.c_str());
    
    // root element
    tinyxml2::XMLElement *rootEle = pDoc->RootElement();
    //
    CCLOG("root name = %s", rootEle->Name());
    //    CCLOG("%s",rootEle->GetText());
    
    // attribute
    const tinyxml2::XMLAttribute *attribute = rootEle->FirstAttribute();
    //
    //    CCLOG("attribute name = %s", attribute->Name());
    //    CCLOG("%s %s",attribute->Name(),attribute->Value());
    
    // query attribute value
    float value = 0.1f;
    rootEle->QueryFloatAttribute("version", &value);
    CCLOG("%f",value);
    
    // set attribute value
    rootEle->SetAttribute("version", 1.4);
    // child element of root element
    tinyxml2::XMLElement *dictEle = rootEle->FirstChildElement();
    CCLOG("child name = %s", dictEle->Name());
    
    const tinyxml2::XMLAttribute* attri = dictEle->FirstAttribute();
    CCLOG("attri name = %s, value = %s", attri->Name(), attri->Value());
    
    //
    tinyxml2::XMLElement *keyEle = dictEle->FirstChildElement();
    //
    //    CCLOG("%s,%s",keyEle->Name(),keyEle->GetText());
    
    //
    tinyxml2::XMLElement *stringEle = keyEle->NextSiblingElement();
    CCLOG("%s,%s",stringEle->Name(),stringEle->GetText());
    
    //
    tinyxml2::XMLElement *nulXmlEle = stringEle->FirstChildElement();
    if (NULL == nulXmlEle)
    {
        CCLOG("has no child element");
    }
     */
}

/* create protocol buffers from XML */
CSParseBinary* ProtocolBuffersSerialize::createProtocolBuffersWithXMLFileForSimulator(const std::string &xmlFileName,
                                                                          bool isSimulator/* = true*/)
{
    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(xmlFileName).c_str();
    
    // xml read
    if (!FileUtils::getInstance()->isFileExist(fullpath))
    {
        CCLOG("file doesn not exists ");
        return nullptr;
    }
    
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
    
    _isSimulator = isSimulator;
    
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
    CCLOG("classname = %s", classname.c_str());
    
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
        CCLOG("objectData name = %s", objectData->Name());
        
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
        CCLOG("element name = %s", objectData->Name());
        
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
    
    CCLOG("protocolbuffersFileName = %s", protocolbuffersFileName.c_str());

    
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
    CCLOG("rootElement name = %s", rootElement->Name());
    
    const tinyxml2::XMLElement* element = rootElement->FirstChildElement();
    
    bool serializeEnabled = false;
    std::string rootType = "";
    
	_isSimulator = isSimulator;

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
    CCLOG("classname = %s", classname.c_str());
    
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
        CCLOG("objectData name = %s", objectData->Name());
        
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
        CCLOG("element name = %s", objectData->Name());
        
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
    
    std::string name = nodeObjectData->Name();
    CCLOG("entity name = %s", name.c_str());
    
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
    CCLOG("animation name = %s", animation->Name());
    
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

/* peterson json */
/* serialize protocol buffers from json */
void ProtocolBuffersSerialize::serializeProtocolBuffersWithJson(const std::string &protocolbuffersFileName,
                                                           const std::string &jsonFileName)
{
    CSParseBinary protobuf;
    
    // Read content from file
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(jsonFileName);
    ssize_t size;
    std::string content = (char*)FileUtils::getInstance()->getFileData(fullPath, "r", &size);
    
    rapidjson::Document doc;
    doc.Parse<0>(content.c_str());
    if (doc.HasParseError())
    {
        CCLOG("GetParseError %s\n", doc.GetParseError());
    }
    
    std::string cocos2dVersion = "";
    bool cocos2dVersion_exist = DICTOOL->checkObjectExist_json(doc, "cocos2dVersion");
    if (cocos2dVersion_exist)
    {
        cocos2dVersion = DICTOOL->getStringValue_json(doc, "cocos2dVersion");
    }
    else
    {
        cocos2dVersion = "3.x";
    }
    protobuf.set_cocos2dversion(cocos2dVersion);
    
    const rapidjson::Value& nodeTreeJson = DICTOOL->getSubDictionary_json(doc, "nodeTree");
    protocolbuffers::NodeTree* nodeTree = protobuf.mutable_nodetree();
    convertNodeTreeProtocolBuffersWithJson(nodeTree, nodeTreeJson);
    
    const rapidjson::Value& actionJson = DICTOOL->getSubDictionary_json(doc, "action");
    protocolbuffers::NodeAction* nodeAction = protobuf.mutable_action();
    convertActionProtocolBuffersWithJson(nodeAction, actionJson);
    
    
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

void ProtocolBuffersSerialize::convertNodeTreeProtocolBuffersWithJson(protocolbuffers::NodeTree *nodetree,
                                                                      const rapidjson::Value &json)
{
    std::string classname = DICTOOL->getStringValue_json(json, "classname");
    const rapidjson::Value& optionsJson = DICTOOL->getSubDictionary_json(json, "options");
    
    nodetree->set_classname(classname);
    
    if (classname == "Node")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        setNodeOptions(nodeOptions, optionsJson);
    }
    else if (classname == "Sprite")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        SpriteOptions* spriteOptions = nodetree->mutable_spriteoptions();
        setSpriteOptions(spriteOptions, widgetOptions, optionsJson);
    }
    else if (classname == "Button")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        ButtonOptions* buttonOptions = nodetree->mutable_buttonoptions();
        setButtonOptions(buttonOptions, widgetOptions, optionsJson);
    }
    else if (classname == "CheckBox")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        CheckBoxOptions* checkBoxOptions = nodetree->mutable_checkboxoptions();
        setCheckBoxOptions(checkBoxOptions, widgetOptions, optionsJson);
    }
    else if (classname == "ImageView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        ImageViewOptions* imageViewOptions = nodetree->mutable_imageviewoptions();
        setImageViewOptions(imageViewOptions, widgetOptions, optionsJson);
    }
    else if (classname == "TextAtlas" || classname == "LabelAtlas")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        TextAtlasOptions* textAtlasOptions = nodetree->mutable_textatlasoptions();
        setTextAtlasOptions(textAtlasOptions, widgetOptions, optionsJson);
    }
    else if (classname == "TextBMFont" || classname == "LabelBMFont")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        TextBMFontOptions* textBMFontOptions = nodetree->mutable_textbmfontoptions();
        setTextBMFontOptions(textBMFontOptions, widgetOptions, optionsJson);
    }
    else if (classname == "Text" || classname == "Label")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        TextOptions* textOptions = nodetree->mutable_textoptions();
        setTextOptions(textOptions, widgetOptions, optionsJson);
    }
    else if (classname == "LoadingBar")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        LoadingBarOptions* loadingBarOptions = nodetree->mutable_loadingbaroptions();
        setLoadingBarOptions(loadingBarOptions, widgetOptions, optionsJson);
    }
    else if (classname == "Slider")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        SliderOptions* sliderOptions = nodetree->mutable_slideroptions();
        setSliderOptions(sliderOptions, widgetOptions, optionsJson);
    }
    else if (classname == "TextField")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        TextFieldOptions* textFieldOptions = nodetree->mutable_textfieldoptions();
        setTextFieldOptions(textFieldOptions, widgetOptions, optionsJson);
    }
    else if (classname == "Panel")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        PanelOptions* layoutOptions = nodetree->mutable_paneloptions();
        setLayoutOptions(layoutOptions, widgetOptions, optionsJson);
    }
    else if (classname == "PageView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        PageViewOptions* pageViewOptions = nodetree->mutable_pageviewoptions();
        setPageViewOptions(pageViewOptions, widgetOptions, optionsJson);
    }
    else if (classname == "ScrollView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        ScrollViewOptions* scrollViewOptions = nodetree->mutable_scrollviewoptions();
        setScrollViewOptions(scrollViewOptions, widgetOptions, optionsJson);
    }
    else if (classname == "ListView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        ListViewOptions* listViewOptions = nodetree->mutable_listviewoptions();
        setListViewOptions(listViewOptions, widgetOptions, optionsJson);
    }
    
    int childrenCount = DICTOOL->getArrayCount_json(json, "children");
    for (int i = 0; i < childrenCount; i++)
    {
        protocolbuffers::NodeTree* childNodeTree = nodetree->add_children();
        const rapidjson::Value& childJson = DICTOOL->getDictionaryFromArray_json(json, "children", i);
        convertNodeTreeProtocolBuffersWithJson(childNodeTree, childJson);
    }
}

void ProtocolBuffersSerialize::setNodeOptions(protocolbuffers::WidgetOptions *nodeOptions,
                                              const rapidjson::Value &optionsJson)
{
    protocolbuffers::WidgetOptions* options = nodeOptions;
    bool ignoreSizeExsit = DICTOOL->checkObjectExist_json(optionsJson, "ignoreSize");
    if (ignoreSizeExsit)
    {
        options->set_ignoresize(DICTOOL->getBooleanValue_json(optionsJson, "ignoreSize"));
    }
    
    options->set_sizetype(DICTOOL->getIntValue_json(optionsJson, "sizeType"));
    options->set_positiontype(DICTOOL->getIntValue_json(optionsJson, "positionType"));
    
    options->set_sizepercentx(DICTOOL->getFloatValue_json(optionsJson, "sizePercentX"));
    options->set_sizepercenty(DICTOOL->getFloatValue_json(optionsJson, "sizePercentY"));
    
    options->set_positionpercentx(DICTOOL->getFloatValue_json(optionsJson, "positionPercentX"));
    options->set_positionpercenty(DICTOOL->getFloatValue_json(optionsJson, "positionPercentY"));
    
    float width = DICTOOL->getFloatValue_json(optionsJson, "width");
    float height = DICTOOL->getFloatValue_json(optionsJson, "height");
    options->set_width(width);
    options->set_height(height);
    
    options->set_tag(DICTOOL->getIntValue_json(optionsJson, "tag"));
    options->set_actiontag(DICTOOL->getIntValue_json(optionsJson, "actionTag"));
    options->set_touchable(DICTOOL->getBooleanValue_json(optionsJson, "touchAble"));
    const char* name = DICTOOL->getStringValue_json(optionsJson, "name");
    const char* nodeName = name ? name:"default";
    options->set_name(nodeName);
    
    float x = DICTOOL->getFloatValue_json(optionsJson, "x");
    float y = DICTOOL->getFloatValue_json(optionsJson, "y");
    options->set_x(x);
    options->set_y(y);
    
    options->set_scalex(DICTOOL->getFloatValue_json(optionsJson, "scaleX", 1.0));
    options->set_scaley(DICTOOL->getFloatValue_json(optionsJson, "scaleY", 1.0));
    
    options->set_rotation(DICTOOL->getFloatValue_json(optionsJson, "rotation", 0));
    
    options->set_visible(true);
    bool vb = DICTOOL->checkObjectExist_json(optionsJson, "visible");
    if (vb)
    {
        options->set_visible(DICTOOL->getBooleanValue_json(optionsJson, "visible"));
    }
    int z = DICTOOL->getIntValue_json(optionsJson, "ZOrder");
    options->set_zorder(z);
    
    
    options->set_opacity(255);
    bool op = DICTOOL->checkObjectExist_json(optionsJson, "opacity");
    if (op)
    {
        options->set_opacity(DICTOOL->getIntValue_json(optionsJson, "opacity"));
    }
    bool cr = DICTOOL->checkObjectExist_json(optionsJson, "colorR");
    bool cg = DICTOOL->checkObjectExist_json(optionsJson, "colorG");
    bool cb = DICTOOL->checkObjectExist_json(optionsJson, "colorB");
    int colorR = cr ? DICTOOL->getIntValue_json(optionsJson, "colorR") : 255;
    int colorG = cg ? DICTOOL->getIntValue_json(optionsJson, "colorG") : 255;
    int colorB = cb ? DICTOOL->getIntValue_json(optionsJson, "colorB") : 255;
    options->set_colorr(colorR);
    options->set_colorg(colorG);
    options->set_colorb(colorB);
    
    bool isAnchorPointXExists = DICTOOL->checkObjectExist_json(optionsJson, "anchorPointX");
    float anchorPointXInFile = 0.0f;
    if (isAnchorPointXExists)
    {
        anchorPointXInFile = DICTOOL->getFloatValue_json(optionsJson, "anchorPointX");
    }
    
    bool isAnchorPointYExists = DICTOOL->checkObjectExist_json(optionsJson, "anchorPointY");
    float anchorPointYInFile = 0.0f;
    if (isAnchorPointYExists)
    {
        anchorPointYInFile = DICTOOL->getFloatValue_json(optionsJson, "anchorPointY");
    }
    
    if (isAnchorPointXExists || isAnchorPointYExists)
    {
        options->set_anchorpointx(anchorPointXInFile);
        options->set_anchorpointy(anchorPointYInFile);
    }
    
    bool flipX = DICTOOL->getBooleanValue_json(optionsJson, "flipX");
    bool flipY = DICTOOL->getBooleanValue_json(optionsJson, "flipY");
    options->set_flipx(flipX);
    options->set_flipx(flipY);
}

void ProtocolBuffersSerialize::setSpriteOptions(protocolbuffers::SpriteOptions *spriteOptions,
                                                protocolbuffers::WidgetOptions *nodeOptions,
                                                const rapidjson::Value &optionsJson)
{
    setNodeOptions(nodeOptions, optionsJson);
    
    SpriteOptions* options = spriteOptions;
    
    int type = 0;
    std::string path = "", plistFile = "";
    
    bool imageFileNameDicExist = DICTOOL->checkObjectExist_json(optionsJson, "fileNameData");
    if (imageFileNameDicExist)
    {
        ResourceData* imageFileData = options->mutable_filenamedata();
        const rapidjson::Value& imageFileNameDic = DICTOOL->getSubDictionary_json(optionsJson, "fileNameData");
        type = DICTOOL->getIntValue_json(imageFileNameDic, "resourceType");
        path = DICTOOL->getStringValue_json(imageFileNameDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(imageFileNameDic, "plistFile", "");
        imageFileData->set_resourcetype(type);
        imageFileData->set_path(path);
        plistFile = "armature/Cowboy0.plist"; // cheat
        imageFileData->set_plistfile(plistFile);
    }
}

void ProtocolBuffersSerialize::setWidgetOptions(protocolbuffers::WidgetOptions *widgetOptions,
                                                const rapidjson::Value &optionsJson)
{
    protocolbuffers::WidgetOptions* options = widgetOptions;
    bool ignoreSizeExsit = DICTOOL->checkObjectExist_json(optionsJson, "ignoreSize");
    if (ignoreSizeExsit)
    {
        options->set_ignoresize(DICTOOL->getBooleanValue_json(optionsJson, "ignoreSize"));
    }
    
    options->set_sizetype(DICTOOL->getIntValue_json(optionsJson, "sizeType"));
    options->set_positiontype(DICTOOL->getIntValue_json(optionsJson, "positionType"));
    
    options->set_sizepercentx(DICTOOL->getFloatValue_json(optionsJson, "sizePercentX"));
    options->set_sizepercenty(DICTOOL->getFloatValue_json(optionsJson, "sizePercentY"));
    
    options->set_positionpercentx(DICTOOL->getFloatValue_json(optionsJson, "positionPercentX"));
    options->set_positionpercenty(DICTOOL->getFloatValue_json(optionsJson, "positionPercentY"));
    
    float width = DICTOOL->getFloatValue_json(optionsJson, "width");
    float height = DICTOOL->getFloatValue_json(optionsJson, "height");
    options->set_width(width);
    options->set_height(height);
    
    options->set_tag(DICTOOL->getIntValue_json(optionsJson, "tag"));
    options->set_actiontag(DICTOOL->getIntValue_json(optionsJson, "actiontag"));
    options->set_touchable(DICTOOL->getBooleanValue_json(optionsJson, "touchAble"));
    const char* name = DICTOOL->getStringValue_json(optionsJson, "name");
    const char* nodeName = name ? name:"default";
    options->set_name(nodeName);
    
    float x = DICTOOL->getFloatValue_json(optionsJson, "x");
    float y = DICTOOL->getFloatValue_json(optionsJson, "y");
    options->set_x(x);
    options->set_y(y);
    
    options->set_scalex(DICTOOL->getFloatValue_json(optionsJson, "scaleX", 1.0));
    options->set_scaley(DICTOOL->getFloatValue_json(optionsJson, "scaleY", 1.0));
    
    options->set_rotation(DICTOOL->getFloatValue_json(optionsJson, "rotation", 0));
    
    options->set_visible(true);
    bool vb = DICTOOL->checkObjectExist_json(optionsJson, "visible");
    if (vb)
    {
        options->set_visible(DICTOOL->getBooleanValue_json(optionsJson, "visible"));
    }
    int z = DICTOOL->getIntValue_json(optionsJson, "ZOrder");
    options->set_zorder(z);
    
    
    options->set_alpha(255);
    bool op = DICTOOL->checkObjectExist_json(optionsJson, "opacity");
    if (op)
    {
        options->set_alpha(DICTOOL->getIntValue_json(optionsJson, "opacity"));
    }
    bool cr = DICTOOL->checkObjectExist_json(optionsJson, "colorR");
    bool cg = DICTOOL->checkObjectExist_json(optionsJson, "colorG");
    bool cb = DICTOOL->checkObjectExist_json(optionsJson, "colorB");
    int colorR = cr ? DICTOOL->getIntValue_json(optionsJson, "colorR") : 255;
    int colorG = cg ? DICTOOL->getIntValue_json(optionsJson, "colorG") : 255;
    int colorB = cb ? DICTOOL->getIntValue_json(optionsJson, "colorB") : 255;
    options->set_colorr(colorR);
    options->set_colorg(colorG);
    options->set_colorb(colorB);
    
    bool isAnchorPointXExists = DICTOOL->checkObjectExist_json(optionsJson, "anchorPointX");
    float anchorPointXInFile = 0.5f;
    if (isAnchorPointXExists)
    {
        anchorPointXInFile = DICTOOL->getFloatValue_json(optionsJson, "anchorPointX");
    }
    
    bool isAnchorPointYExists = DICTOOL->checkObjectExist_json(optionsJson, "anchorPointY");
    float anchorPointYInFile = 0.5f;
    if (isAnchorPointYExists)
    {
        anchorPointYInFile = DICTOOL->getFloatValue_json(optionsJson, "anchorPointY");
    }
    
    options->set_anchorpointx(anchorPointXInFile);
    options->set_anchorpointy(anchorPointYInFile);    
    
    bool flipX = DICTOOL->getBooleanValue_json(optionsJson, "flipX");
    bool flipY = DICTOOL->getBooleanValue_json(optionsJson, "flipY");
    options->set_flipx(flipX);
    options->set_flipx(flipY);
}

void ProtocolBuffersSerialize::setButtonOptions(protocolbuffers::ButtonOptions *buttonOptions,
                                                protocolbuffers::WidgetOptions *widgetOptions,
                                                const rapidjson::Value &optionsJson)
{
    setWidgetOptions(widgetOptions, optionsJson);
    
    ButtonOptions* options = buttonOptions;
    
    bool scale9Enable = DICTOOL->getBooleanValue_json(optionsJson, "scale9Enable");
    options->set_scale9enable(scale9Enable);
    
    int type = 0;
    std::string path = "", plistFile = "";
    
    bool normalDicExist = DICTOOL->checkObjectExist_json(optionsJson, "normalData");
    if (normalDicExist)
    {
        ResourceData* normalFileData = options->mutable_normaldata();
        const rapidjson::Value& normalDic = DICTOOL->getSubDictionary_json(optionsJson, "normalData");
        type = DICTOOL->getIntValue_json(normalDic, "resourceType");
        path = DICTOOL->getStringValue_json(normalDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(normalDic, "plistFile", "");
        normalFileData->set_resourcetype(type);
        normalFileData->set_path(path);
        normalFileData->set_plistfile(plistFile);
    }
    
    bool pressedDicExist = DICTOOL->checkObjectExist_json(optionsJson, "pressedData");
    if (pressedDicExist)
    {
        ResourceData* pressedFileData = options->mutable_presseddata();
        const rapidjson::Value& pressedDic = DICTOOL->getSubDictionary_json(optionsJson, "pressedData");
        type = DICTOOL->getIntValue_json(pressedDic, "resourceType");
        path = DICTOOL->getStringValue_json(pressedDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(pressedDic, "plistFile", "");
        pressedFileData->set_resourcetype(type);
        pressedFileData->set_path(path);
        pressedFileData->set_plistfile(plistFile);
    }
    
    bool disabledDicExist = DICTOOL->checkObjectExist_json(optionsJson, "disabledData");
    if (disabledDicExist)
    {
        ResourceData* disabledFileData = options->mutable_disableddata();
        const rapidjson::Value& disabledDic = DICTOOL->getSubDictionary_json(optionsJson, "disabledData");
        type = DICTOOL->getIntValue_json(disabledDic, "resourceType");
        path = DICTOOL->getStringValue_json(disabledDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(disabledDic, "plistFile", "");
        disabledFileData->set_resourcetype(type);
        disabledFileData->set_path(path);
        disabledFileData->set_plistfile(plistFile);
    }
    
    if (scale9Enable)
    {
        float cx = DICTOOL->getFloatValue_json(optionsJson, "capInsetsX");
        float cy = DICTOOL->getFloatValue_json(optionsJson, "capInsetsY");
        float cw = DICTOOL->getFloatValue_json(optionsJson, "capInsetsWidth");
        float ch = DICTOOL->getFloatValue_json(optionsJson, "capInsetsHeight");
        options->set_capinsetsx(cx);
        options->set_capinsetsy(cy);
        options->set_capinsetswidth(cw);
        options->set_capinsetsheight(ch);
        
        bool sw = DICTOOL->checkObjectExist_json(optionsJson, "scale9Width");
        bool sh = DICTOOL->checkObjectExist_json(optionsJson, "scale9Height");
        if (sw && sh)
        {
            float swf = DICTOOL->getFloatValue_json(optionsJson, "scale9Width");
            float shf = DICTOOL->getFloatValue_json(optionsJson, "scale9Height");
            
            options->set_scale9width(swf);
            options->set_scale9height(shf);
        }
    }
    bool tt = DICTOOL->checkObjectExist_json(optionsJson, "text");
    if (tt)
    {
        const char* text = DICTOOL->getStringValue_json(optionsJson, "text");
        if (text)
        {
            options->set_text(text);
        }
    }
    
    
    int cri = DICTOOL->getIntValue_json(optionsJson, "textColorR", 255);
    int cgi = DICTOOL->getIntValue_json(optionsJson, "textColorgG", 255);
    int cbi = DICTOOL->getIntValue_json(optionsJson, "textColorB", 255);
    options->set_textcolorr(cri);
    options->set_textcolorg(cgi);
    options->set_textcolorb(cbi);
    
    options->set_fontsize(DICTOOL->getIntValue_json(optionsJson, "fontSize", 14));
    
    options->set_fontname(DICTOOL->getStringValue_json(optionsJson, "fontName", "微软雅黑"));
}

void ProtocolBuffersSerialize::setCheckBoxOptions(protocolbuffers::CheckBoxOptions *checkBoxOptions,
                                                  protocolbuffers::WidgetOptions *widgetOptions,
                                                  const rapidjson::Value &optionsJson)
{
    setWidgetOptions(widgetOptions, optionsJson);
    
    protocolbuffers::CheckBoxOptions* options = checkBoxOptions;
    
    static const char* P_BackGroundBoxData = "backGroundBoxData";
    static const char* P_BackGroundBoxSelectedData = "backGroundBoxSelectedData";
    static const char* P_FrontCrossData = "frontCrossData";
    static const char* P_BackGroundBoxDisabledData = "backGroundBoxDisabledData";
    static const char* P_FrontCrossDisabledData = "frontCrossDisabledData";
    
    
    int type = 0;
    std::string path = "", plistFile = "";
    
    //load background image
    bool backGroundDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_BackGroundBoxData);
    if (backGroundDicExist)
    {
        ResourceData* backGroundFileData = options->mutable_backgroundboxdata();
        const rapidjson::Value& backGroundDic = DICTOOL->getSubDictionary_json(optionsJson, P_BackGroundBoxData);
        type = DICTOOL->getIntValue_json(backGroundDic, "resourceType");
        path = DICTOOL->getStringValue_json(backGroundDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(backGroundDic, "plistFile", "");
        backGroundFileData->set_resourcetype(type);
        backGroundFileData->set_path(path);
        backGroundFileData->set_plistfile(plistFile);
    }
    
    //load background selected image
    bool backGroundSelectedDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_BackGroundBoxSelectedData);
    if (backGroundSelectedDicExist)
    {
        ResourceData* backGroundSelectedFileData = options->mutable_backgroundboxselecteddata();
        const rapidjson::Value& backGroundSelectedDic = DICTOOL->getSubDictionary_json(optionsJson, P_BackGroundBoxSelectedData);
        type = DICTOOL->getIntValue_json(backGroundSelectedDic, "resourceType");
        path = DICTOOL->getStringValue_json(backGroundSelectedDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(backGroundSelectedDic, "plistFile", "");
        backGroundSelectedFileData->set_resourcetype(type);
        backGroundSelectedFileData->set_path(path);
        backGroundSelectedFileData->set_plistfile(plistFile);
    }
    
    //load frontCross image
    bool frontCrossDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_FrontCrossData);
    if (frontCrossDicExist)
    {
        ResourceData* frontCrossFileData = options->mutable_frontcrossdata();
        const rapidjson::Value& frontCrossDic = DICTOOL->getSubDictionary_json(optionsJson, P_FrontCrossData);
        type = DICTOOL->getIntValue_json(frontCrossDic, "resourceType");
        path = DICTOOL->getStringValue_json(frontCrossDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(frontCrossDic, "plistFile", "");
        frontCrossFileData->set_resourcetype(type);
        frontCrossFileData->set_path(path);
        frontCrossFileData->set_plistfile(plistFile);
    }
    
    //load backGroundBoxDisabledData
    bool backGroundDisabledDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_BackGroundBoxDisabledData);
    if (backGroundDisabledDicExist)
    {
        ResourceData* backGroundDisabledFileData = options->mutable_backgroundboxdisableddata();
        const rapidjson::Value& backGroundDisabledDic = DICTOOL->getSubDictionary_json(optionsJson, P_BackGroundBoxDisabledData);
        type = DICTOOL->getIntValue_json(backGroundDisabledDic, "resourceType");
        path = DICTOOL->getStringValue_json(backGroundDisabledDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(backGroundDisabledDic, "plistFile", "");
        backGroundDisabledFileData->set_resourcetype(type);
        backGroundDisabledFileData->set_path(path);
        backGroundDisabledFileData->set_plistfile(plistFile);
    }
    
    ///load frontCrossDisabledData
    bool frontCrossDisabledDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_FrontCrossDisabledData);
    if (frontCrossDisabledDicExist)
    {
        ResourceData* frontCrossDisabledFileData = options->mutable_frontcrossdisableddata();
        const rapidjson::Value& frontCrossDisabledDic = DICTOOL->getSubDictionary_json(optionsJson, P_FrontCrossDisabledData);
        type = DICTOOL->getIntValue_json(frontCrossDisabledDic, "resourceType");
        path = DICTOOL->getStringValue_json(frontCrossDisabledDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(frontCrossDisabledDic, "plistFile", "");
        frontCrossDisabledFileData->set_resourcetype(type);
        frontCrossDisabledFileData->set_path(path);
        frontCrossDisabledFileData->set_plistfile(plistFile);
    }
    
    bool selectState = DICTOOL->getBooleanValue_json(optionsJson, "selectState");
    options->set_selectedstate(selectState);
}

void ProtocolBuffersSerialize::setImageViewOptions(protocolbuffers::ImageViewOptions *imageViewOptions,
                                                   protocolbuffers::WidgetOptions *widgetOptions,
                                                   const rapidjson::Value &optionsJson)
{
    setWidgetOptions(widgetOptions, optionsJson);
    
    protocolbuffers::ImageViewOptions* options = imageViewOptions;
    
    static const char* P_Scale9Enable = "scale9Enable";
    static const char* P_FileNameData = "fileNameData";
    static const char* P_CapInsetsX = "capInsetsX";
    static const char* P_CapInsetsY = "capInsetsY";
    static const char* P_CapInsetsWidth = "capInsetsWidth";
    static const char* P_CapInsetsHeight = "capInsetsHeight";
    static const char* P_Scale9Width = "scale9Width";
    static const char* P_Scale9Height = "scale9Height";
    
    int type = 0;
    std::string path = "", plistFile = "";
    
    bool imageFileNameDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_FileNameData);
    if (imageFileNameDicExist)
    {
        ResourceData* imageFileData = options->mutable_filenamedata();
        const rapidjson::Value& imageFileNameDic = DICTOOL->getSubDictionary_json(optionsJson, P_FileNameData);
        type = DICTOOL->getIntValue_json(imageFileNameDic, "resourceType");
        path = DICTOOL->getStringValue_json(imageFileNameDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(imageFileNameDic, "plistFile", "");
        imageFileData->set_resourcetype(type);
        imageFileData->set_path(path);
        imageFileData->set_plistfile(plistFile);
    }
    
    
    bool scale9EnableExist = DICTOOL->checkObjectExist_json(optionsJson, P_Scale9Enable);
    bool scale9Enable = false;
    if (scale9EnableExist)
    {
        scale9Enable = DICTOOL->getBooleanValue_json(optionsJson, P_Scale9Enable);
    }
    options->set_scale9enable(scale9Enable);
    
    
    if (scale9Enable)
    {
        float swf = DICTOOL->getFloatValue_json(optionsJson, P_Scale9Width, 80.0f);
        float shf = DICTOOL->getFloatValue_json(optionsJson, P_Scale9Height, 80.0f);
        options->set_scale9width(swf);
        options->set_scale9height(shf);
        
        float cx = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsX);
        float cy = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsY);
        float cw = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsWidth,1.0f);
        float ch = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsHeight,1.0f);
        options->set_capinsetsx(cx);
        options->set_capinsetsy(cy);
        options->set_capinsetswidth(cw);
        options->set_capinsetsheight(ch);
    }
}

void ProtocolBuffersSerialize::setTextAtlasOptions(protocolbuffers::TextAtlasOptions *textAtlasOptions,
                                                   protocolbuffers::WidgetOptions *widgetOptions,
                                                   const rapidjson::Value &optionsJson)
{
    setWidgetOptions(widgetOptions, optionsJson);
    
    protocolbuffers::TextAtlasOptions* options = textAtlasOptions;
    
    static const char* P_StringValue = "stringValue";
    static const char* P_CharMapFileData = "charMapFileData";
    static const char* P_ItemWidth = "itemWidth";
    static const char* P_ItemHeight = "itemHeight";
    static const char* P_StartCharMap = "startCharMap";
    
    int type = 0;
    std::string path = "", plistFile = "";
    
    bool cmftDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_CharMapFileData);
    if (cmftDicExist)
    {
        ResourceData* cmftFileData = options->mutable_charmapfiledata();
        const rapidjson::Value& cmftDic = DICTOOL->getSubDictionary_json(optionsJson, P_CharMapFileData);
        type = DICTOOL->getIntValue_json(cmftDic, "resourceType");
        path = DICTOOL->getStringValue_json(cmftDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(cmftDic, "plistFile", "");
        cmftFileData->set_resourcetype(type);
        cmftFileData->set_path(path);
        cmftFileData->set_plistfile(plistFile);
    }
    
    options->set_stringvalue(DICTOOL->getStringValue_json(optionsJson, P_StringValue, "12345678"));
    options->set_itemwidth(DICTOOL->getIntValue_json(optionsJson, P_ItemWidth, 24));
    options->set_itemheight(DICTOOL->getIntValue_json(optionsJson,P_ItemHeight, 32));
    options->set_startcharmap(DICTOOL->getStringValue_json(optionsJson, P_StartCharMap));
}

void ProtocolBuffersSerialize::setTextBMFontOptions(protocolbuffers::TextBMFontOptions *textBMFontOptions,
                                                    protocolbuffers::WidgetOptions *widgetOptions,
                                                    const rapidjson::Value &optionsJson)
{
    setWidgetOptions(widgetOptions, optionsJson);
    
    protocolbuffers::TextBMFontOptions* options = textBMFontOptions;
    
    static const char* P_FileNameData = "fileNameData";
    static const char* P_Text = "text";
    
    int type = 0;
    std::string path = "", plistFile = "";
    
    bool cmftDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_FileNameData);
    if (cmftDicExist)
    {
        ResourceData* cmftFileData = options->mutable_filenamedata();
        const rapidjson::Value& cmftDic = DICTOOL->getSubDictionary_json(optionsJson, P_FileNameData);
        type = DICTOOL->getIntValue_json(cmftDic, "resourceType");
        path = DICTOOL->getStringValue_json(cmftDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(cmftDic, "plistFile", "");
        cmftFileData->set_resourcetype(type);
        cmftFileData->set_path(path);
        cmftFileData->set_plistfile(plistFile);
    }
    
    const char* text = DICTOOL->getStringValue_json(optionsJson, P_Text,"Text Label");
    options->set_text(text);
}

void ProtocolBuffersSerialize::setTextOptions(protocolbuffers::TextOptions *textOptions,
                                              protocolbuffers::WidgetOptions *widgetOptions,
                                              const rapidjson::Value &optionsJson)
{
    setWidgetOptions(widgetOptions, optionsJson);
    
    protocolbuffers::TextOptions* options = textOptions;
    
    static const char* P_TouchScaleEnable = "touchScaleEnable";
    static const char* P_Text = "text";
    static const char* P_FontSize = "fontSize";
    static const char* P_FontName = "fontName";
    static const char* P_AreaWidth = "areaWidth";
    static const char* P_AreaHeight = "areaHeight";
    static const char* P_HAlignment = "hAlignment";
    static const char* P_VAlignment = "vAlignment";
    
    bool touchScaleChangeAble = DICTOOL->getBooleanValue_json(optionsJson, P_TouchScaleEnable);
    options->set_touchscaleenable(touchScaleChangeAble);
    
    const char* text = DICTOOL->getStringValue_json(optionsJson, P_Text,"Text Label");
    options->set_text(text);
    
    options->set_fontsize(DICTOOL->getIntValue_json(optionsJson, P_FontSize, 20));
    
    std::string fontName = DICTOOL->getStringValue_json(optionsJson, P_FontName, "微软雅黑");
    options->set_fontname(fontName);
    
    bool aw = DICTOOL->checkObjectExist_json(optionsJson, P_AreaWidth);
    bool ah = DICTOOL->checkObjectExist_json(optionsJson, P_AreaHeight);
    if (aw && ah)
    {
        options->set_areawidth(DICTOOL->getFloatValue_json(optionsJson, P_AreaWidth));
        options->set_areaheight(DICTOOL->getFloatValue_json(optionsJson, P_AreaHeight));
    }
    bool ha = DICTOOL->checkObjectExist_json(optionsJson, P_HAlignment);
    if (ha)
    {
        options->set_halignment(DICTOOL->getIntValue_json(optionsJson, P_HAlignment));
    }
    bool va = DICTOOL->checkObjectExist_json(optionsJson, P_VAlignment);
    if (va)
    {
        options->set_valignment(DICTOOL->getIntValue_json(optionsJson, P_VAlignment));
    }
}

void ProtocolBuffersSerialize::setLoadingBarOptions(protocolbuffers::LoadingBarOptions *loadingBarOptions,
                                                    protocolbuffers::WidgetOptions *widgetOptions,
                                                    const rapidjson::Value &optionsJson)
{
    setWidgetOptions(widgetOptions, optionsJson);
    
    protocolbuffers::LoadingBarOptions* options = loadingBarOptions;
    
    static const char* P_Scale9Enable = "scale9Enable";
    static const char* P_CapInsetsX = "capInsetsX";
    static const char* P_CapInsetsY = "capInsetsY";
    static const char* P_CapInsetsWidth = "capInsetsWidth";
    static const char* P_CapInsetsHeight = "capInsetsHeight";
    static const char* P_TextureData = "textureData";
    static const char* P_Direction = "direction";
    static const char* P_Percent = "percent";
    
    int type = 0;
    std::string path = "", plistFile = "";
    
    bool imageFileNameDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_TextureData);
    if (imageFileNameDicExist)
    {
        ResourceData* imageFileData = options->mutable_texturedata();
        const rapidjson::Value& imageFileNameDic = DICTOOL->getSubDictionary_json(optionsJson, P_TextureData);
        type = DICTOOL->getIntValue_json(imageFileNameDic, "resourceType");
        path = DICTOOL->getStringValue_json(imageFileNameDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(imageFileNameDic, "plistFile", "");
        imageFileData->set_resourcetype(type);
        imageFileData->set_path(path);
        imageFileData->set_plistfile(plistFile);
    }
    
    
    /* gui mark add load bar scale9 parse */
    bool scale9Enable = DICTOOL->getBooleanValue_json(optionsJson, P_Scale9Enable);
    options->set_scale9enable(scale9Enable);
    
    if (scale9Enable)
    {
        float cx = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsX);
        float cy = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsY);
        float cw = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsWidth, 0);
        float ch = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsHeight, 0);
        
        options->set_capinsetsx(cx);
        options->set_capinsetsy(cy);
        options->set_capinsetswidth(cw);
        options->set_capinsetsheight(ch);
        
        float scale9Width = DICTOOL->getFloatValue_json(optionsJson, "scale9Width");
        float scale9Height = DICTOOL->getFloatValue_json(optionsJson, "scale9Height");
        options->set_scale9width(scale9Width);
        options->set_scale9height(scale9Height);
    }
    /**/
    
    options->set_direction(DICTOOL->getIntValue_json(optionsJson, P_Direction));
    options->set_percent(DICTOOL->getIntValue_json(optionsJson, P_Percent, 100));
}

void ProtocolBuffersSerialize::setSliderOptions(protocolbuffers::SliderOptions *sliderOptions,
                                                protocolbuffers::WidgetOptions *widgetOptions,
                                                const rapidjson::Value &optionsJson)
{
    setWidgetOptions(widgetOptions, optionsJson);
    
    protocolbuffers::SliderOptions* options = sliderOptions;
    
    static const char* P_Scale9Enable = "scale9Enable";
    static const char* P_Percent = "percent";
    static const char* P_BarFileNameData = "barFileNameData";
    static const char* P_Length = "length";
    static const char* P_BallNormalData = "ballNormalData";
    static const char* P_BallPressedData = "ballPressedData";
    static const char* P_BallDisabledData = "ballDisabledData";
    static const char* P_ProgressBarData = "progressBarData";
    
    bool barTextureScale9EnableExist = DICTOOL->checkObjectExist_json(optionsJson, P_Scale9Enable);
    if (barTextureScale9EnableExist)
    {
        bool barTextureScale9Enable = DICTOOL->getBooleanValue_json(optionsJson, P_Scale9Enable);
        options->set_scale9enable(barTextureScale9Enable);
    }
    
    options->set_percent(DICTOOL->getIntValue_json(optionsJson, P_Percent));
    
    
    int type = 0;
    std::string path = "", plistFile = "";
    
    bool barFileNameDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_BarFileNameData);
    if (barFileNameDicExist)
    {
        ResourceData* barFileData = options->mutable_barfilenamedata();
        const rapidjson::Value& barFileNameDic = DICTOOL->getSubDictionary_json(optionsJson, P_BarFileNameData);
        type = DICTOOL->getIntValue_json(barFileNameDic, "resourceType");
        path = DICTOOL->getStringValue_json(barFileNameDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(barFileNameDic, "plistFile", "");
        barFileData->set_resourcetype(type);
        barFileData->set_path(path);
        barFileData->set_plistfile(plistFile);
    }
    
    //loading normal slider ball texture
    bool normalDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_BallNormalData);
    if (normalDicExist)
    {
        ResourceData* ballNormalFileData = options->mutable_ballnormaldata();
        const rapidjson::Value& normalDic = DICTOOL->getSubDictionary_json(optionsJson, P_BallNormalData);
        type = DICTOOL->getIntValue_json(normalDic, "resourceType");
        path = DICTOOL->getStringValue_json(normalDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(normalDic, "plistFile", "");
        ballNormalFileData->set_resourcetype(type);
        ballNormalFileData->set_path(path);
        ballNormalFileData->set_plistfile(plistFile);
    }
    
    
    //loading slider ball press texture
    bool pressedDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_BallPressedData);
    if (pressedDicExist)
    {
        ResourceData* ballPressedFileData = options->mutable_ballpresseddata();
        const rapidjson::Value& pressedDic = DICTOOL->getSubDictionary_json(optionsJson, P_BallPressedData);
        type = DICTOOL->getIntValue_json(pressedDic, "resourceType");
        path = DICTOOL->getStringValue_json(pressedDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(pressedDic, "plistFile", "");
        ballPressedFileData->set_resourcetype(type);
        ballPressedFileData->set_path(path);
        ballPressedFileData->set_plistfile(plistFile);
    }
    
    //loading silder ball disable texture
    bool disabledDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_BallDisabledData);
    if (disabledDicExist)
    {
        ResourceData* ballDisabledFileData = options->mutable_balldisableddata();
        const rapidjson::Value& disabledDic = DICTOOL->getSubDictionary_json(optionsJson, P_BallDisabledData);
        type = DICTOOL->getIntValue_json(disabledDic, "resourceType");
        path = DICTOOL->getStringValue_json(disabledDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(disabledDic, "plistFile", "");
        ballDisabledFileData->set_resourcetype(type);
        ballDisabledFileData->set_path(path);
        ballDisabledFileData->set_plistfile(plistFile);
    }
    
    //load slider progress texture
    bool progressBarDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_ProgressBarData);
    if (progressBarDicExist)
    {
        ResourceData* progressBarFileData = options->mutable_progressbardata();
        const rapidjson::Value& progressBarDic = DICTOOL->getSubDictionary_json(optionsJson, P_ProgressBarData);
        type = DICTOOL->getIntValue_json(progressBarDic, "resourceType");
        path = DICTOOL->getStringValue_json(progressBarDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(progressBarDic, "plistFile", "");
        progressBarFileData->set_resourcetype(type);
        progressBarFileData->set_path(path);
        progressBarFileData->set_plistfile(plistFile);
    }
}

void ProtocolBuffersSerialize::setTextFieldOptions(protocolbuffers::TextFieldOptions *textFieldOptions,
                                                   protocolbuffers::WidgetOptions *widgetOptions,
                                                   const rapidjson::Value &optionsJson)
{
    setWidgetOptions(widgetOptions, optionsJson);
    
    protocolbuffers::TextFieldOptions* options = textFieldOptions;
    
    static const char* P_PlaceHolder = "placeHolder";
    static const char* P_Text = "text";
    static const char* P_FontSize = "fontSize";
    static const char* P_FontName = "fontName";
    static const char* P_TouchSizeWidth = "touchSizeWidth";
    static const char* P_TouchSizeHeight = "touchSizeHeight";
    static const char* P_MaxLengthEnable = "maxLengthEnable";
    static const char* P_MaxLength = "maxLength";
    static const char* P_PasswordEnable = "passwordEnable";
    static const char* P_PasswordStyleText = "passwordStyleText";
    
    bool ph = DICTOOL->checkObjectExist_json(optionsJson, P_PlaceHolder);
    if (ph)
    {
        options->set_placeholder(DICTOOL->getStringValue_json(optionsJson, P_PlaceHolder, "input words here"));
    }
    
    options->set_text(DICTOOL->getStringValue_json(optionsJson, P_Text, "Text Tield"));
    
    options->set_fontsize(DICTOOL->getIntValue_json(optionsJson, P_FontSize, 20));
    
    options->set_fontname(DICTOOL->getStringValue_json(optionsJson, P_FontName, "微软雅黑"));
    
    bool maxLengthEnable = DICTOOL->getBooleanValue_json(optionsJson, P_MaxLengthEnable);
    options->set_maxlengthenable(maxLengthEnable);
    
    if (maxLengthEnable)
    {
        int maxLength = DICTOOL->getIntValue_json(optionsJson, P_MaxLength,10);
        options->set_maxlength(maxLength);
    }
    bool passwordEnable = DICTOOL->getBooleanValue_json(optionsJson, P_PasswordEnable);
    options->set_passwordenable(passwordEnable);
    if (passwordEnable)
    {
        options->set_passwordstyletext(DICTOOL->getStringValue_json(optionsJson, P_PasswordStyleText,"*"));
    }
}

void ProtocolBuffersSerialize::setLayoutOptions(protocolbuffers::PanelOptions *layoutOptions,
                                                protocolbuffers::WidgetOptions *widgetOptions,
                                                const rapidjson::Value &optionsJson)
{
    setWidgetOptions(widgetOptions, optionsJson);
    
    protocolbuffers::PanelOptions* options = layoutOptions;
    
    static const char* P_CapInsetsX = "capInsetsX";
    static const char* P_CapInsetsY = "capInsetsY";
    static const char* P_CapInsetsWidth = "capInsetsWidth";
    static const char* P_CapInsetsHeight = "capInsetsHeight";
    static const char* P_ClipAble = "clipAble";
    static const char* P_BackGroundScale9Enable = "backGroundScale9Enable";
    static const char* P_BgColorR = "bgColorR";
    static const char* P_BgColorG = "bgColorG";
    static const char* P_BgColorB = "bgColorB";
    static const char* P_BgStartColorR = "bgStartColorR";
    static const char* P_BgStartColorG = "bgStartColorG";
    static const char* P_BgStartColorB = "bgStartColorB";
    static const char* P_BgEndColorR = "bgEndColorR";
    static const char* P_BgEndColorG = "bgEndColorG";
    static const char* P_BgEndColorB = "bgEndColorB";
    static const char* P_VectorX = "vectorX";
    static const char* P_VectorY = "vectorY";
    static const char* P_BgColorOpacity = "bgColorOpacity";
    static const char* P_ColorType = "colorType";
    static const char* P_BackGroundImageData = "backGroundImageData";
    static const char* P_LayoutType = "layoutType";
    static const char* P_Scale9Width = "scale9Width";
    static const char* P_Scale9Height = "scale9Height";
    
    options->set_clipable(DICTOOL->getBooleanValue_json(optionsJson, P_ClipAble));
    
    bool backGroundScale9Enable = DICTOOL->getBooleanValue_json(optionsJson, P_BackGroundScale9Enable);
    options->set_backgroundscale9enable(backGroundScale9Enable);
    
    
    int cr;
    int cg;
    int cb;
    int scr;
    int scg;
    int scb;
    int ecr;
    int ecg;
    int ecb;
    
    cr = DICTOOL->getIntValue_json(optionsJson, "bgColorR",150);
    cg = DICTOOL->getIntValue_json(optionsJson, "bgColorG",200);
    cb = DICTOOL->getIntValue_json(optionsJson, "bgColorB",255);
    
    scr = DICTOOL->getIntValue_json(optionsJson, "bgStartColorR",255);
    scg = DICTOOL->getIntValue_json(optionsJson, "bgStartColorG",255);
    scb = DICTOOL->getIntValue_json(optionsJson, "bgStartColorB",255);
    
    ecr = DICTOOL->getIntValue_json(optionsJson, "bgEndColorR",150);
    ecg = DICTOOL->getIntValue_json(optionsJson, "bgEndColorG",200);
    ecb = DICTOOL->getIntValue_json(optionsJson, "bgEndColorB",255);
    
    float bgcv1 = DICTOOL->getFloatValue_json(optionsJson, P_VectorX);
    float bgcv2 = DICTOOL->getFloatValue_json(optionsJson, P_VectorY, -0.5f);
    options->set_vectorx(bgcv1);
    options->set_vectory(bgcv2);
    
    int co = DICTOOL->getIntValue_json(optionsJson, P_BgColorOpacity, 100);
    
    int colorType = DICTOOL->getIntValue_json(optionsJson, P_ColorType, 1);
    options->set_colortype(colorType);
    
    options->set_bgstartcolorr(scr);
    options->set_bgstartcolorg(scg);
    options->set_bgstartcolorb(scb);
    options->set_bgendcolorr(ecr);
    options->set_bgendcolorg(ecg);
    options->set_bgendcolorb(ecb);
    
    options->set_bgcolorr(cr);
    options->set_bgcolorg(cg);
    options->set_bgcolorb(cb);
    
    options->set_bgcoloropacity(co);
    
    bool imageFileNameDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_BackGroundImageData);
    if (imageFileNameDicExist)
    {
        int type = 0;
        std::string path = "", plistFile = "";
        
        ResourceData* backgroundimageFileData = options->mutable_backgroundimagedata();
        const rapidjson::Value& imageFileNameDic = DICTOOL->getSubDictionary_json(optionsJson, P_BackGroundImageData);
        type = DICTOOL->getIntValue_json(imageFileNameDic, "resourceType");
        path = DICTOOL->getStringValue_json(imageFileNameDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(imageFileNameDic, "plistFile", "");
        backgroundimageFileData->set_resourcetype(type);
        backgroundimageFileData->set_path(path);
        backgroundimageFileData->set_plistfile(plistFile);
    }
    
    
    if (backGroundScale9Enable)
    {
        float cx = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsX);
        float cy = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsY);
        float cw = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsWidth, 1);
        float ch = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsHeight, 1);
        options->set_capinsetsx(cx);
        options->set_capinsetsy(cy);
        options->set_capinsetswidth(cw);
        options->set_capinsetsheight(ch);
    }
    
    int bgimgcr = DICTOOL->getIntValue_json(optionsJson, "colorR", 255);
    int bgimgcg = DICTOOL->getIntValue_json(optionsJson, "colorG", 255);
    int bgimgcb = DICTOOL->getIntValue_json(optionsJson, "colorB", 255);
    widgetOptions->set_colorr(bgimgcr);
    widgetOptions->set_colorg(bgimgcg);
    widgetOptions->set_colorb(bgimgcb);
    
    int bgimgopacity = DICTOOL->getIntValue_json(optionsJson, "opacity",255);
    widgetOptions->set_opacity(bgimgopacity);
}

void ProtocolBuffersSerialize::setPageViewOptions(protocolbuffers::PageViewOptions *pageViewOptions,
                                                  protocolbuffers::WidgetOptions *widgetOptions,
                                                  const rapidjson::Value &optionsJson)
{
    setWidgetOptions(widgetOptions, optionsJson);
    
    protocolbuffers::PageViewOptions* options = pageViewOptions;
    
    static const char* P_CapInsetsX = "capInsetsX";
    static const char* P_CapInsetsY = "capInsetsY";
    static const char* P_CapInsetsWidth = "capInsetsWidth";
    static const char* P_CapInsetsHeight = "capInsetsHeight";
    static const char* P_ClipAble = "clipAble";
    static const char* P_BackGroundScale9Enable = "backGroundScale9Enable";
    static const char* P_BgColorR = "bgColorR";
    static const char* P_BgColorG = "bgColorG";
    static const char* P_BgColorB = "bgColorB";
    static const char* P_BgStartColorR = "bgStartColorR";
    static const char* P_BgStartColorG = "bgStartColorG";
    static const char* P_BgStartColorB = "bgStartColorB";
    static const char* P_BgEndColorR = "bgEndColorR";
    static const char* P_BgEndColorG = "bgEndColorG";
    static const char* P_BgEndColorB = "bgEndColorB";
    static const char* P_VectorX = "vectorX";
    static const char* P_VectorY = "vectorY";
    static const char* P_BgColorOpacity = "bgColorOpacity";
    static const char* P_ColorType = "colorType";
    static const char* P_BackGroundImageData = "backGroundImageData";
    static const char* P_LayoutType = "layoutType";
    static const char* P_Scale9Width = "scale9Width";
    static const char* P_Scale9Height = "scale9Height";
    
    options->set_clipable(DICTOOL->getBooleanValue_json(optionsJson, P_ClipAble));
    
    bool backGroundScale9Enable = DICTOOL->getBooleanValue_json(optionsJson, P_BackGroundScale9Enable);
    options->set_backgroundscale9enable(backGroundScale9Enable);
    
    
    int cr;
    int cg;
    int cb;
    int scr;
    int scg;
    int scb;
    int ecr;
    int ecg;
    int ecb;
    
    cr = DICTOOL->getIntValue_json(optionsJson, "bgColorR",150);
    cg = DICTOOL->getIntValue_json(optionsJson, "bgColorG",150);
    cb = DICTOOL->getIntValue_json(optionsJson, "bgColorB",100);
    
    scr = DICTOOL->getIntValue_json(optionsJson, "bgStartColorR",255);
    scg = DICTOOL->getIntValue_json(optionsJson, "bgStartColorG",255);
    scb = DICTOOL->getIntValue_json(optionsJson, "bgStartColorB",255);
    
    ecr = DICTOOL->getIntValue_json(optionsJson, "bgEndColorR",255);
    ecg = DICTOOL->getIntValue_json(optionsJson, "bgEndColorG",150);
    ecb = DICTOOL->getIntValue_json(optionsJson, "bgEndColorB",100);
    
    float bgcv1 = DICTOOL->getFloatValue_json(optionsJson, P_VectorX);
    float bgcv2 = DICTOOL->getFloatValue_json(optionsJson, P_VectorY, -0.5f);
    options->set_vectorx(bgcv1);
    options->set_vectory(bgcv2);
    
    int co = DICTOOL->getIntValue_json(optionsJson, P_BgColorOpacity, 100);
    
    int colorType = DICTOOL->getIntValue_json(optionsJson, P_ColorType, 1);
    options->set_colortype(colorType);
    
    options->set_bgstartcolorr(scr);
    options->set_bgstartcolorg(scg);
    options->set_bgstartcolorb(scb);
    options->set_bgendcolorr(ecr);
    options->set_bgendcolorg(ecg);
    options->set_bgendcolorb(ecb);
    
    options->set_bgcolorr(cr);
    options->set_bgcolorg(cg);
    options->set_bgcolorb(cb);
    
    options->set_bgcoloropacity(co);
    
    
    bool imageFileNameDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_BackGroundImageData);
    if (imageFileNameDicExist)
    {
        int type = 0;
        std::string path = "", plistFile = "";
        
        ResourceData* backgroundimageFileData = options->mutable_backgroundimagedata();
        const rapidjson::Value& imageFileNameDic = DICTOOL->getSubDictionary_json(optionsJson, P_BackGroundImageData);
        type = DICTOOL->getIntValue_json(imageFileNameDic, "resourceType");
        path = DICTOOL->getStringValue_json(imageFileNameDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(imageFileNameDic, "plistFile", "");
        backgroundimageFileData->set_resourcetype(type);
        backgroundimageFileData->set_path(path);
        backgroundimageFileData->set_plistfile(plistFile);
    }
    
    
    if (backGroundScale9Enable)
    {
        float cx = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsX);
        float cy = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsY);
        float cw = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsWidth, 1);
        float ch = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsHeight, 1);
        options->set_capinsetsx(cx);
        options->set_capinsetsy(cy);
        options->set_capinsetswidth(cw);
        options->set_capinsetsheight(ch);
    }
    
    int bgimgcr = DICTOOL->getIntValue_json(optionsJson, "colorR", 255);
    int bgimgcg = DICTOOL->getIntValue_json(optionsJson, "colorG", 255);
    int bgimgcb = DICTOOL->getIntValue_json(optionsJson, "colorB", 255);
    widgetOptions->set_colorr(bgimgcr);
    widgetOptions->set_colorg(bgimgcg);
    widgetOptions->set_colorb(bgimgcb);
    
    int bgimgopacity = DICTOOL->getIntValue_json(optionsJson, "opacity",255);
    widgetOptions->set_opacity(bgimgopacity);
}

void ProtocolBuffersSerialize::setScrollViewOptions(protocolbuffers::ScrollViewOptions *scrollViewOptions,
                                                    protocolbuffers::WidgetOptions *widgetOptions,
                                                    const rapidjson::Value &optionsJson)
{
    setWidgetOptions(widgetOptions, optionsJson);
    
    protocolbuffers::ScrollViewOptions* options = scrollViewOptions;
    
    static const char* P_CapInsetsX = "capInsetsX";
    static const char* P_CapInsetsY = "capInsetsY";
    static const char* P_CapInsetsWidth = "capInsetsWidth";
    static const char* P_CapInsetsHeight = "capInsetsHeight";
    static const char* P_ClipAble = "clipAble";
    static const char* P_BackGroundScale9Enable = "backGroundScale9Enable";
    static const char* P_BgColorR = "bgColorR";
    static const char* P_BgColorG = "bgColorG";
    static const char* P_BgColorB = "bgColorB";
    static const char* P_BgStartColorR = "bgStartColorR";
    static const char* P_BgStartColorG = "bgStartColorG";
    static const char* P_BgStartColorB = "bgStartColorB";
    static const char* P_BgEndColorR = "bgEndColorR";
    static const char* P_BgEndColorG = "bgEndColorG";
    static const char* P_BgEndColorB = "bgEndColorB";
    static const char* P_VectorX = "vectorX";
    static const char* P_VectorY = "vectorY";
    static const char* P_BgColorOpacity = "bgColorOpacity";
    static const char* P_ColorType = "colorType";
    static const char* P_BackGroundImageData = "backGroundImageData";
    static const char* P_LayoutType = "layoutType";
    static const char* P_Scale9Width = "scale9Width";
    static const char* P_Scale9Height = "scale9Height";
    
    static const char* P_InnerWidth = "innerWidth";
    static const char* P_InnerHeight = "innerHeight";
    static const char* P_Direction = "direction";
    static const char* P_BounceEnable = "bounceEnable";
    
    options->set_clipable(DICTOOL->getBooleanValue_json(optionsJson, P_ClipAble));
    
    bool backGroundScale9Enable = DICTOOL->getBooleanValue_json(optionsJson, P_BackGroundScale9Enable);
    options->set_backgroundscale9enable(backGroundScale9Enable);
    
    
    int cr;
    int cg;
    int cb;
    int scr;
    int scg;
    int scb;
    int ecr;
    int ecg;
    int ecb;
    
    cr = DICTOOL->getIntValue_json(optionsJson, "bgColorR",255);
    cg = DICTOOL->getIntValue_json(optionsJson, "bgColorG",150);
    cb = DICTOOL->getIntValue_json(optionsJson, "bgColorB",100);
    
    scr = DICTOOL->getIntValue_json(optionsJson, "bgStartColorR",255);
    scg = DICTOOL->getIntValue_json(optionsJson, "bgStartColorG",255);
    scb = DICTOOL->getIntValue_json(optionsJson, "bgStartColorB",255);
    
    ecr = DICTOOL->getIntValue_json(optionsJson, "bgEndColorR",255);
    ecg = DICTOOL->getIntValue_json(optionsJson, "bgEndColorG",150);
    ecb = DICTOOL->getIntValue_json(optionsJson, "bgEndColorB",100);
    
    float bgcv1 = DICTOOL->getFloatValue_json(optionsJson, P_VectorX);
    float bgcv2 = DICTOOL->getFloatValue_json(optionsJson, P_VectorY, -0.5f);
    options->set_vectorx(bgcv1);
    options->set_vectory(bgcv2);
    
    int co = DICTOOL->getIntValue_json(optionsJson, P_BgColorOpacity, 100);
    
    int colorType = DICTOOL->getIntValue_json(optionsJson, P_ColorType, 1);
    options->set_colortype(colorType);
    
    options->set_bgstartcolorr(scr);
    options->set_bgstartcolorg(scg);
    options->set_bgstartcolorb(scb);
    options->set_bgendcolorr(ecr);
    options->set_bgendcolorg(ecg);
    options->set_bgendcolorb(ecb);
    
    options->set_bgcolorr(cr);
    options->set_bgcolorg(cg);
    options->set_bgcolorb(cb);
    
    options->set_bgcoloropacity(co);
    
    
    bool imageFileNameDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_BackGroundImageData);
    if (imageFileNameDicExist)
    {
        int type = 0;
        std::string path = "", plistFile = "";
        
        ResourceData* backgroundimageFileData = options->mutable_backgroundimagedata();
        const rapidjson::Value& imageFileNameDic = DICTOOL->getSubDictionary_json(optionsJson, P_BackGroundImageData);
        type = DICTOOL->getIntValue_json(imageFileNameDic, "resourceType");
        path = DICTOOL->getStringValue_json(imageFileNameDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(imageFileNameDic, "plistFile", "");
        backgroundimageFileData->set_resourcetype(type);
        backgroundimageFileData->set_path(path);
        backgroundimageFileData->set_plistfile(plistFile);
    }
    
    
    if (backGroundScale9Enable)
    {
        float cx = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsX);
        float cy = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsY);
        float cw = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsWidth, 1);
        float ch = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsHeight, 1);
        options->set_capinsetsx(cx);
        options->set_capinsetsy(cy);
        options->set_capinsetswidth(cw);
        options->set_capinsetsheight(ch);
    }
    
    int bgimgcr = DICTOOL->getIntValue_json(optionsJson, "colorR", 255);
    int bgimgcg = DICTOOL->getIntValue_json(optionsJson, "colorG", 255);
    int bgimgcb = DICTOOL->getIntValue_json(optionsJson, "colorB", 255);
    widgetOptions->set_colorr(bgimgcr);
    widgetOptions->set_colorg(bgimgcg);
    widgetOptions->set_colorb(bgimgcb);
    
    int bgimgopacity = DICTOOL->getIntValue_json(optionsJson, "opacity",255);
    widgetOptions->set_opacity(bgimgopacity);
    
    float innerWidth = DICTOOL->getFloatValue_json(optionsJson, P_InnerWidth,200);
    float innerHeight = DICTOOL->getFloatValue_json(optionsJson, P_InnerHeight,200);
    options->set_innerwidth(innerWidth);
    options->set_innerheight(innerHeight);
    
    int direction = DICTOOL->getFloatValue_json(optionsJson, P_Direction,1);
    options->set_direction(direction);
    
    options->set_bounceenable(DICTOOL->getBooleanValue_json(optionsJson, P_BounceEnable));
}

void ProtocolBuffersSerialize::setListViewOptions(protocolbuffers::ListViewOptions *listViewOptions,
                                                  protocolbuffers::WidgetOptions *widgetOptions,
                                                  const rapidjson::Value &optionsJson)
{
    setWidgetOptions(widgetOptions, optionsJson);
    
    protocolbuffers::ListViewOptions* options = listViewOptions;
    
    static const char* P_CapInsetsX = "capInsetsX";
    static const char* P_CapInsetsY = "capInsetsY";
    static const char* P_CapInsetsWidth = "capInsetsWidth";
    static const char* P_CapInsetsHeight = "capInsetsHeight";
    static const char* P_ClipAble = "clipAble";
    static const char* P_BackGroundScale9Enable = "backGroundScale9Enable";
    static const char* P_BgColorR = "bgColorR";
    static const char* P_BgColorG = "bgColorG";
    static const char* P_BgColorB = "bgColorB";
    static const char* P_BgStartColorR = "bgStartColorR";
    static const char* P_BgStartColorG = "bgStartColorG";
    static const char* P_BgStartColorB = "bgStartColorB";
    static const char* P_BgEndColorR = "bgEndColorR";
    static const char* P_BgEndColorG = "bgEndColorG";
    static const char* P_BgEndColorB = "bgEndColorB";
    static const char* P_VectorX = "vectorX";
    static const char* P_VectorY = "vectorY";
    static const char* P_BgColorOpacity = "bgColorOpacity";
    static const char* P_ColorType = "colorType";
    static const char* P_BackGroundImageData = "backGroundImageData";
    static const char* P_LayoutType = "layoutType";
    static const char* P_Scale9Width = "scale9Width";
    static const char* P_Scale9Height = "scale9Height";
    
    static const char* P_InnerWidth = "innerWidth";
    static const char* P_InnerHeight = "innerHeight";
    static const char* P_Direction = "direction";
    static const char* P_BounceEnable = "bounceEnable";
    
    static const char* P_ItemMargin = "itemMargin";
    
    options->set_clipable(DICTOOL->getBooleanValue_json(optionsJson, P_ClipAble));
    
    bool backGroundScale9Enable = DICTOOL->getBooleanValue_json(optionsJson, P_BackGroundScale9Enable);
    options->set_backgroundscale9enable(backGroundScale9Enable);
    
    
    int cr;
    int cg;
    int cb;
    int scr;
    int scg;
    int scb;
    int ecr;
    int ecg;
    int ecb;
    
    cr = DICTOOL->getIntValue_json(optionsJson, "bgColorR",150);
    cg = DICTOOL->getIntValue_json(optionsJson, "bgColorG",200);
    cb = DICTOOL->getIntValue_json(optionsJson, "bgColorB",255);
    
    scr = DICTOOL->getIntValue_json(optionsJson, "bgStartColorR",255);
    scg = DICTOOL->getIntValue_json(optionsJson, "bgStartColorG",255);
    scb = DICTOOL->getIntValue_json(optionsJson, "bgStartColorB",255);
    
    ecr = DICTOOL->getIntValue_json(optionsJson, "bgEndColorR",150);
    ecg = DICTOOL->getIntValue_json(optionsJson, "bgEndColorG",200);
    ecb = DICTOOL->getIntValue_json(optionsJson, "bgEndColorB",255);
    
    float bgcv1 = DICTOOL->getFloatValue_json(optionsJson, P_VectorX);
    float bgcv2 = DICTOOL->getFloatValue_json(optionsJson, P_VectorY, -0.5f);
    options->set_vectorx(bgcv1);
    options->set_vectory(bgcv2);
    
    int co = DICTOOL->getIntValue_json(optionsJson, P_BgColorOpacity, 100);
    
    int colorType = DICTOOL->getIntValue_json(optionsJson, P_ColorType, 1);
    options->set_colortype(colorType);
    
    options->set_bgstartcolorr(scr);
    options->set_bgstartcolorg(scg);
    options->set_bgstartcolorb(scb);
    options->set_bgendcolorr(ecr);
    options->set_bgendcolorg(ecg);
    options->set_bgendcolorb(ecb);
    
    options->set_bgcolorr(cr);
    options->set_bgcolorg(cg);
    options->set_bgcolorb(cb);
    
    options->set_bgcoloropacity(co);
    
    
    bool imageFileNameDicExist = DICTOOL->checkObjectExist_json(optionsJson, P_BackGroundImageData);
    if (imageFileNameDicExist)
    {
        int type = 0;
        std::string path = "", plistFile = "";
        
        ResourceData* backgroundimageFileData = options->mutable_backgroundimagedata();
        const rapidjson::Value& imageFileNameDic = DICTOOL->getSubDictionary_json(optionsJson, P_BackGroundImageData);
        type = DICTOOL->getIntValue_json(imageFileNameDic, "resourceType");
        path = DICTOOL->getStringValue_json(imageFileNameDic, "path", "");
        plistFile = DICTOOL->getStringValue_json(imageFileNameDic, "plistFile", "");
        backgroundimageFileData->set_resourcetype(type);
        backgroundimageFileData->set_path(path);
        backgroundimageFileData->set_plistfile(plistFile);
    }
    
    
    if (backGroundScale9Enable)
    {
        float cx = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsX);
        float cy = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsY);
        float cw = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsWidth, 1);
        float ch = DICTOOL->getFloatValue_json(optionsJson, P_CapInsetsHeight, 1);
        options->set_capinsetsx(cx);
        options->set_capinsetsy(cy);
        options->set_capinsetswidth(cw);
        options->set_capinsetsheight(ch);
    }
    
    int bgimgcr = DICTOOL->getIntValue_json(optionsJson, "colorR", 255);
    int bgimgcg = DICTOOL->getIntValue_json(optionsJson, "colorG", 255);
    int bgimgcb = DICTOOL->getIntValue_json(optionsJson, "colorB", 255);
    widgetOptions->set_colorr(bgimgcr);
    widgetOptions->set_colorg(bgimgcg);
    widgetOptions->set_colorb(bgimgcb);
    
    int bgimgopacity = DICTOOL->getIntValue_json(optionsJson, "opacity",255);
    widgetOptions->set_opacity(bgimgopacity);
    
    float innerWidth = DICTOOL->getFloatValue_json(optionsJson, P_InnerWidth,200);
    float innerHeight = DICTOOL->getFloatValue_json(optionsJson, P_InnerHeight,200);
    options->set_innerwidth(innerWidth);
    options->set_innerheight(innerHeight);
    
    int direction = DICTOOL->getFloatValue_json(optionsJson, P_Direction,1);
    options->set_direction(direction);
    
    options->set_bounceenable(DICTOOL->getBooleanValue_json(optionsJson, P_BounceEnable));
    
    options->set_gravity(DICTOOL->getIntValue_json(optionsJson, "gravity", 3));
    
    float itemMargin = DICTOOL->getFloatValue_json(optionsJson, P_ItemMargin);
    options->set_itemmargin(itemMargin);
}

void ProtocolBuffersSerialize::convertActionProtocolBuffersWithJson(protocolbuffers::NodeAction *nodeAction,
                                                                    const rapidjson::Value &json)
{
    int duration = DICTOOL->getIntValue_json(json, "duration");
    nodeAction->set_duration(duration);
    
    int timeSpeed = DICTOOL->getFloatValue_json(json, "speed", 1.0f);
    nodeAction->set_speed(timeSpeed);
    
    int timelineLength = DICTOOL->getArrayCount_json(json, "timelines");
    for (int i = 0; i < timelineLength; i++)
    {
        const rapidjson::Value& dic = DICTOOL->getSubDictionary_json(json, "timelines", i);
        protocolbuffers::TimeLine* timeLine = nodeAction->add_timelines();
        convertTimelineProtocolBuffers(timeLine, dic);
    }
}

void ProtocolBuffersSerialize::convertTimelineProtocolBuffers(protocolbuffers::TimeLine *timeLine,
                                                              const rapidjson::Value &json)
{
    // get frame type
    const char* frameTypeTemp = DICTOOL->getStringValue_json(json, "frameType");
	if (frameTypeTemp == NULL)
        return;
    
    if (frameTypeTemp)
    {
        timeLine->set_frametype(frameTypeTemp);
        
        int actionTag = DICTOOL->getIntValue_json(json, "actionTag");
        timeLine->set_actiontag(actionTag);
        
        
        int length = DICTOOL->getArrayCount_json(json, "frames");
        for (int i = 0; i < length; i++)
        {
            const rapidjson::Value& dic = DICTOOL->getSubDictionary_json(json, "frames", i);
            
            protocolbuffers::Frame* frame = timeLine->add_frames();
            
            std::string frameType = frameTypeTemp;
            
            if (frameType == "VisibleFrame")
            {
                TimeLineBoolFrame* visibleFrame = frame->mutable_visibleframe();
                setVisibleFrame(visibleFrame, dic);
            }
            else if (frameType == "PositionFrame")
            {
                TimeLinePointFrame* positionFrame = frame->mutable_positionframe();
                setPositionFrame(positionFrame, dic);
            }
            else if (frameType == "ScaleFrame")
            {
                TimeLinePointFrame* scaleFrame = frame->mutable_scaleframe();
                setScaleFrame(scaleFrame, dic);
            }
            else if (frameType == "RotationSkewFrame")
            {
                TimeLinePointFrame* rotationSkewFrame = frame->mutable_rotationskewframe();
                setRotationSkewFrame(rotationSkewFrame, dic);
            }
            else if (frameType == "AnchorFrame")
            {
                TimeLinePointFrame* anchorFrame = frame->mutable_anchorpointframe();
                setAnchorFrame(anchorFrame, dic);
            }
            else if (frameType == "ColorFrame")
            {
                TimeLineColorFrame* colorFrame = frame->mutable_colorframe();
                setColorFrame(colorFrame, dic);
            }
            else if (frameType == "TextureFrame")
            {
                TimeLineTextureFrame* textureFrame = frame->mutable_textureframe();
                setTextureFrame(textureFrame, dic);
            }
            else if (frameType == "EventFrame")
            {
                TimeLineStringFrame* eventFrame = frame->mutable_eventframe();
                setEventFrame(eventFrame, dic);
            }
            else if (frameType == "ZOrderFrame")
            {
                TimeLineIntFrame* zOrderFrame = frame->mutable_zorderframe();
                setZOrderFrame(zOrderFrame, dic);
            }
        }
    }
}

void ProtocolBuffersSerialize::setVisibleFrame(protocolbuffers::TimeLineBoolFrame *visibleFrame,
                                               const rapidjson::Value &json)
{
    bool visible = DICTOOL->getBooleanValue_json(json, "value");
    visibleFrame->set_value(visible);
    
    int frameIndex = DICTOOL->getIntValue_json(json, "frameIndex");
    visibleFrame->set_frameindex(frameIndex);
    
    bool tween = DICTOOL->getBooleanValue_json(json, "tween", false);
    visibleFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setPositionFrame(protocolbuffers::TimeLinePointFrame *positionFrame,
                                                const rapidjson::Value &json)
{
    float x = DICTOOL->getFloatValue_json(json, "x");
    float y = DICTOOL->getFloatValue_json(json, "y");
    positionFrame->set_x(x);
    positionFrame->set_y(y);
    
    int frameIndex = DICTOOL->getIntValue_json(json, "frameIndex");
    positionFrame->set_frameindex(frameIndex);
    
    bool tween = DICTOOL->getBooleanValue_json(json, "tween", false);
    positionFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setScaleFrame(protocolbuffers::TimeLinePointFrame *scaleFrame,
                                             const rapidjson::Value &json)
{
    float scalex = DICTOOL->getFloatValue_json(json, "x");
    float scaley = DICTOOL->getFloatValue_json(json, "y");
    scaleFrame->set_x(scalex);
    scaleFrame->set_y(scaley);
    
    int frameIndex = DICTOOL->getIntValue_json(json, "frameIndex");
    scaleFrame->set_frameindex(frameIndex);
    
    bool tween = DICTOOL->getBooleanValue_json(json, "tween", false);
    scaleFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setRotationSkewFrame(protocolbuffers::TimeLinePointFrame *rotationSkewFrame,
                                                    const rapidjson::Value &json)
{
    float skewx = DICTOOL->getFloatValue_json(json, "x");
    float skewy = DICTOOL->getFloatValue_json(json, "y");
    rotationSkewFrame->set_x(skewx);
    rotationSkewFrame->set_y(skewy);
    
    int frameIndex = DICTOOL->getIntValue_json(json, "frameIndex");
    rotationSkewFrame->set_frameindex(frameIndex);
    
    bool tween = DICTOOL->getBooleanValue_json(json, "tween", false);
    rotationSkewFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setAnchorFrame(protocolbuffers::TimeLinePointFrame *anchorPointframe,
                                              const rapidjson::Value &json)
{
    float anchorx = DICTOOL->getFloatValue_json(json, "x");
    float anchory = DICTOOL->getFloatValue_json(json, "y");
    anchorPointframe->set_x(anchorx);
    anchorPointframe->set_y(anchory);
    
    int frameIndex = DICTOOL->getIntValue_json(json, "frameIndex");
    anchorPointframe->set_frameindex(frameIndex);
    
    bool tween = DICTOOL->getBooleanValue_json(json, "tween", false);
    anchorPointframe->set_tween(tween);
}

void ProtocolBuffersSerialize::setColorFrame(protocolbuffers::TimeLineColorFrame *colorFrame,
                                             const rapidjson::Value &json)
{
    int alpha = (GLubyte)DICTOOL->getIntValue_json(json, "alpha");
    int red   = (GLubyte)DICTOOL->getIntValue_json(json, "red");
    int green = (GLubyte)DICTOOL->getIntValue_json(json, "green");
    int blue  = (GLubyte)DICTOOL->getIntValue_json(json, "blue");
    colorFrame->set_alpha(alpha);
    colorFrame->set_red(red);
    colorFrame->set_green(green);
    colorFrame->set_blue(blue);
    
    int frameIndex = DICTOOL->getIntValue_json(json, "frameIndex");
    colorFrame->set_frameindex(frameIndex);
    
    bool tween = DICTOOL->getBooleanValue_json(json, "tween", false);
    colorFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setTextureFrame(protocolbuffers::TimeLineTextureFrame *textureFrame,
                                               const rapidjson::Value &json)
{
    const char* texture = DICTOOL->getStringValue_json(json, "value");
    if (texture != NULL)
        textureFrame->set_name(texture);
    
    int frameIndex = DICTOOL->getIntValue_json(json, "frameIndex");
    textureFrame->set_frameindex(frameIndex);
    
    bool tween = DICTOOL->getBooleanValue_json(json, "tween", false);
    textureFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setEventFrame(protocolbuffers::TimeLineStringFrame *eventFrame,
                                             const rapidjson::Value &json)
{
    const char* evnt = DICTOOL->getStringValue_json(json, "value");
    
    if(evnt != NULL)
        eventFrame->set_value(evnt);
    
    int frameIndex = DICTOOL->getIntValue_json(json, "frameIndex");
    eventFrame->set_frameindex(frameIndex);
    
    bool tween = DICTOOL->getBooleanValue_json(json, "tween", false);
    eventFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setZOrderFrame(protocolbuffers::TimeLineIntFrame *zorderFrame,
                                              const rapidjson::Value &json)
{
    int zorder = DICTOOL->getIntValue_json(json, "value");
    zorderFrame->set_value(zorder);
    
    int frameIndex = DICTOOL->getIntValue_json(json, "frameIndex");
    zorderFrame->set_frameindex(frameIndex);
    
    bool tween = DICTOOL->getBooleanValue_json(json, "tween", false);
    zorderFrame->set_tween(tween);
}
/**/
/**/

/*
void ProtocolBuffersSerialize::serializeProtocolBuffersWithFile(const std::string &fileName)
{
    using namespace cocostudio::timeline;
 
 
    Node* root = NodeReader::getInstance()->createNode(fileName);
//    Layout* root = static_cast<Layout*>(reader->temp(fileName));

    CSParseBinary protobuf;
//    GUIProtocolBuffersProtobuf protobuf;
    
    // base property
    protobuf.set_classname(reader->getClassname());
    protobuf.set_name(reader->getName());
    protobuf.set_datascale(reader->getDataScale());
    protobuf.set_designheight(reader->getDesignHeight());
    protobuf.set_designwidth(reader->getDesignWidth());
    protobuf.set_version(reader->getVersion());
    
    
    // textures, texturesPng
    std::vector<std::string>::iterator iter;
    
    std::vector<std::string> textures = reader->getTextures();
    for (iter = textures.begin(); iter != textures.end(); ++iter)
    {
        protobuf.add_textures(*iter);
    }
    
    std::vector<std::string> texturesPng = reader->getTexturesPng();
    for (iter = texturesPng.begin(); iter != texturesPng.end(); ++iter)
    {
        std::string pngFile = *iter;
        protobuf.add_texturespng(*iter);
    }
    
    // nodeTree
    protocolbuffers::NodeTree* nodeTreeRoot = protobuf.mutable_nodetree();
    convertNodeTreeProtocolBuffersFromNode(nodeTreeRoot, root, "Node");
    
    // action
    protocolbuffers::NodeAction* action = protobuf.mutable_action();
//    convertActionProtocolBuffersWithFile(action, fileName);
    
    
    // out, in for stream
    std::fstream out("/Users/pipu/Public/space/excersize/cocos2d-x_test_protobuf/cocos2d-x_test_protobuf_mono_actiontimeline/cocos2d-x/projects/ProtobufTest/Resources/binary_from_mono_gggg_14/gui-protocol-buffers", std::ios::out | std::ios::binary | std::ios::trunc);
//    protobuf.SerializeToOstream(&out);
    out.close();
}
 */

#pragma mark -
#pragma mark serialize protocol buffers with node
#pragma mark -

/* serialize protocol buffers from Node*/
void ProtocolBuffersSerialize::serializeProtocolBuffersFileWithNode(const std::string &outFileName,
                                                                    cocos2d::Node *node,
                                                                    cocostudio::timeline::ActionTimeline *action,
                                                                    const EtcProperties *etc)
{
    using namespace cocostudio::timeline;
    
    Node* root = node;
    
    CSParseBinary protobuf;
    
    // base property
    protobuf.set_version(etc->getVersion());
    protobuf.set_cocos2dversion(etc->getCocos2dVersion());
    protobuf.set_editortype(etc->getEditorType());
    protobuf.set_designwidth(etc->getDesignWidth());
    protobuf.set_designheight(etc->getDesignHeight());
    protobuf.set_datascale(etc->getDataScale());
    
    
    std::vector<std::string>::iterator iter;
    
    std::vector<std::string> textures = etc->getTextures();
    for (iter = textures.begin(); iter != textures.end(); ++iter)
    {
        protobuf.add_textures(*iter);
    }
    
    std::vector<std::string> texturesPng = etc->getTexturePngs();
    for (iter = texturesPng.begin(); iter != texturesPng.end(); ++iter)
    {
        std::string pngFile = *iter;
        protobuf.add_texturespng(*iter);
    }
    //
    
    // nodeTree
    protocolbuffers::NodeTree* nodeTreeRoot = protobuf.mutable_nodetree();
    convertNodeTreeProtocolBuffersFromNode(nodeTreeRoot, root, "Node");
    
    // action
    protocolbuffers::NodeAction* nodeAction = protobuf.mutable_action();
    convertActionProtocolBuffersWithNode(nodeAction, action);
    
    
    // out, in for stream
    const char* temp = outFileName.c_str();
//    FILE* file = fopen(outFileName.c_str(), "wb, ccs = UNICODE");
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
    
//    std::fstream out(outFileName, std::ios::out | std::ios::binary | std::ios::trunc);
//    protobuf.SerializeToOstream(&out);
//    out.close();
}

void ProtocolBuffersSerialize::convertNodeTreeProtocolBuffersFromNode(protocolbuffers::NodeTree *nodetree,
                                                                      cocos2d::Node *node,
                                                                      std::string classname)
{
    nodetree->set_classname(classname);
    
    WidgetOptions* curOptions = nullptr;
    
    if (classname == "Node")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        setNodeOptions(nodeOptions, node);
        
        curOptions = nodeOptions;
    }
    else if (classname == "Sprite")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        SpriteOptions* options = nodetree->mutable_spriteoptions();
        setSpriteOptions(options, nodeOptions, node);
        
        curOptions = nodeOptions;
    }
    else if (classname == "TMXTiledMap")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        TMXTiledMapOptions* options = nodetree->mutable_tmxtiledmapoptions();
        setTMXTiledMapOptions(options, nodeOptions, node);
        
        curOptions = nodeOptions;
    }
    else if (classname == "ParticleSystem")
    {
        WidgetOptions* nodeOptions = nodetree->mutable_widgetoptions();
        ParticleSystemOptions* options = nodetree->mutable_particlesystemoptions();
        setParticleSystemOptions(options, nodeOptions, node);
        
        curOptions = nodeOptions;
    }
    else if (classname == "Button")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        Widget* widget = dynamic_cast<Widget*>(node);
        
        ButtonOptions* options = nodetree->mutable_buttonoptions();
        setButtonOptions(options, widgetOptions, widget);
        
        curOptions = widgetOptions;
    }
    else if (classname == "CheckBox")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        Widget* widget = dynamic_cast<Widget*>(node);
        
        CheckBoxOptions* options = nodetree->mutable_checkboxoptions();
        setCheckBoxOptions(options, widgetOptions, widget);
        
        curOptions = widgetOptions;
    }
    else if (classname == "ImageView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        Widget* widget = dynamic_cast<Widget*>(node);
        
        ImageViewOptions* options = nodetree->mutable_imageviewoptions();
        setImageViewOptions(options, widgetOptions, widget);
        
        curOptions = widgetOptions;
    }
    else if (classname == "TextAtlas")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        Widget* widget = dynamic_cast<Widget*>(node);
        
        TextAtlasOptions* options = nodetree->mutable_textatlasoptions();
        setLabelAtlasOptions(options, widgetOptions, widget);
        
        curOptions = widgetOptions;
    }
    else if (classname == "TextBMFont")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        Widget* widget = dynamic_cast<Widget*>(node);
        
        TextBMFontOptions* options = nodetree->mutable_textbmfontoptions();
        setLabelBMFontOptions(options, widgetOptions, widget);
        
        curOptions = widgetOptions;
    }
    else if (classname == "Label")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        Widget* widget = dynamic_cast<Widget*>(node);
        
        TextOptions* options = nodetree->mutable_textoptions();
        setLabelOptions(options, widgetOptions, widget);
        
        curOptions = widgetOptions;
    }
    else if (classname == "LoadingBar")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        Widget* widget = dynamic_cast<Widget*>(node);
        
        LoadingBarOptions* options = nodetree->mutable_loadingbaroptions();
        setLoadingBarOptions(options, widgetOptions, widget);
        
        curOptions = widgetOptions;
    }
    else if (classname == "Slider")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        Widget* widget = dynamic_cast<Widget*>(node);
        
        SliderOptions* options = nodetree->mutable_slideroptions();
        setSliderOptions(options, widgetOptions, widget);
        
        curOptions = widgetOptions;
    }
    else if (classname == "TextField")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        Widget* widget = dynamic_cast<Widget*>(node);
        
        TextFieldOptions* options = nodetree->mutable_textfieldoptions();
        setTextFieldOptions(options, widgetOptions, widget);
        
        curOptions = widgetOptions;
    }
    else if (classname == "Layout")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        Widget* widget = dynamic_cast<Widget*>(node);
        
        PanelOptions* layoutOptions = nodetree->mutable_paneloptions();
        setLayoutOptions(layoutOptions, widgetOptions, widget);
        
        curOptions = widgetOptions;
    }
    else if (classname == "PageView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        Widget* widget = dynamic_cast<Widget*>(node);
        
        PageViewOptions* options = nodetree->mutable_pageviewoptions();
        setPageViewOptions(options, widgetOptions, widget);
        
        curOptions = widgetOptions;
    }
    else if (classname == "ScrollView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        Widget* widget = dynamic_cast<Widget*>(node);
        
        ScrollViewOptions* options = nodetree->mutable_scrollviewoptions();
        setScrollViewOptions(options, widgetOptions, widget);
        
        curOptions = widgetOptions;
    }
    else if (classname == "ListView")
    {
        WidgetOptions* widgetOptions = nodetree->mutable_widgetoptions();
        Widget* widget = dynamic_cast<Widget*>(node);
        
        ListViewOptions* options = nodetree->mutable_listviewoptions();
        setListViewOptions(options, widgetOptions, widget);
        
        curOptions = widgetOptions;
    }
    
    // component
    if (node->getName() == "CocoStudioDefaultElementSkip")
    {
        const Map<std::string, Component*>* components = node->getAllComponents();
        
        if (components != nullptr)
        {
            for (auto iter = components->begin(); iter != components->end(); ++iter)
            {
                ComponentOptions* options = curOptions->add_componentoptions();
                Component* component = iter->second;
                convertComponentProtocolBuffers(options, component);
            }
        }
    }
    //
    
    // children
    int max = node->getChildrenCount();
    CCLOG("max = %d", max);
    Vector<Node*> children = node->getChildren();
    for (auto& obj : children)
    {
        Node* child = static_cast<Node*>(obj);
        if (child)
        {
            // ignore node
            if (child->getName() == "CocoStudioDefaultElementIgnore")
            {
                continue;
            }
            
            protocolbuffers::NodeTree* subNodeTree = nodetree->add_children();
            std::string childClassName = "";
            if (dynamic_cast<Sprite*>(child))
            {
                childClassName = "Sprite";
                Sprite* sprite = dynamic_cast<Sprite*>(child);
                convertNodeTreeProtocolBuffersFromNode(subNodeTree, sprite, childClassName);
            }
            else if (dynamic_cast<ParticleSystem*>(child))
            {
                childClassName = "ParticleSystem";
                ParticleSystem* particle = dynamic_cast<ParticleSystem*>(child);
                convertNodeTreeProtocolBuffersFromNode(subNodeTree, particle, childClassName);
            }
            else if (dynamic_cast<TMXTiledMap*>(child))
            {
                childClassName = "TMXTiledMap";
                TMXTiledMap* tmx = dynamic_cast<TMXTiledMap*>(child);
                convertNodeTreeProtocolBuffersFromNode(subNodeTree, tmx, childClassName);
            }
            else if (dynamic_cast<Widget*>(child))
            {
                Widget* widget = dynamic_cast<Widget*>(child);
                childClassName = widget->getDescription();
                convertNodeTreeProtocolBuffersFromNode(subNodeTree, widget, childClassName);
            }
        }
    }
    //
}

bool ProtocolBuffersSerialize::isUIWidget(std::string classname)
{
    return (classname == "Widget"
            || classname == "Button"
            || classname == "CheckBox"
            || classname == "ImageView"
            || classname == "LabelAtlas"
            || classname == "LabelBMFont"
            || classname == "Label"
            || classname == "TextField"
            || classname == "LoadingBar"
            || classname == "Slider"
            || classname == "Layout"
            || classname == "ScrollView"
            || classname == "ListView"
            || classname == "PageView");
}

// node
void ProtocolBuffersSerialize::setNodeOptions(protocolbuffers::WidgetOptions *nodeOptions, cocos2d::Node *node)
{
    WidgetOptions* options = nodeOptions;
    
    options->set_name(node->getName());
    options->set_x(node->getPositionX());
    options->set_y(node->getPositionY());
    options->set_scalex(node->getScaleX());
    options->set_scaley(node->getScaleY());
    options->set_rotation(node->getRotation());
    options->set_anchorpointx(node->getAnchorPoint().x);
    options->set_anchorpointy(node->getAnchorPoint().y);
    options->set_zorder(node->getZOrder());
    options->set_tag(node->getTag());
    options->set_visible(node->isVisible());
    options->set_opacity(node->getOpacity());
    options->set_colorr(node->getColor().r);
    options->set_colorg(node->getColor().g);
    options->set_colorb(node->getColor().b);
    
    cocostudio::timeline::ActionTimelineData* timelineActionData = dynamic_cast<cocostudio::timeline::ActionTimelineData*>(node->getUserObject());
    int actionTag = timelineActionData->getActionTag();
    options->set_actiontag(actionTag);
}

void ProtocolBuffersSerialize::setSpriteOptions(protocolbuffers::SpriteOptions *spriteOptions,
                                                protocolbuffers::WidgetOptions *nodeOptions,
                                                cocos2d::Node *node)
{
    setNodeOptions(nodeOptions, node);
    
    SpriteOptions* options = spriteOptions;
    Sprite* sprite = static_cast<Sprite*>(node);
    
    nodeOptions->set_flipx(sprite->isFlippedX());
    nodeOptions->set_flipy(sprite->isFlippedY());
    
    options->set_filename(sprite->getFileName());

}

void ProtocolBuffersSerialize::setTMXTiledMapOptions(protocolbuffers::TMXTiledMapOptions *tmxTiledMapOptions,
                                                     protocolbuffers::WidgetOptions *nodeOptions,
                                                     cocos2d::Node *node)
{
    setNodeOptions(nodeOptions, node);
    
    TMXTiledMapOptions* options = tmxTiledMapOptions;
    TMXTiledMap* tmxTiledMap = static_cast<TMXTiledMap*>(node);
    
    options->set_tmxfile(tmxTiledMap->getTMXFile());
    options->set_tmxstring(tmxTiledMap->getTMXString());
    options->set_resourcepath(tmxTiledMap->getResourcePath());
}

void ProtocolBuffersSerialize::setParticleSystemOptions(protocolbuffers::ParticleSystemOptions *particleSystemOptions,
                                                        protocolbuffers::WidgetOptions *nodeOptions,
                                                        cocos2d::Node *node)
{
    setNodeOptions(nodeOptions, node);
    
    ParticleSystemOptions* options = particleSystemOptions;
    ParticleSystem* particleSystem = static_cast<ParticleSystem*>(node);
    
    options->set_plistfile(particleSystem->getPlistFile());
    options->set_totalparticles(particleSystem->getTotalParticles());
}

void ProtocolBuffersSerialize::setWidgetOptions(protocolbuffers::WidgetOptions *widgetOptions,
                                                cocos2d::ui::Widget *widget)
{
    WidgetOptions* options = widgetOptions;
    
    options->set_name(widget->getName());
    options->set_zorder(widget->getZOrder());
    options->set_actiontag(widget->getActionTag());
    options->set_anchorpointx(widget->getAnchorPoint().x);
    options->set_anchorpointy(widget->getAnchorPoint().y);
    options->set_colorb(widget->getColor().b);
    options->set_colorg(widget->getColor().g);
    options->set_colorr(widget->getColor().r);
    options->set_flipx(widget->isFlippedX());
    options->set_flipy(widget->isFlippedY());
    options->set_height(widget->getContentSize().width);
    options->set_ignoresize(widget->isIgnoreContentAdaptWithSize());
    options->set_opacity(widget->getOpacity());
    options->set_positionpercentx(widget->getPositionPercent().x);
    options->set_positionpercenty(widget->getPositionPercent().y);
    options->set_positiontype((int)widget->getPositionType());
    options->set_rotation(widget->getRotation());
    options->set_scalex(widget->getScaleX());
    options->set_scaley(widget->getScaleY());
    options->set_sizepercentx(widget->getSizePercent().x);
    options->set_sizepercenty(widget->getSizePercent().y);
    options->set_sizetype((int)widget->getSizeType());
    options->set_tag(widget->getTag());
    options->set_touchable(widget->isTouchEnabled());
    options->set_visible(widget->isVisible());
    options->set_width(widget->getContentSize().width);
    options->set_x(widget->getPositionX());
    options->set_y(widget->getPositionY());
    
    
    cocostudio::timeline::ActionTimelineData* timelineActionData = dynamic_cast<cocostudio::timeline::ActionTimelineData*>(widget->getUserObject());
    int actionTag = timelineActionData->getActionTag();
    options->set_actiontag(actionTag);
    
    protocolbuffers::LayoutParameter* layoutParameter = options->mutable_layoutparameter();
    ui::LayoutParameter* linearLP = widget->getLayoutParameter();
    ui::LayoutParameter* relativeLP = widget->getLayoutParameter();
    if (linearLP || relativeLP)
    {
        Margin margin;
        if (linearLP)
        {
            LinearLayoutParameter* linear = dynamic_cast<LinearLayoutParameter*>(linearLP);
            margin = linear->getMargin();
            layoutParameter->set_type((int)linearLP->getLayoutType());
            layoutParameter->set_gravity((int)linear->getGravity());
        }
        else if (relativeLP)
        {
            RelativeLayoutParameter* relative = dynamic_cast<RelativeLayoutParameter*>(relativeLP);
            margin = relative->getMargin();
            layoutParameter->set_type((int)relativeLP->getLayoutType());
            layoutParameter->set_relativename(relative->getRelativeName());
            layoutParameter->set_relativetoname(relative->getRelativeToWidgetName());
            layoutParameter->set_align((int)relative->getAlign());
        }
        layoutParameter->set_margindown(margin.bottom);
        layoutParameter->set_marginleft(margin.left);
        layoutParameter->set_marginright(margin.right);
        layoutParameter->set_margintop(margin.top);
    }
    
    // need to modify
//    WidgetOptions_CustomProperty* customProperty = options->mutable_customproperty();
//    customProperty->set_stringtest("custom");
    //
}

void ProtocolBuffersSerialize::setButtonOptions(protocolbuffers::ButtonOptions *buttonOptions,
                                                protocolbuffers::WidgetOptions *widgetOptions,
                                                cocos2d::ui::Widget *widget)
{
    setWidgetOptions(widgetOptions, widget);
    
    Button* button = static_cast<Button*>(widget);
    ButtonOptions* options = buttonOptions;
    
    options->set_scale9enable(button->isScale9Enabled());
    options->set_capinsetsx(button->getCapInsetsNormalRenderer().origin.x);
    options->set_capinsetsy(button->getCapInsetsNormalRenderer().origin.y);
    options->set_capinsetswidth(button->getCapInsetsNormalRenderer().size.width);
    options->set_capinsetsheight(button->getCapInsetsNormalRenderer().size.height);
    options->set_text(button->getTitleText());
    options->set_textcolorr(button->getTitleColor().r);
    options->set_textcolorg(button->getTitleColor().g);
    options->set_textcolorb(button->getTitleColor().b);
    options->set_fontsize(button->getTitleFontSize());
    options->set_fontname(button->getTitleFontName());
    
    
    ResourceData* normalData = options->mutable_normaldata();
    normalData->set_resourcetype((int)button->getNormalTextureResType());
    normalData->set_path(button->getNormalFileName());
    
    ResourceData* pressedData = options->mutable_presseddata();
    pressedData->set_resourcetype((int)button->getPressedTextureResType());
    pressedData->set_path(button->getPressedFileName());
    
    ResourceData* disabledData = options->mutable_disableddata();
    disabledData->set_resourcetype((int)button->getDisabledTextureResType());
    disabledData->set_path(button->getDisableFileName());
    
    
    // for testing
//    int pos = GUIReader::getInstance()->getFilePath().length();
//    std::string path = button->getNormalFileName();
//    std::string realPath = (normalPath != "") ? normalPath.substr(pos) : "";
//    normalData->set_path(realPath);
//    normalData->set_plistfile("");
//    normalData->set_resourcetype(0);
    
    // for testing
//    path = button->getPressedFileName();
//    realPath = (path != "") ? path.substr(pos) : "";
//    ButtonOptions_PressedData* pressData = options->mutable_presseddata();
//    pressData->set_path(realPath);
//    pressData->set_plistfile("");
//    pressData->set_resourcetype(0);
    
    // for testing
//    path = button->getDisableFileName();
//    realPath = (path != "") ? path.substr(pos) : "";
//    ButtonOptions_DisabledData* disabledData = options->mutable_disableddata();
//    disabledData->set_path(realPath);
//    disabledData->set_plistfile("");
//    disabledData->set_resourcetype(0);
}

void ProtocolBuffersSerialize::setCheckBoxOptions(protocolbuffers::CheckBoxOptions *checkBoxOptions,
                                                  protocolbuffers::WidgetOptions *widgetOptions,
                                                  cocos2d::ui::Widget *widget)
{
    setWidgetOptions(widgetOptions, widget);
   
    CheckBox* checkBox = static_cast<CheckBox*>(widget);
    CheckBoxOptions* options = checkBoxOptions;
    
    ResourceData* backGroundBoxData = options->mutable_backgroundboxdata();
    backGroundBoxData->set_resourcetype((int)checkBox->getBackGroundTextureResType());
    std::string t = checkBox->getBackGroundFileName();
    backGroundBoxData->set_path(checkBox->getBackGroundFileName());
    
    ResourceData* backGroundBoxSelectedData = options->mutable_backgroundboxselecteddata();
    backGroundBoxSelectedData->set_resourcetype((int)checkBox->getBackGroundSelectedTextureResType());
    backGroundBoxSelectedData->set_path(checkBox->getBackGroundSelectedFileName());
    
    ResourceData* frontCrossData = options->mutable_frontcrossdata();
    frontCrossData->set_resourcetype((int)checkBox->getFrontCrossTextureResType());
    frontCrossData->set_path(checkBox->getFrontCrossFileName());
    
    ResourceData* backGroundBoxDisabledData = options->mutable_backgroundboxdisableddata();
    backGroundBoxDisabledData->set_resourcetype((int)checkBox->getBackGroundDisabledTextureResType());
    backGroundBoxDisabledData->set_path(checkBox->getBackGroundDisabledFileName());
    
    ResourceData* frontCrossDisabledData = options->mutable_frontcrossdisableddata();
    frontCrossDisabledData->set_resourcetype((int)checkBox->getFrontCrossDisabledTextureResType());
    frontCrossDisabledData->set_path(checkBox->getFrontCrossDisabledFileName());
    
    
    
    // for testing
//    int pos = GUIReader::shareReader()->getFilePath().length();
//    std::string path = checkBox->getBackGroundFileName();
//    std::string realPath = (path != "") ? path.substr(pos) : "";
//    backGroundBoxData->set_path(realPath);
//    backGroundBoxData->set_plistfile("");
//    backGroundBoxData->set_resourcetype(0);
    
    // for testing
//    path = checkBox->getBackGroundSelectedFileName();
//    realPath = (path != "") ? path.substr(pos) : "";
//    backGroundBoxSelectedData->set_path(realPath);
//    backGroundBoxSelectedData->set_plistfile("");
//    backGroundBoxSelectedData->set_resourcetype(0);
    
    // for testing
//    path = checkBox->getFrontCrossFileName();
//    realPath = (path != "") ? path.substr(pos) : "";
//    frontCrossData->set_path(realPath);
//    frontCrossData->set_plistfile("");
//    frontCrossData->set_resourcetype(0);
    
    // for testing
//    path = checkBox->getBackGroundDisabledFileName();
//    realPath = (path != "") ? path.substr(pos) : "";
//    backGroundBoxDisabledData->set_path(realPath);
//    backGroundBoxDisabledData->set_plistfile("");
//    backGroundBoxDisabledData->set_resourcetype(0);
    
    // for testing
//    path = checkBox->getBackGroundDisabledFileName();
//    realPath = (path != "") ? path.substr(pos) : "";
//    frontCrossDisabledData->set_path(realPath);
//    frontCrossDisabledData->set_plistfile("");
//    frontCrossDisabledData->set_resourcetype(0);
}

void ProtocolBuffersSerialize::setImageViewOptions(protocolbuffers::ImageViewOptions *imageViewOptions,
                                                   protocolbuffers::WidgetOptions *widgetOptions,
                                                   cocos2d::ui::Widget *widget)
{
    setWidgetOptions(widgetOptions, widget);
    
    ImageView* imageView = static_cast<ImageView*>(widget);
    ImageViewOptions* options = imageViewOptions;
    
    options->set_capinsetsheight(imageView->getCapInsets().size.height);
    options->set_capinsetswidth(imageView->getCapInsets().size.width);
    options->set_capinsetsx(imageView->getCapInsets().origin.x);
    options->set_capinsetsy(imageView->getCapInsets().origin.y);
    options->set_scale9enable(imageView->isScale9Enabled());
    options->set_scale9height(imageView->getContentSize().height);
    options->set_scale9width(imageView->getContentSize().width);
    
    ResourceData* fileNameData = options->mutable_filenamedata();
    fileNameData->set_resourcetype((int)imageView->getTextureResType());
    fileNameData->set_path(imageView->getTextureFile());
    
    // for testing
//    int pos = GUIReader::shareReader()->getFilePath().length();
//    std::string path = imageView->getTextureFile();
//    std::string realPath = (path != "") ? path.substr(pos) : "";
//    fileNameData->set_path(realPath.c_str());
//    fileNameData->set_plistfile("");
//    fileNameData->set_resourcetype(0);
}

void ProtocolBuffersSerialize::setLabelAtlasOptions(protocolbuffers::TextAtlasOptions *labelAtlasOptions,
                                                    protocolbuffers::WidgetOptions *widgetOptions,
                                                    cocos2d::ui::Widget *widget)
{
    setWidgetOptions(widgetOptions, widget);
    
    TextAtlas* labelAtlas = static_cast<TextAtlas*>(widget);
    TextAtlasOptions* options = labelAtlasOptions;
    
    options->set_stringvalue(labelAtlas->getString());
    options->set_itemwidth(labelAtlas->getItemWidth());
    options->set_itemheight(labelAtlas->getItemHeight());
    options->set_startcharmap(labelAtlas->getStartCharMap());
    
    ResourceData* charMapFileData = options->mutable_charmapfiledata();
    charMapFileData->set_path(labelAtlas->getCharMapFileName());
    
    // for testing
//    int pos = GUIReader::shareReader()->getFilePath().length();
//    std::string path = labelAtlas->getCharMapFileName();
//    std::string realPath = (path != "") ? path.substr(pos) : "";
//    charMapFileData->set_path(realPath.c_str());
//    charMapFileData->set_plistfile("");
//    charMapFileData->set_resourcetype(0);
    
}

void ProtocolBuffersSerialize::setLabelBMFontOptions(protocolbuffers::TextBMFontOptions *labelBMFontOptions,
                                                     protocolbuffers::WidgetOptions *widgetOptions,
                                                     cocos2d::ui::Widget *widget)
{
    setWidgetOptions(widgetOptions, widget);
    
    TextBMFontOptions* options = labelBMFontOptions;
    TextBMFont* labelBMFont = static_cast<TextBMFont*>(widget);
    
    options->set_text(labelBMFont->getString());
    
    ResourceData* fileNameData = options->mutable_filenamedata();
    fileNameData->set_path(labelBMFont->getFntFileName());
    
    // for testing
//    int pos = GUIReader::shareReader()->getFilePath().length();
//    std::string path = labelBMFont->getFntFileName();
//    std::string realPath = (path != "") ? path.substr(pos) : "";
//    fileNameData->set_path(realPath.c_str());
//    fileNameData->set_plistfile("");
//    fileNameData->set_resourcetype(0);
}

void ProtocolBuffersSerialize::setLabelOptions(protocolbuffers::TextOptions *labelOptions,
                                               protocolbuffers::WidgetOptions *widgetOptions,
                                               cocos2d::ui::Widget *widget)
{
    setWidgetOptions(widgetOptions, widget);
    
    Text* label = static_cast<Text*>(widget);
    TextOptions* options = labelOptions;
    
    options->set_touchscaleenable(label->isTouchScaleChangeEnabled());
    options->set_text(label->getString());
    options->set_fontsize(label->getFontSize());
    options->set_fontname(label->getFontName());
    options->set_areawidth(label->getTextAreaSize().width);
    options->set_areaheight(label->getTextAreaSize().height);
    options->set_halignment((int)label->getTextHorizontalAlignment());
    options->set_valignment((int)label->getTextVerticalAlignment());
}

void ProtocolBuffersSerialize::setLoadingBarOptions(protocolbuffers::LoadingBarOptions *loadingBarOptions,
                                                    protocolbuffers::WidgetOptions *widgetOptions,
                                                    cocos2d::ui::Widget *widget)
{
    setWidgetOptions(widgetOptions, widget);
    
    LoadingBar* loadingBar = static_cast<LoadingBar*>(widget);
    LoadingBarOptions* options = loadingBarOptions;
    
    options->set_direction((int)loadingBar->getDirection());
    options->set_percent(loadingBar->getPercent());
    options->set_scale9enable(loadingBar->isScale9Enabled());
    options->set_capinsetsx(loadingBar->getCapInsets().origin.x);
    options->set_capinsetsy(loadingBar->getCapInsets().origin.y);
    options->set_capinsetswidth(loadingBar->getCapInsets().size.width);
    options->set_capinsetsheight(loadingBar->getCapInsets().size.height);
    
    ResourceData* textureData = options->mutable_texturedata();
    textureData->set_resourcetype((int)loadingBar->getTextureResType());
    textureData->set_path(loadingBar->getTextureFile());
    
    // for testing
//    int pos = GUIReader::shareReader()->getFilePath().length();
//    std::string path = loadingBar->getTextureFile();
//    std::string realPath = (path != "") ? path.substr(pos) : "";
//    textureData->set_path(realPath);
//    textureData->set_plistfile("");
//    textureData->set_resourcetype(0);
}

void ProtocolBuffersSerialize::setSliderOptions(protocolbuffers::SliderOptions *sliderOptions,
                                                protocolbuffers::WidgetOptions *widgetOptions,
                                                cocos2d::ui::Widget *widget)
{
    setWidgetOptions(widgetOptions, widget);
    
    Slider* slider = static_cast<Slider*>(widget);
    SliderOptions* options = sliderOptions;
    
    options->set_scale9enable(slider->isScale9Enabled());
    options->set_length(slider->getContentSize().width);
    options->set_percent(slider->getPercent());
    
    ResourceData* barFileNameData = options->mutable_barfilenamedata();
    barFileNameData->set_resourcetype((int)slider->getBarTextureResType());
    barFileNameData->set_path(slider->getTextureFile());
    
    ResourceData* ballNormalData = options->mutable_ballnormaldata();
    ballNormalData->set_resourcetype((int)slider->getBallNormalTexureResType());
    ballNormalData->set_path(slider->getSlidBallNormalTextureFile());
    
    ResourceData* ballPressedData = options->mutable_ballpresseddata();
    ballPressedData->set_resourcetype((int)slider->getBallPressedTexureResType());
    ballPressedData->set_path(slider->getSlidBallPressedTextureFile());
    
    ResourceData* ballDisabledData = options->mutable_balldisableddata();
    ballDisabledData->set_resourcetype((int)slider->getBallDisabledTextureResType());
    ballDisabledData->set_path(slider->getSlidBallDisabledTextureFile());
    
    ResourceData* progressBarData = options->mutable_progressbardata();
    progressBarData->set_resourcetype((int)slider->getProgressBarTextureResType());
    progressBarData->set_path(slider->getProgressBarTextureFile());
    
    // for testing
//    int pos = GUIReader::shareReader()->getFilePath().length();
//    std::string path = slider->getTextureFile();
//    std::string realPath = (path != "") ? path.substr(pos) : "";
//    barFileNameData->set_path(realPath);
//    barFileNameData->set_plistfile("");
//    barFileNameData->set_resourcetype(0);
    
    // for testing
//    path = slider->getSlidBallNormalTextureFile();
//    realPath = (path != "") ? path.substr(pos) : "";
//    ballNormalData->set_path(realPath);
//    ballNormalData->set_plistfile("");
//    ballNormalData->set_resourcetype(0);
    
    // for testing
//    path = slider->getSlidBallPressedTextureFile();
//    realPath = (path != "") ? path.substr(pos) : "";
//    ballPressedData->set_path(realPath);
//    ballPressedData->set_plistfile("");
//    ballPressedData->set_resourcetype(0);
    
    // for testing
//    path = slider->getSlidBallDisabledTextureFile();
//    realPath = (path != "") ? path.substr(pos) : "";
//    ballDisabledData->set_path(realPath);
//    ballDisabledData->set_plistfile("");
//    ballDisabledData->set_resourcetype(0);
    
    // for testing
//    path = slider->getProgressBarTextureFile();
//    realPath = (path != "") ? path.substr(pos) : "";
//    progressBarData->set_path(realPath);
//    progressBarData->set_plistfile("");
//    progressBarData->set_resourcetype(0);
}

void ProtocolBuffersSerialize::setTextFieldOptions(protocolbuffers::TextFieldOptions *textFieldOptions,
                                                   protocolbuffers::WidgetOptions *widgetOptions,
                                                   cocos2d::ui::Widget *widget)
{
    setWidgetOptions(widgetOptions, widget);
    
    TextField* textField = static_cast<TextField*>(widget);
    TextFieldOptions* options = textFieldOptions;
    
    options->set_placeholder(textField->getPlaceHolder());
    options->set_text(textField->getStringValue());
    options->set_fontsize(textField->getFontSize());
    options->set_fontname(textField->getFontName());
    options->set_maxlengthenable(textField->isMaxLengthEnabled());
    options->set_maxlength(textField->getMaxLength());
    options->set_passwordenable(textField->isPasswordEnabled());
    options->set_passwordstyletext(textField->getPasswordStyleText());
    options->set_areawidth(textField->getTextAreaSize().width);
    options->set_areaheight(textField->getTextAreaSize().height);
}

void ProtocolBuffersSerialize::setLayoutOptions(protocolbuffers::PanelOptions *layoutOptions,
                                                protocolbuffers::WidgetOptions *widgetOptions,
                                                cocos2d::ui::Widget *widget)
{
    setWidgetOptions(widgetOptions, widget);
    
    Layout* layout = static_cast<Layout*>(widget);
    PanelOptions* options = layoutOptions;
    
    options->set_backgroundscale9enable(layout->isBackGroundImageScale9Enabled());
    options->set_bgcolorb(layout->getBackGroundColor().b);
    options->set_bgcolorg(layout->getBackGroundColor().g);
    options->set_bgcolorr(layout->getBackGroundColor().r);
    options->set_bgcoloropacity(layout->getBackGroundColorOpacity());
    options->set_bgendcolorb(layout->getBackGroundEndColor().b);
    options->set_bgendcolorg(layout->getBackGroundEndColor().g);
    options->set_bgendcolorr(layout->getBackGroundEndColor().r);
    options->set_bgstartcolorb(layout->getBackGroundStartColor().b);
    options->set_bgstartcolorg(layout->getBackGroundStartColor().g);
    options->set_bgstartcolorr(layout->getBackGroundStartColor().r);
    options->set_capinsetsheight(layout->getBackGroundImageCapInsets().size.height);
    options->set_capinsetswidth(layout->getBackGroundImageCapInsets().size.width);
    options->set_capinsetsx(layout->getBackGroundImageCapInsets().origin.x);
    options->set_capinsetsy(layout->getBackGroundImageCapInsets().origin.y);
    options->set_clipable(layout->isClippingEnabled());
    options->set_colortype((int)layout->getBackGroundColorType());
    options->set_layouttype((int)layout->getLayoutType());
    options->set_vectorx(layout->getBackGroundColorVector().x);
    options->set_vectory(layout->getBackGroundColorVector().y);
    
    ResourceData* backGroundImageData = options->mutable_backgroundimagedata();
    backGroundImageData->set_resourcetype((int)layout->getBackGroundImageTextureResType());
    backGroundImageData->set_path(layout->getBackGroundImageFileName());
    
    // for testing
//    int pos = GUIReader::shareReader()->getFilePath().length();
//    std::string path = layout->getBackGroundImageFileName();
//    std::string realPath = (path != "") ? path.substr(pos) : "";
//    backGroundImageData->set_path(realPath);
//    backGroundImageData->set_plistfile("");
//    backGroundImageData->set_resourcetype(0);
}

void ProtocolBuffersSerialize::setPageViewOptions(protocolbuffers::PageViewOptions *pageViewOptions,
                                                  protocolbuffers::WidgetOptions *widgetOptions,
                                                  cocos2d::ui::Widget *widget)
{
    setWidgetOptions(widgetOptions, widget);
    
    PageView* pageView = static_cast<PageView*>(widget);
    PageViewOptions* options = pageViewOptions;
    
    options->set_backgroundscale9enable(pageView->isBackGroundImageScale9Enabled());
    options->set_bgcolorb(pageView->getBackGroundColor().b);
    options->set_bgcolorg(pageView->getBackGroundColor().g);
    options->set_bgcolorr(pageView->getBackGroundColor().r);
    options->set_bgcoloropacity(pageView->getBackGroundColorOpacity());
    options->set_bgendcolorb(pageView->getBackGroundEndColor().b);
    options->set_bgendcolorg(pageView->getBackGroundEndColor().g);
    options->set_bgendcolorr(pageView->getBackGroundEndColor().r);
    options->set_bgstartcolorb(pageView->getBackGroundStartColor().b);
    options->set_bgstartcolorg(pageView->getBackGroundStartColor().g);
    options->set_bgstartcolorr(pageView->getBackGroundStartColor().r);
    options->set_capinsetsheight(pageView->getBackGroundImageCapInsets().size.height);
    options->set_capinsetswidth(pageView->getBackGroundImageCapInsets().size.width);
    options->set_capinsetsx(pageView->getBackGroundImageCapInsets().origin.x);
    options->set_capinsetsy(pageView->getBackGroundImageCapInsets().origin.y);
    options->set_clipable(pageView->isClippingEnabled());
    options->set_colortype((int)pageView->getBackGroundColorType());
    options->set_vectorx(pageView->getBackGroundColorVector().x);
    options->set_vectory(pageView->getBackGroundColorVector().y);
    
    ResourceData* backGroundImageData = options->mutable_backgroundimagedata();
    backGroundImageData->set_resourcetype((int)pageView->getBackGroundImageTextureResType());
    backGroundImageData->set_path(pageView->getBackGroundImageFileName());
    
}

void ProtocolBuffersSerialize::setScrollViewOptions(protocolbuffers::ScrollViewOptions *scrollViewOptions,
                                                    protocolbuffers::WidgetOptions *widgetOptions,
                                                    cocos2d::ui::Widget *widget)
{
    setWidgetOptions(widgetOptions, widget);
    
    ScrollView* scrollView = static_cast<ScrollView*>(widget);
    ScrollViewOptions* options = scrollViewOptions;
    
    options->set_backgroundscale9enable(scrollView->isBackGroundImageScale9Enabled());
    options->set_bgcolorb(scrollView->getBackGroundColor().b);
    options->set_bgcolorg(scrollView->getBackGroundColor().g);
    options->set_bgcolorr(scrollView->getBackGroundColor().r);
    options->set_bgcoloropacity(scrollView->getBackGroundColorOpacity());
    options->set_bgendcolorb(scrollView->getBackGroundEndColor().b);
    options->set_bgendcolorg(scrollView->getBackGroundEndColor().g);
    options->set_bgendcolorr(scrollView->getBackGroundEndColor().r);
    options->set_bgstartcolorb(scrollView->getBackGroundStartColor().b);
    options->set_bgstartcolorg(scrollView->getBackGroundStartColor().g);
    options->set_bgstartcolorr(scrollView->getBackGroundStartColor().r);
    options->set_capinsetsheight(scrollView->getBackGroundImageCapInsets().size.height);
    options->set_capinsetswidth(scrollView->getBackGroundImageCapInsets().size.width);
    options->set_capinsetsx(scrollView->getBackGroundImageCapInsets().origin.x);
    options->set_capinsetsy(scrollView->getBackGroundImageCapInsets().origin.y);
    options->set_clipable(scrollView->isClippingEnabled());
    options->set_colortype((int)scrollView->getBackGroundColorType());
    options->set_layouttype((int)scrollView->getLayoutType());
    options->set_vectorx(scrollView->getBackGroundColorVector().x);
    options->set_vectory(scrollView->getBackGroundColorVector().y);
    
    ResourceData* backGroundImageData = options->mutable_backgroundimagedata();
    backGroundImageData->set_resourcetype((int)scrollView->getBackGroundImageTextureResType());
    backGroundImageData->set_path(scrollView->getBackGroundImageFileName());
    
    options->set_innerwidth(scrollView->getInnerContainerSize().width);
    options->set_innerheight(scrollView->getInnerContainerSize().height);
    options->set_direction((int)scrollView->getDirection());
    options->set_bounceenable(scrollView->isBounceEnabled());
}

void ProtocolBuffersSerialize::setListViewOptions(protocolbuffers::ListViewOptions *listViewOptions,
                                                  protocolbuffers::WidgetOptions *widgetOptions,
                                                  cocos2d::ui::Widget *widget)
{
    setWidgetOptions(widgetOptions, widget);
    
    ListView* listView = static_cast<ListView*>(widget);
    ListViewOptions* options = listViewOptions;
    
    options->set_backgroundscale9enable(listView->isBackGroundImageScale9Enabled());
    options->set_bgcolorb(listView->getBackGroundColor().b);
    options->set_bgcolorg(listView->getBackGroundColor().g);
    options->set_bgcolorr(listView->getBackGroundColor().r);
    options->set_bgcoloropacity(listView->getBackGroundColorOpacity());
    options->set_bgendcolorb(listView->getBackGroundEndColor().b);
    options->set_bgendcolorg(listView->getBackGroundEndColor().g);
    options->set_bgendcolorr(listView->getBackGroundEndColor().r);
    options->set_bgstartcolorb(listView->getBackGroundStartColor().b);
    options->set_bgstartcolorg(listView->getBackGroundStartColor().g);
    options->set_bgstartcolorr(listView->getBackGroundStartColor().r);
    options->set_capinsetsheight(listView->getBackGroundImageCapInsets().size.height);
    options->set_capinsetswidth(listView->getBackGroundImageCapInsets().size.width);
    options->set_capinsetsx(listView->getBackGroundImageCapInsets().origin.x);
    options->set_capinsetsy(listView->getBackGroundImageCapInsets().origin.y);
    options->set_clipable(listView->isClippingEnabled());
    options->set_colortype((int)listView->getBackGroundColorType());
    options->set_vectorx(listView->getBackGroundColorVector().x);
    options->set_vectory(listView->getBackGroundColorVector().y);
    
    ResourceData* backGroundImageData = options->mutable_backgroundimagedata();
    backGroundImageData->set_resourcetype((int)listView->getBackGroundImageTextureResType());
    backGroundImageData->set_path(listView->getBackGroundImageFileName());
    
    
    options->set_innerwidth(listView->getInnerContainerSize().width);
    options->set_innerheight(listView->getInnerContainerSize().height);
    options->set_direction((int)listView->getDirection());
    options->set_bounceenable(listView->isBounceEnabled());
    
    options->set_direction((int)listView->getDirection());
    options->set_gravity((int)listView->getGravity());
    options->set_itemmargin(listView->getItemsMargin());
}

#pragma mark -
#pragma mark serialize component into nodetree for protocol buffers with node
#pragma mark -

// component
void ProtocolBuffersSerialize::convertComponentProtocolBuffers(protocolbuffers::ComponentOptions *componentOptions,
                                                               cocos2d::Component *component)
{
    if (dynamic_cast<cocostudio::ComAudio*>(component))
    {
        componentOptions->set_type("ComAudio");
        
        ComAudioOptions* options = componentOptions->mutable_comaudiooptions();
        setComAudioOptions(options, component);
    }
}

void ProtocolBuffersSerialize::setComAudioOptions(protocolbuffers::ComAudioOptions *comAudioOptions,
                                                  cocos2d::Component *component)
{
    ComAudioOptions* options = comAudioOptions;
    cocostudio::ComAudio* comAudio = static_cast<cocostudio::ComAudio*>(component);
    
    options->set_loop(comAudio->isLoop());
    
    options->set_name(component->getName());
    options->set_enabled(component->isEnabled());
    
//    ResourceData* fileNameData = options->mutable_filenamedata();
//    fileNameData->set_resourcetype((int)imageView->getTextureResType());
//    fileNameData->set_path(imageView->getTextureFile());
}

#pragma mark -
#pragma mark serialize action into protocol buffers with node
#pragma mark -

// ActionTimeline
void ProtocolBuffersSerialize::convertActionProtocolBuffersWithNode(protocolbuffers::NodeAction *nodeAction, cocostudio::timeline::ActionTimeline *action)
{
    int duration = action->getDuration();
    nodeAction->set_duration(duration);
    
    int timeSpeed = action->getTimeSpeed();
    nodeAction->set_speed(timeSpeed);
    
    int timelineLength = action->getTimelines().size();
    for (int i = 0; i < timelineLength; i++)
    {
        cocostudio::timeline::Timeline* timeline = action->getTimelines().at(i);
        protocolbuffers::TimeLine* protoBufTimeLine = nodeAction->add_timelines();
        convertTimelineProtocolBuffers(protoBufTimeLine, timeline);
    }
}

void ProtocolBuffersSerialize::convertTimelineProtocolBuffers(protocolbuffers::TimeLine *protolBuftimeline,
                                                              cocostudio::timeline::Timeline *timeline)
{
    using namespace protocolbuffers;
    
    // get frame type
    std::string frameTypeTemp = timeline->getFrameType().c_str();
	if (frameTypeTemp == "")
        return;
    
    protolBuftimeline->set_frametype(frameTypeTemp);
    
    int actionTag = timeline->getActionTag();
    protolBuftimeline->set_actiontag(actionTag);
    
    
    int length = timeline->getFrames().size();
    for (int i = 0; i < length; i++)
    {
        cocostudio::timeline::Frame* frame = timeline->getFrames().at(i);
        
        protocolbuffers::Frame* protoBufFrame = protolBuftimeline->add_frames();
        
        std::string frameType = frameTypeTemp;
        
        if (frameType == FrameType_VisibleFrame)
        {            
            TimeLineBoolFrame* protoBufVisibleFrame = protoBufFrame->mutable_visibleframe();
            setVisibleFrame(protoBufVisibleFrame, frame);
        }
        else if (frameType == FrameType_PositionFrame)
        {
            TimeLinePointFrame* protoBufPositionFrame = protoBufFrame->mutable_positionframe();
            setPositionFrame(protoBufPositionFrame, frame);
        }
        else if (frameType == FrameType_ScaleFrame)
        {
            TimeLinePointFrame* protolBufScaleFrame = protoBufFrame->mutable_scaleframe();
            setScaleFrame(protolBufScaleFrame, frame);
        }
        else if (frameType == FrameType_RotationSkewFrame)
        {
            TimeLinePointFrame* protoBufRotationSkewFrame = protoBufFrame->mutable_rotationskewframe();
            setRotationSkewFrame(protoBufRotationSkewFrame, frame);
        }
        else if (frameType == FrameType_AnchorFrame)
        {
            TimeLinePointFrame* protoBufAnchorPointFrame = protoBufFrame->mutable_anchorpointframe();
            setAnchorFrame(protoBufAnchorPointFrame, frame);
        }
        else if (frameType == FrameType_ColorFrame)
        {
            TimeLineColorFrame* protoBufColorFrame = protoBufFrame->mutable_colorframe();
            setColorFrame(protoBufColorFrame, frame);
        }
        else if (frameType == FrameType_TextureFrame)
        {
            TimeLineTextureFrame* protoBufTextureFrame = protoBufFrame->mutable_textureframe();
            setTextureFrame(protoBufTextureFrame, frame);
        }
        else if (frameType == FrameType_EventFrame)
        {
            TimeLineStringFrame* protoBufEventFrame = protoBufFrame->mutable_eventframe();
            setEventFrame(protoBufEventFrame, frame);
        }
        else if (frameType == FrameType_ZOrderFrame)
        {
            TimeLineIntFrame* protoBufZOrderFrame = protoBufFrame->mutable_zorderframe();
            setZOrderFrame(protoBufZOrderFrame, frame);
        }
    }
}

void ProtocolBuffersSerialize::setVisibleFrame(protocolbuffers::TimeLineBoolFrame *protoBufFrame,
                                               cocostudio::timeline::Frame *frame)
{
    VisibleFrame* visibleFrame = static_cast<VisibleFrame*>(frame);
    
    bool visible = visibleFrame->isVisible();
    protoBufFrame->set_value(visible);
    
    int frameIndex = frame->getFrameIndex();
    protoBufFrame->set_frameindex(frameIndex);
    
    bool tween = frame->isTween();
    protoBufFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setPositionFrame(protocolbuffers::TimeLinePointFrame *protoBufFrame,
                                                cocostudio::timeline::Frame *frame)
{
    PositionFrame* positionFrame = static_cast<PositionFrame*>(frame);
    
    float x = positionFrame->getX();
    float y = positionFrame->getY();
    protoBufFrame->set_x(x);
    protoBufFrame->set_y(y);
    
    int frameIndex = frame->getFrameIndex();
    protoBufFrame->set_frameindex(frameIndex);
    
    bool tween = frame->isTween();
    protoBufFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setScaleFrame(protocolbuffers::TimeLinePointFrame *protoBufFrame,
                                             cocostudio::timeline::Frame *frame)
{
    ScaleFrame* scaleFrame = static_cast<ScaleFrame*>(frame);
    
    float scalex = scaleFrame->getScaleX();
    float scaley = scaleFrame->getScaleY();
    protoBufFrame->set_x(scalex);
    protoBufFrame->set_y(scaley);
    
    int frameIndex = frame->getFrameIndex();
    protoBufFrame->set_frameindex(frameIndex);
    
    bool tween = frame->isTween();
    protoBufFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setRotationSkewFrame(protocolbuffers::TimeLinePointFrame *protoBufFrame,
                                                    cocostudio::timeline::Frame *frame)
{
    RotationSkewFrame* rotationSkewFrame = static_cast<RotationSkewFrame*>(frame);
    
    float skewx = rotationSkewFrame->getSkewX();
    float skewy = rotationSkewFrame->getSkewY();
    protoBufFrame->set_x(skewx);
    protoBufFrame->set_y(skewy);
    
    int frameIndex = frame->getFrameIndex();
    protoBufFrame->set_frameindex(frameIndex);
    
    bool tween = frame->isTween();
    protoBufFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setAnchorFrame(protocolbuffers::TimeLinePointFrame *protoBufFrame,
                                              cocostudio::timeline::Frame *frame)
{
    AnchorPointFrame* anchorPointFrame = static_cast<AnchorPointFrame*>(frame);
    
    float anchorx = anchorPointFrame->getAnchorPoint().x;
    float anchory = anchorPointFrame->getAnchorPoint().y;
    protoBufFrame->set_x(anchorx);
    protoBufFrame->set_y(anchory);
    
    int frameIndex = frame->getFrameIndex();
    protoBufFrame->set_frameindex(frameIndex);
    
    bool tween = frame->isTween();
    protoBufFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setColorFrame(protocolbuffers::TimeLineColorFrame *protoBufFrame,
                                             cocostudio::timeline::Frame *frame)
{
    ColorFrame* colorFrame = static_cast<ColorFrame*>(frame);
    
    int alpha = colorFrame->getAlpha();
    int red = colorFrame->getColor().r;
    int green = colorFrame->getColor().g;
    int blue = colorFrame->getColor().b;
    protoBufFrame->set_alpha(alpha);
    protoBufFrame->set_red(red);
    protoBufFrame->set_green(green);
    protoBufFrame->set_blue(blue);
    
    int frameIndex = frame->getFrameIndex();
    protoBufFrame->set_frameindex(frameIndex);
    
    bool tween = frame->isTween();
    protoBufFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setTextureFrame(protocolbuffers::TimeLineTextureFrame *protoBufFrame,
                                               cocostudio::timeline::Frame *frame)
{
    TextureFrame* textureFrame = static_cast<TextureFrame*>(frame);
    
    std::string texture = textureFrame->getTextureName();
    protoBufFrame->set_name(texture);
    
    int frameIndex = frame->getFrameIndex();
    protoBufFrame->set_frameindex(frameIndex);
    
    bool tween = frame->isTween();
    protoBufFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setEventFrame(protocolbuffers::TimeLineStringFrame *protoBufFrame,
                                             cocostudio::timeline::Frame *frame)
{
    EventFrame* eventFrame = static_cast<EventFrame*>(frame);
    
    std::string evnt = eventFrame->getEvent();
    protoBufFrame->set_value(evnt);
    
    int frameIndex = frame->getFrameIndex();
    protoBufFrame->set_frameindex(frameIndex);
    
    bool tween = frame->isTween();
    protoBufFrame->set_tween(tween);
}

void ProtocolBuffersSerialize::setZOrderFrame(protocolbuffers::TimeLineIntFrame *protoBufFrame,
                                              cocostudio::timeline::Frame *frame)
{
    ZOrderFrame* zorderFrame = static_cast<ZOrderFrame*>(frame);
    
    int zorder = zorderFrame->getZOrder();
    protoBufFrame->set_value(zorder);
    
    int frameIndex = frame->getFrameIndex();
    protoBufFrame->set_frameindex(frameIndex);
    
    bool tween = frame->isTween();
    protoBufFrame->set_tween(tween);
}


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
