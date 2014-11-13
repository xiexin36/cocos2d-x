/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 
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

#include "FlatBuffersSerialize.h"

#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "tinyxml2/tinyxml2.h"

#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/util.h"

#include "CSParseBinary_generated.h"



USING_NS_CC;
using namespace cocos2d::ui;
using namespace cocostudio;
using namespace cocostudio::timeline;
using namespace flatbuffers;

static const char* FrameType_VisibleFrame       = "VisibleFrame";
static const char* FrameType_PositionFrame      = "PositionFrame";
static const char* FrameType_ScaleFrame         = "ScaleFrame";
static const char* FrameType_RotationSkewFrame  = "RotationSkewFrame";
static const char* FrameType_AnchorFrame        = "AnchorPointFrame";
static const char* FrameType_ColorFrame         = "ColorFrame";
static const char* FrameType_TextureFrame       = "TextureFrame";
static const char* FrameType_EventFrame         = "EventFrame";
static const char* FrameType_ZOrderFrame        = "ZOrderFrame";

static FlatBuffersSerialize* sharedFlatBuffersSerialize = nullptr;

FlatBuffersSerialize* FlatBuffersSerialize::getInstance()
{
    if (!sharedFlatBuffersSerialize)
    {
        sharedFlatBuffersSerialize = new FlatBuffersSerialize();
    }
    
    return sharedFlatBuffersSerialize;
}

FlatBuffersSerialize::FlatBuffersSerialize()
: _isSimulator(false)
, _builder(nullptr)
, _csparsebinary(nullptr)
{
    
}

FlatBuffersSerialize::~FlatBuffersSerialize()
{
    purge();
}

void FlatBuffersSerialize::purge()
{
    CC_SAFE_DELETE(sharedFlatBuffersSerialize);
}

/*
std::string FlatBuffersSerialize::test(const std::string &xmlFileName,
                                       const std::string &flatbuffersFileName)
{
    FlatBufferBuilder builder;
    _builder = &builder;
    CSParseBinaryBuilder cspbb(builder);
    
    
    std::vector<flatbuffers::Offset<flatbuffers::String>> textures;
    textures.push_back(builder.CreateString("1"));
    textures.push_back(builder.CreateString("2"));
    textures.push_back(builder.CreateString("3"));
    
    std::vector<flatbuffers::Offset<flatbuffers::String>> texturePngs;
    texturePngs.push_back(builder.CreateString("1"));
    
    
    auto mloc = CreateCSParseBinary(builder,
                                    builder.CreateVector(textures),
                                    builder.CreateVector(texturePngs));
    builder.Finish(mloc);
    
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(flatbuffersFileName);
    auto save = flatbuffers::SaveFile(fullPath.c_str(),
                                      reinterpret_cast<const char *>(builder.GetBufferPointer()),
                                      builder.GetSize(),
                                      true);
    if (!save)
    {
        CCLOG("couldn't save files!");
        return "couldn't save files!";
    }
    
    std::string csbContent;
    auto load = flatbuffers::LoadFile(fullPath.c_str(), true, &csbContent);
    if (!load)
    {
        CCLOG("couldn't load files");
    }
    auto csparsebinary = GetCSParseBinary(csbContent.data());
    
    int size = csparsebinary->textures()->size();
    for (int i = 0; i < size; ++i)
    {
        const flatbuffers::String* temp = csparsebinary->textures()->Get(i);
        CCLOG("temp = %s", temp->c_str());
    }
    
    
    return "";
}
*/

std::string FlatBuffersSerialize::serializeFlatBuffersWithXMLFile(const std::string &xmlFileName,
                                                                  const std::string &flatbuffersFileName)
{
    
    std::string inFullpath = FileUtils::getInstance()->fullPathForFilename(xmlFileName).c_str();
    
    // xml read
    if (!FileUtils::getInstance()->isFileExist(inFullpath))
    {
        return ".csd file doesn not exists ";
    }
    
    ssize_t size;
    std::string content =(char*)FileUtils::getInstance()->getFileData(inFullpath, "r", &size);
    
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
            
            //
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
    
    if (serializeEnabled)
    {
        FlatBufferBuilder builder;
        _builder = &builder;
        
        Offset<NodeTree> nodeTree;
        Offset<NodeAction> aciton;
        
        
        const tinyxml2::XMLElement* child = element->FirstChildElement();
        
        while (child)
        {
            std::string name = child->Name();
            
            if (name == "Animation") // action
            {
                const tinyxml2::XMLElement* animation = child;
                aciton = createNodeAction(animation);
            }
            else if (name == "ObjectData") // nodeTree
            {
                const tinyxml2::XMLElement* objectData = child;
                nodeTree = createNodeTree(objectData, rootType);
            }
            
            child = child->NextSiblingElement();
        }
        
        auto csparsebinary = CreateCSParseBinary(builder,
                                                 builder.CreateVector(_textures),
                                                 builder.CreateVector(_texturePngs),
                                                 nodeTree,
                                                 aciton);
        builder.Finish(csparsebinary);        
        
        
        std::string outFullPath = FileUtils::getInstance()->fullPathForFilename(flatbuffersFileName);
        size_t pos = outFullPath.find_last_of('.');
        std::string convert = outFullPath.substr(0, pos).append(".csb");
        auto save = flatbuffers::SaveFile(convert.c_str(),
                                          reinterpret_cast<const char *>(builder.GetBufferPointer()),
                                          builder.GetSize(),
                                          true);
        if (!save)
        {
            return "couldn't save files!";
        }
    }
    
    return "";
}

// NodeTree
Offset<NodeTree> FlatBuffersSerialize::createNodeTree(const tinyxml2::XMLElement *objectData,
                                                      std::string classType)
{
    std::string classname = classType.substr(0, classType.find("ObjectData"));
    CCLOG("classname = %s", classname.c_str());
    
    std::string name = "";
    
    Offset<Options> options;
    std::vector<Offset<NodeTree>> children;
    
    if (classname == "Node")
    {
        auto nodeOptions = createNodeOptions(objectData);
        options = CreateOptions(*_builder, nodeOptions);
    }
    else if (classname == "SingleNode")
    {
        auto nodeOptions = createNodeOptions(objectData);
        auto singleNodeOptions = createSingleNodeOptions(objectData);
        options = CreateOptions(*_builder,
                                nodeOptions,
                                singleNodeOptions);
    }
    else if (classname == "Sprite")
    {
        auto nodeOptions = createNodeOptions(objectData);
        auto spriteOptions = createSpriteOptions(objectData);
        options = CreateOptions(*_builder,
                                nodeOptions,
                                0, // SingleNodeOptions
                                spriteOptions);
    }
    else if (classname == "Particle")
    {
        auto nodeOptions = createNodeOptions(objectData);
        auto particleOptions = createParticleOptions(objectData);
        options = CreateOptions(*_builder,
                                nodeOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                particleOptions);
    }
    else if (classname == "GameMap")
    {
        auto nodeOptions = createNodeOptions(objectData);
        auto tmxTiledMapOptions = createTMXTiledMapOptions(objectData);
        options = CreateOptions(*_builder,
                                nodeOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                tmxTiledMapOptions);
    }
    else if (classname == "Button")
    {
        auto widgetOptions = createWidgetOptions(objectData);
        auto buttonOptions = createButtonOptions(objectData);
        options = CreateOptions(*_builder,
                                widgetOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                buttonOptions);
    }
    else if (classname == "CheckBox")
    {
        auto widgetOptions = createWidgetOptions(objectData);
        auto checkBoxOptions = createCheckBoxOptions(objectData);
        options = CreateOptions(*_builder,
                                widgetOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                checkBoxOptions);
    }
    else if (classname == "ImageView")
    {
        auto widgetOptions = createWidgetOptions(objectData);
        auto imageViewOptions = createImageViewOptions(objectData);
        options = CreateOptions(*_builder,
                                widgetOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                0, // CheckBoxOptions
                                imageViewOptions);
    }
    else if (classname == "TextAtlas")
    {
        auto widgetOptions = createWidgetOptions(objectData);
        auto textAtlasOptions = createTextAltasOptions(objectData);
        
        options = CreateOptions(*_builder,
                                widgetOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                0, // CheckBoxOptions
                                0, // ImageViewOptions
                                0, // TextBMFontOptions
                                0, // TextOptions
                                0, // TextFieldOptions
                                0, // LoadingBarOptions
                                0, // SliderOptions
                                0, // PanelOptions
                                0, // ScrollViewOptions
                                0, // PageViewOptions
                                0, // ListViewOptions
                                0, // ProjectNodeOptions
                                0, // ComponentOptions
                                textAtlasOptions);
    }
    else if (classname == "TextBMFont")
    {
        auto widgetOptions = createWidgetOptions(objectData);
        auto textBMFontOptions = createTextBMFontOptions(objectData);
        options = CreateOptions(*_builder,
                                widgetOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                0, // CheckBoxOptions
                                0, // ImageViewOptions
                                textBMFontOptions);
    }
    else if (classname == "Text")
    {
        auto widgetOptions = createWidgetOptions(objectData);
        auto textOptions = createTextOptions(objectData);
        options = CreateOptions(*_builder,
                                widgetOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                0, // CheckBoxOptions
                                0, // ImageViewOptions
                                0, // TextBMFontOptions
                                textOptions);
    }
    else if (classname == "TextField")
    {
        auto widgetOptions = createWidgetOptions(objectData);
        auto textFiledOptions = createTextFiledOptions(objectData);
        options = CreateOptions(*_builder,
                                widgetOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                0, // CheckBoxOptions
                                0, // ImageViewOptions
                                0, // TextBMFontOptions
                                0, // TextOptions
                                textFiledOptions);
    }
    else if (classname == "LoadingBar")
    {
        auto widgetOptions = createWidgetOptions(objectData);
        auto loadingBarOptions = createLoadingBarOptions(objectData);
        options = CreateOptions(*_builder,
                                widgetOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                0, // CheckBoxOptions
                                0, // ImageViewOptions
                                0, // TextBMFontOptions
                                0, // TextOptions
                                0, // TextFieldOptions
                                loadingBarOptions);
    }
    else if (classname == "Slider")
    {
        auto widgetOptions = createWidgetOptions(objectData);
        auto sliderOptions = createSliderOptions(objectData);
        options = CreateOptions(*_builder,
                                widgetOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                0, // CheckBoxOptions
                                0, // ImageViewOptions
                                0, // TextBMFontOptions
                                0, // TextOptions
                                0, // TextFieldOptions
                                0, // LoadingBarOptions
                                sliderOptions);
    }
    else if (classname == "Panel")
    {
        auto widgetOptions = createWidgetOptions(objectData);
        auto layoutOptions = createLayoutOptions(objectData);
        options = CreateOptions(*_builder,
                                widgetOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                0, // CheckBoxOptions
                                0, // ImageViewOptions
                                0, // TextBMFontOptions
                                0, // TextOptions
                                0, // TextFieldOptions
                                0, // LoadingBarOptions
                                0, // SliderOptions
                                layoutOptions);
    }
    else if (classname == "ScrollView")
    {
        auto widgetOptions = createWidgetOptions(objectData);
        auto scrollViewOptions = createScrollViewOptions(objectData);
        options = CreateOptions(*_builder,
                                widgetOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                0, // CheckBoxOptions
                                0, // ImageViewOptions
                                0, // TextBMFontOptions
                                0, // TextOptions
                                0, // TextFieldOptions
                                0, // LoadingBarOptions
                                0, // SliderOptions
                                0, // LayoutOptions
                                scrollViewOptions);
    }
    else if (classname == "PageView")
    {
        auto widgetOptions = createWidgetOptions(objectData);
        auto pageViewOptions = createPageViewOptions(objectData);
        options = CreateOptions(*_builder,
                                widgetOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                0, // CheckBoxOptions
                                0, // ImageViewOptions
                                0, // TextBMFontOptions
                                0, // TextOptions
                                0, // TextFieldOptions
                                0, // LoadingBarOptions
                                0, // SliderOptions
                                0, // LayoutOptions
                                0, // ScrollViewOptions
                                pageViewOptions);
    }
    else if (classname == "ListView")
    {
        auto widgetOptions = createWidgetOptions(objectData);
        auto listViewOptions = createListViewOptions(objectData);
        options = CreateOptions(*_builder,
                                widgetOptions,
                                0, // SingleNodeOptions,
                                0, // SpriteOptions,
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                0, // CheckBoxOptions
                                0, // ImageViewOptions
                                0, // TextBMFontOptions
                                0, // TextOptions
                                0, // TextFieldOptions
                                0, // LoadingBarOptions
                                0, // SliderOptions
                                0, // LayoutOptions
                                0, // ScrollViewOptions
                                0, // PageViewOptions
                                listViewOptions);
    }
    else if (classname == "ProjectNode")
    {
        auto nodeOptions = createNodeOptions(objectData);
        auto projectNodeOptions = createProjectNodeOptions(objectData);
        options = CreateOptions(*_builder,
                                nodeOptions,
                                0, // SingleNodeOptions
                                0, // SpriteOptions
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                0, // CheckBoxOptions
                                0, // ImageViewOptions
                                0, // TextBMFontOptions
                                0, // TextOptions
                                0, // TextFieldOptions
                                0, // LoadingBarOptions
                                0, // SliderOptions
                                0, // PanelOptions
                                0, // ScrollViewOptions
                                0, // PageViewOptions
                                0, // ListViewOptions
                                projectNodeOptions);
    }
    else if (classname == "SimpleAudio")
    {
        auto nodeOptions = createNodeOptions(objectData);
        
        std::string type = "ComAudio";
        auto componentOptions = CreateComponentOptions(*_builder,
                                                       _builder->CreateString(type),
                                                       createComAudioOptions(objectData));
        options = CreateOptions(*_builder,
                                nodeOptions,
                                0, // SingleNodeOptions
                                0, // SpriteOptions
                                0, // ParticleSystemOptions
                                0, // TMXTiledMapOptions
                                0, // ButtonOptions
                                0, // CheckBoxOptions
                                0, // ImageViewOptions
                                0, // TextBMFontOptions
                                0, // TextOptions
                                0, // TextFieldOptions
                                0, // LoadingBarOptions
                                0, // SliderOptions
                                0, // PanelOptions
                                0, // ScrollViewOptions
                                0, // PageViewOptions
                                0, // ListViewOptions
                                0, // ProjectNodeOptions
                                componentOptions);
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
            const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
            bool bHasType = false;
            while (attribute)
            {
                std::string attriname = attribute->Name();
                std::string value = attribute->Value();
                
                if (attriname == "ctype")
                {
                    children.push_back(createNodeTree(objectData, value));
                    
                    bHasType = true;
                    break;
                }
                
                attribute = attribute->Next();
            }
            
            if(!bHasType)
            {
                children.push_back(createNodeTree(objectData, "NodeObjectData"));
            }
            
            objectData = objectData->NextSiblingElement();
        }
    }
    //
    
    
    return CreateNodeTree(*_builder,
                          _builder->CreateString(classname),
                          _builder->CreateVector(children),
                          options);
}

Offset<WidgetOptions> FlatBuffersSerialize::createNodeOptions(const tinyxml2::XMLElement *objectData)
{
    std::string name = "";
    long actionTag = 0;
    Vec2 rotationSkew = Vec2::ZERO;
    int zOrder = 0;
    bool visible = true;
    GLubyte alpha = 255;
    int tag = 0;
    Vec2 position = Vec2::ZERO;
    Vec2 scale = Vec2(1.0f, 1.0f);
    Vec2 anchorPoint = Vec2::ZERO;
    Color4B color = { 255, 255, 255, 255 };
    Vec2 size = Vec2::ZERO;
    bool flipX = false;
    bool flipY = false;
    bool ignoreSize = false;
    bool touchEnabled = false;
    std::string frameEvent = "";
    std::string customProperty = "";
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string attriname = attribute->Name();
        std::string value = attribute->Value();
        
        if (attriname == "Name")
        {
            name = value;
        }
        else if (attriname == "ActionTag")
        {
            actionTag = atol(value.c_str());
        }
        else if (attriname == "RotationSkewX")
        {
            rotationSkew.x = atof(value.c_str());
        }
        else if (attriname == "RotationSkewY")
        {
            rotationSkew.y = atof(value.c_str());
        }
        else if (attriname == "Rotation")
        {
//            rotation = atoi(value.c_str());
        }
        else if (attriname == "ZOrder")
        {
            zOrder = atoi(value.c_str());
        }
        else if (attriname == "Visible")
        {
//            visible = (value == "True") ? true : false;
        }
        else if (attriname == "VisibleForFrame")
        {
            visible = (value == "True") ? true : false;
        }
        else if (attriname == "Alpha")
        {
            alpha = atoi(value.c_str());
        }
        else if (attriname == "Tag")
        {
            tag = atoi(value.c_str());
        }
        else if (attriname == "TouchEnable")
        {
            touchEnabled = (value == "True") ? true : false;
        }
        else if (attriname == "FrameEvent")
        {
            frameEvent = value;
        }
        
        attribute = attribute->Next();
    }
    
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string attriname = child->Name();
        if (attriname == "Children")
        {
            break;
        }
        else if (attriname == "Position")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                attriname = attribute->Name();
                std::string value = attribute->Value();
                
                if (attriname == "X")
                {
                    position.x = atof(value.c_str());
                }
                else if (attriname == "Y")
                {
                    position.y = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (attriname == "Scale")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                attriname = attribute->Name();
                std::string value = attribute->Value();
                
                if (attriname == "ScaleX")
                {
                    scale.x = atof(value.c_str());
                }
                else if (attriname == "ScaleY")
                {
                    scale.y = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (attriname == "AnchorPoint")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                attriname = attribute->Name();
                std::string value = attribute->Value();
                
                if (attriname == "ScaleX")
                {
                    anchorPoint.x = atof(value.c_str());
                }
                else if (attriname == "ScaleY")
                {
                    anchorPoint.y = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (attriname == "CColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                attriname = attribute->Name();
                std::string value = attribute->Value();
                
                if (attriname == "A")
                {
                    color.a = atoi(value.c_str());
                }
                else if (attriname == "R")
                {
                    color.r = atoi(value.c_str());
                }
                else if (attriname == "G")
                {
                    color.g = atoi(value.c_str());
                }
                else if (attriname == "B")
                {
                    color.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (attriname == "Size")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                attriname = attribute->Name();
                std::string value = attribute->Value();
                
                if (attriname == "X")
                {
                    size.x = atof(value.c_str());
                }
                else if (attriname == "Y")
                {
                    size.y = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    RotationSkew f_rotationskew(rotationSkew.x, rotationSkew.y);
    Position f_position(position.x, position.y);
    Scale f_scale(scale.x, scale.y);
    AnchorPoint f_anchortpoint(anchorPoint.x, anchorPoint.y);
    Color f_color(color.a, color.r, color.g, color.b);
    FlatSize f_size(size.x, size.y);
    
    return CreateWidgetOptions(*_builder,
                               _builder->CreateString(name),
                               actionTag,
                               &f_rotationskew,
                               zOrder,
                               visible,
                               alpha,
                               tag,
                               &f_position,
                               &f_scale,
                               &f_anchortpoint,
                               &f_color,
                               &f_size,
                               flipX,
                               flipY,
                               ignoreSize,
                               touchEnabled,
                               _builder->CreateString(frameEvent),
                               _builder->CreateString(customProperty)
                               );
}

Offset<SingleNodeOptions> FlatBuffersSerialize::createSingleNodeOptions(const tinyxml2::XMLElement *objectData)
{
    return CreateSingleNodeOptions(*_builder);
}

Offset<SpriteOptions> FlatBuffersSerialize::createSpriteOptions(const tinyxml2::XMLElement *objectData)
{
    std::string path = "";
    std::string plistFile = "";
    int resourceType = 0;
    
    // FileData
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "FileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateSpriteOptions(*_builder,
                               CreateResourceData(*_builder,
                                                  _builder->CreateString(path),
                                                  _builder->CreateString(plistFile),
                                                  resourceType));
}

Offset<ParticleSystemOptions> FlatBuffersSerialize::createParticleOptions(const tinyxml2::XMLElement *objectData)
{
    std::string path = "";
    std::string plistFile = "";
    int resourceType = 0;
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
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
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateParticleSystemOptions(*_builder,
                                       CreateResourceData(*_builder,
                                                          _builder->CreateString(path),
                                                          _builder->CreateString(plistFile),
                                                          resourceType));
}

Offset<TMXTiledMapOptions> FlatBuffersSerialize::createTMXTiledMapOptions(const tinyxml2::XMLElement *objectData)
{
    std::string path = "";
    std::string plistFile = "";
    int resourceType = 0;
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
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
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateTMXTiledMapOptions(*_builder,
                                    CreateResourceData(*_builder,
                                                       _builder->CreateString(path),
                                                       _builder->CreateString(plistFile),
                                                       resourceType));
}

Offset<WidgetOptions> FlatBuffersSerialize::createWidgetOptions(const tinyxml2::XMLElement *objectData)
{
    std::string name = "";
    long actionTag = 0;
    Vec2 rotationSkew = Vec2::ZERO;
    int zOrder = 0;
    bool visible = true;
    GLubyte alpha = 255;
    int tag = 0;
    Vec2 position = Vec2::ZERO;
    Vec2 scale = Vec2(1.0f, 1.0f);
    Vec2 anchorPoint = Vec2::ZERO;
    Color4B color = { 255, 255, 255, 255 };
    Vec2 size = Vec2::ZERO;
    bool flipX = false;
    bool flipY = false;
    bool ignoreSize = false;
    bool touchEnabled = false;
    std::string frameEvent = "";
    std::string customProperty = "";
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string attriname = attribute->Name();
        std::string value = attribute->Value();
        
        if (attriname == "Name")
        {
            name = value;
        }
        else if (attriname == "ActionTag")
        {
            actionTag = atol(value.c_str());
        }
        else if (attriname == "RotationSkewX")
        {
            rotationSkew.x = atof(value.c_str());
        }
        else if (attriname == "RotationSkewY")
        {
            rotationSkew.y = atof(value.c_str());
        }
        else if (attriname == "Rotation")
        {
//            rotation = atoi(value.c_str());
        }
        else if (attriname == "ZOrder")
        {
            zOrder = atoi(value.c_str());
        }
        else if (attriname == "Visible")
        {
//            visible = (value == "True") ? true : false;
        }
        else if (attriname == "VisibleForFrame")
        {
            visible = (value == "True") ? true : false;
        }
        else if (attriname == "Alpha")
        {
            alpha = atoi(value.c_str());
        }
        else if (attriname == "Tag")
        {
            tag = atoi(value.c_str());
        }
        else if (attriname == "TouchEnable")
        {
            touchEnabled = (value == "True") ? true : false;
        }
        else if (attriname == "FrameEvent")
        {
            frameEvent = value;
        }
        
        attribute = attribute->Next();
    }
    
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string attriname = child->Name();
        if (attriname == "Children")
        {
            break;
        }
        else if (attriname == "Position")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                attriname = attribute->Name();
                std::string value = attribute->Value();
                
                if (attriname == "X")
                {
                    position.x = atof(value.c_str());
                }
                else if (attriname == "Y")
                {
                    position.y = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (attriname == "Scale")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                attriname = attribute->Name();
                std::string value = attribute->Value();
                
                if (attriname == "ScaleX")
                {
                    scale.x = atof(value.c_str());
                }
                else if (attriname == "ScaleY")
                {
                    scale.y = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (attriname == "AnchorPoint")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                attriname = attribute->Name();
                std::string value = attribute->Value();
                
                if (attriname == "ScaleX")
                {
                    anchorPoint.x = atof(value.c_str());
                }
                else if (attriname == "ScaleY")
                {
                    anchorPoint.y = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (attriname == "CColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                attriname = attribute->Name();
                std::string value = attribute->Value();
                
                if (attriname == "A")
                {
                    color.a = atoi(value.c_str());
                }
                else if (attriname == "R")
                {
                    color.r = atoi(value.c_str());
                }
                else if (attriname == "G")
                {
                    color.g = atoi(value.c_str());
                }
                else if (attriname == "B")
                {
                    color.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (attriname == "Size")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                attriname = attribute->Name();
                std::string value = attribute->Value();
                
                if (attriname == "X")
                {
                    size.x = atof(value.c_str());
                }
                else if (attriname == "Y")
                {
                    size.y = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    RotationSkew f_rotationskew(rotationSkew.x, rotationSkew.y);
    Position f_position(position.x, position.y);
    Scale f_scale(scale.x, scale.y);
    AnchorPoint f_anchortpoint(anchorPoint.x, anchorPoint.y);
    Color f_color(color.a, color.r, color.g, color.b);
    FlatSize f_size(size.x, size.y);
    
    return CreateWidgetOptions(*_builder,
                               _builder->CreateString(name),
                               actionTag,
                               &f_rotationskew,
                               zOrder,
                               visible,
                               alpha,
                               tag,
                               &f_position,
                               &f_scale,
                               &f_anchortpoint,
                               &f_color,
                               &f_size,
                               flipX,
                               flipY,
                               ignoreSize,
                               touchEnabled,
                               _builder->CreateString(frameEvent),
                               _builder->CreateString(customProperty)
                               );
}

Offset<ButtonOptions> FlatBuffersSerialize::createButtonOptions(const tinyxml2::XMLElement *objectData)
{
    bool displaystate = true;
    bool scale9Enabled = false;
    Rect capInsets;
    std::string text = "";
    int fontSize = 14;
    std::string fontName = "微软雅黑";
    cocos2d::Size scale9Size;
    Color4B textColor(255, 255, 255, 255);

    std::string normalPath = "";
    std::string normalPlistFile = "";
    int normalResourceType = 0;
    
    std::string pressedPath = "";
    std::string pressedPlistFile = "";
    int pressedResourceType = 0;
    
    std::string disabledPath = "";
    std::string disabledPlistFile = "";
    int disabledResourceType = 0;
    
    std::string fontResourcePath = "";
    std::string fontResourcePlistFile = "";
    int fontResourceResourceType = 0;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
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
        }
        else if (name == "Scale9OriginX")
        {
            capInsets.origin.x = atof(value.c_str());
        }
        else if (name == "Scale9OriginY")
        {
            capInsets.origin.y = atof(value.c_str());
        }
        else if (name == "Scale9Width")
        {
            capInsets.size.width = atof(value.c_str());
        }
        else if (name == "Scale9Height")
        {
            capInsets.size.height = atof(value.c_str());
        }
        else if (name == "ButtonText")
        {
            text = value;
        }
        else if (name == "FontSize")
        {
            fontSize = atoi(value.c_str());
        }
        else if (name == "FontName")
        {
            fontName = value;
        }
        else if (name == "DisplayState")
        {
            displaystate = (value == "True") ? true : false;
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "Size" && scale9Enabled)
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    scale9Size.width = atof(value.c_str());
                }
                else if (name == "Y")
                {
                    scale9Size.height = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "TextColor")
        {
            attribute = child->FirstAttribute();
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    textColor.r = atoi(value.c_str());
                }
                else if (name == "G")
                {
                    textColor.g = atoi(value.c_str());
                }
                else if (name == "B")
                {
                    textColor.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "DisabledFileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    disabledPath = value;
                }
                else if (name == "Type")
                {
                    disabledResourceType = getResourceType(value);;
                }
                else if (name == "Plist")
                {
                    disabledPlistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (disabledResourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        else if (name == "PressedFileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    pressedPath = value;
                }
                else if (name == "Type")
                {
                    pressedResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    pressedPlistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (pressedResourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        else if (name == "NormalFileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    normalPath = value;
                }
                else if (name == "Type")
                {
                    normalResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    normalPlistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (normalResourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        else if (name == "FontResource")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    fontResourcePath = value;
                }
                else if (name == "Type")
                {
                    fontResourceResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    fontResourcePlistFile = value;
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    Color f_textColor(255, textColor.r, textColor.g, textColor.b);
    CapInsets f_capInsets(capInsets.origin.x, capInsets.origin.y, capInsets.size.width, capInsets.size.height);
    FlatSize f_scale9Size(scale9Size.width, scale9Size.height);
    
    return CreateButtonOptions(*_builder,
                               CreateResourceData(*_builder,
                                                  _builder->CreateString(normalPath),
                                                  _builder->CreateString(normalPlistFile),
                                                  normalResourceType),
                               CreateResourceData(*_builder,
                                                  _builder->CreateString(pressedPath),
                                                  _builder->CreateString(pressedPlistFile),
                                                  pressedResourceType),
                               CreateResourceData(*_builder,
                                                  _builder->CreateString(disabledPath),
                                                  _builder->CreateString(disabledPlistFile),
                                                  disabledResourceType),
                               CreateResourceData(*_builder,
                                                  _builder->CreateString(fontResourcePath),
                                                  _builder->CreateString(fontResourcePlistFile),
                                                  fontResourceResourceType),
                               _builder->CreateString(text),
                               _builder->CreateString(fontName),
                               fontSize,
                               &f_textColor,
                               &f_capInsets,
                               &f_scale9Size,
                               scale9Enabled,
                               displaystate
                               );
}

Offset<CheckBoxOptions> FlatBuffersSerialize::createCheckBoxOptions(const tinyxml2::XMLElement *objectData)
{
    bool selectedState = true;
    bool displaystate = true;
    
    int backgroundboxResourceType = 0;
    std::string backgroundboxPath = "";
    std::string backgroundboxPlistFile = "";
    
    int backGroundBoxSelectedResourceType = 0;
    std::string backGroundBoxSelectedPath = "";
    std::string backGroundBoxSelectedPlistFile = "";
    
    int frontCrossResourceType = 0;
    std::string frontCrossPath = "";
    std::string frontCrossPlistFile = "";
    
    int backGroundBoxDisabledResourceType = 0;
    std::string backGroundBoxDisabledPath = "";
    std::string backGroundBoxDisabledPlistFile = "";
    
    
    int frontCrossDisabledResourceType = 0;
    std::string frontCrossDisabledPath = "";
    std::string frontCrossDisabledPlistFile = "";
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "CheckedState")
        {
            selectedState = (value == "True") ? true : false;
        }
        else if (name == "DisplayState")
        {
            displaystate = (value == "True") ? true : false;
        }
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "NormalBackFileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    backgroundboxPath = value;
                }
                else if (name == "Type")
                {
                    backgroundboxResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    backgroundboxPlistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (backgroundboxResourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        else if (name == "PressedBackFileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    backGroundBoxSelectedPath = value;
                }
                else if (name == "Type")
                {
                    backGroundBoxSelectedResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    backGroundBoxSelectedPlistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (backGroundBoxSelectedResourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        else if (name == "NodeNormalFileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    frontCrossPath = value;
                }
                else if (name == "Type")
                {
                    frontCrossResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    frontCrossPlistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (frontCrossResourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        else if (name == "DisableBackFileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    backGroundBoxDisabledPath = value;
                }
                else if (name == "Type")
                {
                    backGroundBoxDisabledResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    backGroundBoxDisabledPlistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (backGroundBoxDisabledResourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        else if (name == "NodeDisableFileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    frontCrossDisabledPath = value;
                }
                else if (name == "Type")
                {
                    frontCrossDisabledResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    frontCrossDisabledPlistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (frontCrossDisabledResourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateCheckBoxOptions(*_builder,
                                 CreateResourceData(*_builder,
                                                    _builder->CreateString(backgroundboxPath),
                                                    _builder->CreateString(backgroundboxPlistFile),
                                                    backgroundboxResourceType),
                                 CreateResourceData(*_builder,
                                                    _builder->CreateString(backGroundBoxSelectedPath),
                                                    _builder->CreateString(backGroundBoxSelectedPlistFile),
                                                    backGroundBoxSelectedResourceType),
                                 CreateResourceData(*_builder,
                                                    _builder->CreateString(frontCrossPath),
                                                    _builder->CreateString(frontCrossPlistFile),
                                                    frontCrossResourceType),
                                 CreateResourceData(*_builder,
                                                    _builder->CreateString(backGroundBoxDisabledPath),
                                                    _builder->CreateString(backGroundBoxDisabledPlistFile),
                                                    backGroundBoxDisabledResourceType),
                                 CreateResourceData(*_builder,
                                                    _builder->CreateString(frontCrossDisabledPath),
                                                    _builder->CreateString(frontCrossDisabledPlistFile),
                                                    frontCrossDisabledResourceType),
                                 selectedState,
                                 displaystate
                                 );
}

Offset<ImageViewOptions> FlatBuffersSerialize::createImageViewOptions(const tinyxml2::XMLElement *objectData)
{
    bool scale9Enabled = false;
    Rect capInsets;
    cocos2d::Size scale9Size;
    
    std::string path = "";
    std::string plistFile = "";
    int resourceType = 0;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
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
        }
        else if (name == "Scale9OriginX")
        {
            capInsets.origin.x = atof(value.c_str());
        }
        else if (name == "Scale9OriginY")
        {
            capInsets.origin.y = atof(value.c_str());
        }
        else if (name == "Scale9Width")
        {
            capInsets.size.width = atof(value.c_str());
        }
        else if (name == "Scale9Height")
        {
            capInsets.size.height = atof(value.c_str());
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "Size" && scale9Enabled)
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    scale9Size.width = atof(value.c_str());
                }
                else if (name == "Y")
                {
                    scale9Size.height = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    CapInsets f_capInsets(capInsets.origin.x, capInsets.origin.y, capInsets.size.width, capInsets.size.height);
    FlatSize f_scale9Size(scale9Size.width, scale9Size.height);
    
    return CreateImageViewOptions(*_builder,
                                  CreateResourceData(*_builder,
                                                     _builder->CreateString(path),
                                                     _builder->CreateString(plistFile),
                                                     resourceType),
                                  &f_capInsets,
                                  &f_scale9Size,
                                  scale9Enabled);
}

Offset<TextBMFontOptions> FlatBuffersSerialize::createTextBMFontOptions(const tinyxml2::XMLElement *objectData)
{
    std::string text = "Fnt Text Label";
    
    std::string path = "";
    std::string plistFlie = "";
    int resourceType = 0;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "LabelText")
        {
            text = value;
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "LabelBMFontFile_CNB")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFlie = value;
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateTextBMFontOptions(*_builder,
                                   CreateResourceData(*_builder,
                                                      _builder->CreateString(path),
                                                      _builder->CreateString(plistFlie),
                                                      resourceType),
                                   _builder->CreateString(text));
}

Offset<TextOptions> FlatBuffersSerialize::createTextOptions(const tinyxml2::XMLElement *objectData)
{
    bool touchScaleEnabled = false;
    bool isCustomSize = false;
    std::string fontName = "微软雅黑";
    int fontSize = 20;
    std::string text = "Text Label";
    int areaWidth = 0;
    int areaHeight = 0;
    int h_alignment = 0;
    int v_alignment = 0;
    
    std::string path = "";
    std::string plistFile = "";
    int resourceType = 0;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "TouchScaleChangeAble")
        {
            touchScaleEnabled = (value == "True") ? true : false;
        }
        else if (name == "LabelText")
        {
            text = value;
        }
        else if (name == "FontSize")
        {
            fontSize = atoi(value.c_str());
        }
        else if (name == "FontName")
        {
            fontName = value;
        }
        else if (name == "AreaWidth")
        {
            areaWidth = atoi(value.c_str());
        }
        else if (name == "AreaHeight")
        {
            areaHeight = atoi(value.c_str());
        }
        else if (name == "HorizontalAlignmentType")
        {
            if (value == "HT_Left")
            {
                h_alignment = 0;
            }
            else if (value == "HT_Center")
            {
                h_alignment = 1;
            }
            else if (value == "HT_Right")
            {
                h_alignment = 2;
            }
        }
        else if (name == "VerticalAlignmentType")
        {
            if (value == "VT_Top")
            {
                v_alignment = 0;
            }
            else if (value == "VT_Center")
            {
                v_alignment = 1;
            }
            else if (value == "VT_Bottom")
            {
                v_alignment = 2;
            }
        }
        else if (name == "IsCustomSize")
        {
            isCustomSize = (value == "True") ? true : false;
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "FontResource")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateTextOptions(*_builder,
                             CreateResourceData(*_builder,
                                                 _builder->CreateString(path),
                                                 _builder->CreateString(plistFile),
                                                 resourceType),
                             _builder->CreateString(fontName),
                             fontSize,
                             _builder->CreateString(text),
                             areaWidth,
                             areaHeight,
                             h_alignment,
                             v_alignment,
                             touchScaleEnabled,
                             isCustomSize);
}

Offset<TextFieldOptions> FlatBuffersSerialize::createTextFiledOptions(const tinyxml2::XMLElement *objectData)
{
    std::string path = "";
    std::string plistFile = "";
    int resourceType = 0;
    
    std::string fontName = "微软雅黑";
    int fontSize = 20;
    std::string text = "";
    std::string placeHolder = "Text Field";
    bool passwordEnabled = false;
    std::string passwordStyleText = "*";
    bool maxLengthEnabled = false;
    int maxLength = 10;
    int areaWidth = 0;
    int areaHeight = 0;
    bool isCustomSize = false;
    
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "PlaceHolderText")
        {
            placeHolder = value;
        }
        else if (name == "LabelText")
        {
            text = value;
        }
        else if (name == "FontSize")
        {
            fontSize = atoi(value.c_str());
        }
        else if (name == "FontName")
        {
            fontName = value;
        }
        else if (name == "MaxLengthEnable")
        {
            maxLengthEnabled = (value == "True") ? true : false;
        }
        else if (name == "MaxLengthText")
        {
            maxLength = atoi(value.c_str());
        }
        else if (name == "PasswordEnable")
        {
            passwordEnabled = (value == "True") ? true : false;
        }
        else if (name == "PasswordStyleText")
        {
            passwordStyleText = value;
        }
        else if (name == "IsCustomSize")
        {
            isCustomSize = (value == "True") ? true : false;
        }
        
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "FontResource")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateTextFieldOptions(*_builder,
                                  CreateResourceData(*_builder,
                                                     _builder->CreateString(path),
                                                     _builder->CreateString(plistFile),
                                                     resourceType),
                                  _builder->CreateString(fontName),
                                  fontSize,
                                  _builder->CreateString(text),
                                  _builder->CreateString(placeHolder),
                                  passwordEnabled,
                                  _builder->CreateString(passwordStyleText),
                                  maxLengthEnabled,
                                  maxLength,
                                  areaWidth,
                                  areaHeight,
                                  isCustomSize
                                  );
}

Offset<LoadingBarOptions> FlatBuffersSerialize::createLoadingBarOptions(const tinyxml2::XMLElement *objectData)
{
    std::string path = "";
    std::string plistFile = "";
    int resourceType = 0;
    
    int percent = 80;
    int direction = 0;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "ProgressType")
        {
            direction = (value == "Left_To_Right") ? 0 : 1;
        }
        else if (name == "ProgressInfo")
        {
            percent = atoi(value.c_str());
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "ImageFileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateLoadingBarOptions(*_builder,
                                   CreateResourceData(*_builder,
                                                      _builder->CreateString(path),
                                                      _builder->CreateString(plistFile),
                                                      resourceType),
                                   percent,
                                   direction);
}

Offset<SliderOptions> FlatBuffersSerialize::createSliderOptions(const tinyxml2::XMLElement *objectData)
{
    std::string barFileNamePath = "";
    std::string barFileNamePlistFile = "";
    int barFileNameResourceType = 0;
    
    std::string ballNormalPath = "";
    std::string ballNormalPlistFile = "";
    int ballNormalResourceType = 0;
    
    std::string ballPressedPath = "";
    std::string ballPressedPlistFile = "";
    int ballPressedResourceType = 0;
    
    std::string ballDisabledPath = "";
    std::string ballDisabledPlistFile = "";
    int ballDisabledResourceType = 0;
    
    std::string progressBarPath = "";
    std::string progressBarPlistFile = "";
    int progressBarResourceType = 0;
    
    int percent = 50;
    bool displaystate = true;
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "PercentInfo")
        {
            percent = atoi(value.c_str());
        }
        else if (name == "DisplayState")
        {
            displaystate = (value == "True") ? true : false;
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "BackGroundData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    barFileNamePath = value;
                }
                else if (name == "Type")
                {
                    barFileNameResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    barFileNamePlistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (barFileNameResourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        else if (name == "BallNormalData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    ballNormalPath = value;
                }
                else if (name == "Type")
                {
                    ballNormalResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    ballNormalPlistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (ballNormalResourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        else if (name == "BallPressedData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    ballPressedPath = value;
                }
                else if (name == "Type")
                {
                    ballPressedResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    ballPressedPlistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (ballPressedResourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        else if (name == "BallDisabledData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    ballDisabledPath = value;
                }
                else if (name == "Type")
                {
                    ballDisabledResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    ballDisabledPlistFile = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (ballDisabledResourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        else if (name == "ProgressBarData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    progressBarPath = value;
                }
                else if (name == "Type")
                {
                    progressBarResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    progressBarPlistFile = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (progressBarResourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateSliderOptions(*_builder,
                               CreateResourceData(*_builder,
                                                  _builder->CreateString(barFileNamePath),
                                                  _builder->CreateString(barFileNamePlistFile),
                                                  barFileNameResourceType),
                               CreateResourceData(*_builder,
                                                  _builder->CreateString(ballNormalPath),
                                                  _builder->CreateString(ballNormalPlistFile),
                                                  ballNormalResourceType),
                               CreateResourceData(*_builder,
                                                  _builder->CreateString(ballPressedPath),
                                                  _builder->CreateString(ballPressedPlistFile),
                                                  ballPressedResourceType),
                               CreateResourceData(*_builder,
                                                  _builder->CreateString(ballDisabledPath),
                                                  _builder->CreateString(ballDisabledPlistFile),
                                                  ballDisabledResourceType),
                               CreateResourceData(*_builder,
                                                  _builder->CreateString(progressBarPath),
                                                  _builder->CreateString(progressBarPlistFile),
                                                  progressBarResourceType),
                               percent,
                               displaystate);
}

Offset<PanelOptions> FlatBuffersSerialize::createLayoutOptions(const tinyxml2::XMLElement *objectData)
{
    std::string path = "";
    std::string plistFile = "";
    int resourceType = 0;
    
    bool clipEnabled = false;
    Color3B bgColor;
    Color3B bgStartColor;
    Color3B bgEndColor;
    int colorType = 0;
    GLubyte bgColorOpacity = 255;
    Vec2 colorVector(0.0f, -0.5f);
    Rect capInsets;
    Size scale9Size;
    bool backGroundScale9Enabled = false;
    
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "ClipAble")
        {
            clipEnabled = (value == "True") ? true : false;
        }
        else if (name == "ComboBoxIndex")
        {
            colorType = atoi(value.c_str());
        }
        else if (name == "BackColorAlpha")
        {
            bgColorOpacity = atoi(value.c_str());
        }
        else if (name == "Scale9Enable")
        {
            if (value == "True")
            {
                backGroundScale9Enabled = true;
            }
        }
        else if (name == "Scale9OriginX")
        {
            capInsets.origin.x = atof(value.c_str());
        }
        else if (name == "Scale9OriginY")
        {
            capInsets.origin.y = atof(value.c_str());
        }
        else if (name == "Scale9Width")
        {
            capInsets.size.width = atof(value.c_str());
        }
        else if (name == "Scale9Height")
        {
            capInsets.size.height = atof(value.c_str());
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "Size" && backGroundScale9Enabled)
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    scale9Size.width = atof(value.c_str());
                }
                else if (name == "Y")
                {
                    scale9Size.height = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "SingleColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    bgColor.r = atoi(value.c_str());
                }
                else if (name == "G")
                {
                    bgColor.g = atoi(value.c_str());
                }
                else if (name == "B")
                {
                    bgColor.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "EndColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    bgEndColor.r = atoi(value.c_str());
                }
                else if (name == "G")
                {
                    bgEndColor.g = atoi(value.c_str());
                }
                else if (name == "B")
                {
                    bgEndColor.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FirstColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    bgStartColor.r = atoi(value.c_str());
                }
                else if (name == "G")
                {
                    bgStartColor.g = atoi(value.c_str());
                }
                else if (name == "B")
                {
                    bgStartColor.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "ColorVector")
        {
            attribute = child->FirstAttribute();
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ScaleX")
                {
                    colorVector.x = atof(value.c_str());
                }
                else if (name == "ScaleY")
                {
                    colorVector.y = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    Color f_bgColor(255, bgColor.r, bgColor.g, bgColor.b);
    Color f_bgStartColor(255, bgStartColor.r, bgStartColor.g, bgStartColor.b);
    Color f_bgEndColor(255, bgEndColor.r, bgEndColor.g, bgEndColor.b);
    ColorVector f_colorVector(colorVector.x, colorVector.y);
    CapInsets f_capInsets(capInsets.origin.x, capInsets.origin.y, capInsets.size.width, capInsets.size.height);
    FlatSize f_scale9Size(scale9Size.width, scale9Size.height);
    
    return CreatePanelOptions(*_builder,
                              CreateResourceData(*_builder,
                                                 _builder->CreateString(path),
                                                 _builder->CreateString(plistFile),
                                                 resourceType),
                              clipEnabled,
                              &f_bgColor,
                              &f_bgStartColor,
                              &f_bgEndColor,
                              colorType,
                              bgColorOpacity,
                              &f_colorVector,
                              &f_capInsets,
                              &f_scale9Size,
                              backGroundScale9Enabled);
}

Offset<ScrollViewOptions> FlatBuffersSerialize::createScrollViewOptions(const tinyxml2::XMLElement *objectData)
{
    std::string path = "";
    std::string plistFile = "";
    int resourceType = 0;
    
    bool clipEnabled = false;
    Color3B bgColor;
    Color3B bgStartColor;
    Color3B bgEndColor;
    int colorType = 0;
    GLubyte bgColorOpacity = 255;
    Vec2 colorVector(0.0f, -0.5f);
    Rect capInsets;
    Size scale9Size;
    bool backGroundScale9Enabled = false;
    Size innerSize(200, 300);
    int direction = 0;
    bool bounceEnabled = false;
    
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "ClipAble")
        {
            clipEnabled = (value == "True") ? true : false;
        }
        else if (name == "ComboBoxIndex")
        {
            colorType = atoi(value.c_str());
        }
        else if (name == "BackColorAlpha")
        {
            bgColorOpacity = atoi(value.c_str());
        }
        else if (name == "Scale9Enable")
        {
            if (value == "True")
            {
                backGroundScale9Enabled = true;
            }
        }
        else if (name == "Scale9OriginX")
        {
            capInsets.origin.x = atof(value.c_str());
        }
        else if (name == "Scale9OriginY")
        {
            capInsets.origin.y = atof(value.c_str());
        }
        else if (name == "Scale9Width")
        {
            capInsets.size.width = atof(value.c_str());
        }
        else if (name == "Scale9Height")
        {
            capInsets.size.height = atof(value.c_str());
        }
        else if (name == "ScrollDirectionType")
        {
            if (value == "Vertical")
            {
                direction = 1;
            }
            else if (value == "Horizontal")
            {
                direction = 2;
            }
            else if (value == "Vertical_Horizontal")
            {
                direction = 3;
            }
        }
        else if (name == "IsBounceEnabled")
        {
            bounceEnabled = (value == "True") ? true : false;
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "InnerNodeSize")
        {
            attribute = child->FirstAttribute();
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Width")
                {
                    innerSize.width = atof(value.c_str());
                }
                else if (name == "Height")
                {
                    innerSize.height = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "Size" && backGroundScale9Enabled)
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    scale9Size.width = atof(value.c_str());
                }
                else if (name == "Y")
                {
                    scale9Size.height = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "SingleColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    bgColor.r = atoi(value.c_str());
                }
                else if (name == "G")
                {
                    bgColor.g = atoi(value.c_str());
                }
                else if (name == "B")
                {
                    bgColor.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "EndColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    bgEndColor.r = atoi(value.c_str());
                }
                else if (name == "G")
                {
                    bgEndColor.g = atoi(value.c_str());
                }
                else if (name == "B")
                {
                    bgEndColor.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FirstColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    bgStartColor.r = atoi(value.c_str());
                }
                else if (name == "G")
                {
                    bgStartColor.g = atoi(value.c_str());
                }
                else if (name == "B")
                {
                    bgStartColor.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "ColorVector")
        {
            attribute = child->FirstAttribute();
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ScaleX")
                {
                    colorVector.x = atof(value.c_str());
                }
                else if (name == "ScaleY")
                {
                    colorVector.y = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    Color f_bgColor(255, bgColor.r, bgColor.g, bgColor.b);
    Color f_bgStartColor(255, bgStartColor.r, bgStartColor.g, bgStartColor.b);
    Color f_bgEndColor(255, bgEndColor.r, bgEndColor.g, bgEndColor.b);
    ColorVector f_colorVector(colorVector.x, colorVector.y);
    CapInsets f_capInsets(capInsets.origin.x, capInsets.origin.y, capInsets.size.width, capInsets.size.height);
    FlatSize f_scale9Size(scale9Size.width, scale9Size.height);
    FlatSize f_innerSize(innerSize.width, innerSize.height);
    
    return CreateScrollViewOptions(*_builder,
                                   CreateResourceData(*_builder,
                                                      _builder->CreateString(path),
                                                      _builder->CreateString(plistFile),
                                                      resourceType),
                                   clipEnabled,
                                   &f_bgColor,
                                   &f_bgStartColor,
                                   &f_bgEndColor,
                                   colorType,
                                   bgColorOpacity,
                                   &f_colorVector,
                                   &f_capInsets,
                                   &f_scale9Size,
                                   backGroundScale9Enabled,
                                   &f_innerSize,
                                   direction,
                                   bounceEnabled);
}

Offset<PageViewOptions> FlatBuffersSerialize::createPageViewOptions(const tinyxml2::XMLElement *objectData)
{
    std::string path = "";
    std::string plistFile = "";
    int resourceType = 0;
    
    bool clipEnabled = false;
    Color3B bgColor;
    Color3B bgStartColor;
    Color3B bgEndColor;
    int colorType = 0;
    GLubyte bgColorOpacity = 255;
    Vec2 colorVector(0.0f, -0.5f);
    Rect capInsets;
    Size scale9Size;
    bool backGroundScale9Enabled = false;
    
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "ClipAble")
        {
            clipEnabled = (value == "True") ? true : false;
        }
        else if (name == "ComboBoxIndex")
        {
            colorType = atoi(value.c_str());
        }
        else if (name == "BackColorAlpha")
        {
            bgColorOpacity = atoi(value.c_str());
        }
        else if (name == "Scale9Enable")
        {
            if (value == "True")
            {
                backGroundScale9Enabled = true;
            }
        }
        else if (name == "Scale9OriginX")
        {
            capInsets.origin.x = atof(value.c_str());
        }
        else if (name == "Scale9OriginY")
        {
            capInsets.origin.y = atof(value.c_str());
        }
        else if (name == "Scale9Width")
        {
            capInsets.size.width = atof(value.c_str());
        }
        else if (name == "Scale9Height")
        {
            capInsets.size.height = atof(value.c_str());
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "Size" && backGroundScale9Enabled)
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    scale9Size.width = atof(value.c_str());
                }
                else if (name == "Y")
                {
                    scale9Size.height = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "SingleColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    bgColor.r = atoi(value.c_str());
                }
                else if (name == "G")
                {
                    bgColor.g = atoi(value.c_str());
                }
                else if (name == "B")
                {
                    bgColor.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "EndColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    bgEndColor.r = atoi(value.c_str());
                }
                else if (name == "G")
                {
                    bgEndColor.g = atoi(value.c_str());
                }
                else if (name == "B")
                {
                    bgEndColor.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FirstColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    bgStartColor.r = atoi(value.c_str());
                }
                else if (name == "G")
                {
                    bgStartColor.g = atoi(value.c_str());
                }
                else if (name == "B")
                {
                    bgStartColor.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "ColorVector")
        {
            attribute = child->FirstAttribute();
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ScaleX")
                {
                    colorVector.x = atof(value.c_str());
                }
                else if (name == "ScaleY")
                {
                    colorVector.y = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    Color f_bgColor(255, bgColor.r, bgColor.g, bgColor.b);
    Color f_bgStartColor(255, bgStartColor.r, bgStartColor.g, bgStartColor.b);
    Color f_bgEndColor(255, bgEndColor.r, bgEndColor.g, bgEndColor.b);
    ColorVector f_colorVector(colorVector.x, colorVector.y);
    CapInsets f_capInsets(capInsets.origin.x, capInsets.origin.y, capInsets.size.width, capInsets.size.height);
    FlatSize f_scale9Size(scale9Size.width, scale9Size.height);
    
    return CreatePageViewOptions(*_builder,
                                 CreateResourceData(*_builder,
                                                    _builder->CreateString(path),
                                                    _builder->CreateString(plistFile),
                                                    resourceType),
                                 clipEnabled,
                                 &f_bgColor,
                                 &f_bgStartColor,
                                 &f_bgEndColor,
                                 colorType,
                                 bgColorOpacity,
                                 &f_colorVector,
                                 &f_capInsets,
                                 &f_scale9Size,
                                 backGroundScale9Enabled);
}

Offset<ListViewOptions> FlatBuffersSerialize::createListViewOptions(const tinyxml2::XMLElement *objectData)
{
    std::string path = "";
    std::string plistFile = "";
    int resourceType = 0;
    
    bool clipEnabled = false;
    Color3B bgColor;
    Color3B bgStartColor;
    Color3B bgEndColor;
    int colorType = 0;
    GLubyte bgColorOpacity = 255;
    Vec2 colorVector(0.0f, -0.5f);
    Rect capInsets;
    Size scale9Size;
    bool backGroundScale9Enabled = false;
    Size innerSize(200, 300);
    int direction = 0;
    bool bounceEnabled = false;
    int gravity = 0;
    int itemMargin = 0;
    
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "ClipAble")
        {
            clipEnabled = (value == "True") ? true : false;
        }
        else if (name == "ComboBoxIndex")
        {
            colorType = atoi(value.c_str());
        }
        else if (name == "BackColorAlpha")
        {
            bgColorOpacity = atoi(value.c_str());
        }
        else if (name == "Scale9Enable")
        {
            if (value == "True")
            {
                backGroundScale9Enabled = true;
            }
        }
        else if (name == "Scale9OriginX")
        {
            capInsets.origin.x = atof(value.c_str());
        }
        else if (name == "Scale9OriginY")
        {
            capInsets.origin.y = atof(value.c_str());
        }
        else if (name == "Scale9Width")
        {
            capInsets.size.width = atof(value.c_str());
        }
        else if (name == "Scale9Height")
        {
            capInsets.size.height = atof(value.c_str());
        }
        else if (name == "DirectionType")
        {
            if (value == "Vertical")
            {
                direction = 1;
                
                attribute = objectData->FirstAttribute();
                while (attribute)
                {
                    name = attribute->Name();
                    value = attribute->Value();
                    
                    if (name == "HorizontalType")
                    {
                        if (value == "HORIZONTAL_LEFT")
                        {
                            gravity = 0;
                        }
                        else if (value == "HORIZONTAL_RIGHT")
                        {
                            gravity = 1;
                        }
                        else if (value == "HORIZONTAL_CENTER")
                        {
                            gravity = 2;
                        }
                    }
                    
                    attribute = attribute->Next();
                }
            }
            else if (value == "Horizontal")
            {
                direction = 2;
                
                attribute = objectData->FirstAttribute();
                while (attribute)
                {
                    name = attribute->Name();
                    value = attribute->Value();
                    
                    if (name == "VerticalType")
                    {
                        if (value == "VERTICAL_TOP")
                        {
                            gravity = 3;
                        }
                        else if (value == "VERTICAL_BOTTOM")
                        {
                            gravity = 4;
                        }
                        else if (value == "VERTICAL_CENTER")
                        {
                            gravity = 5;
                        }
                    }
                    
                    attribute = attribute->Next();
                }
            }
        }
        else if (name == "IsBounceEnabled")
        {
            bounceEnabled = (value == "True") ? true : false;
        }
        else if (name == "ItemMargin")
        {
            itemMargin = atoi(value.c_str());
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "InnerNodeSize")
        {
            attribute = child->FirstAttribute();
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Width")
                {
                    innerSize.width = atof(value.c_str());
                }
                else if (name == "Height")
                {
                    innerSize.height = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "Size" && backGroundScale9Enabled)
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "X")
                {
                    scale9Size.width = atof(value.c_str());
                }
                else if (name == "Y")
                {
                    scale9Size.height = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "SingleColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    bgColor.r = atoi(value.c_str());
                }
                else if (name == "G")
                {
                    bgColor.g = atoi(value.c_str());
                }
                else if (name == "B")
                {
                    bgColor.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "EndColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    bgEndColor.r = atoi(value.c_str());
                }
                else if (name == "G")
                {
                    bgEndColor.g = atoi(value.c_str());
                }
                else if (name == "B")
                {
                    bgEndColor.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FirstColor")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "R")
                {
                    bgStartColor.r = atoi(value.c_str());
                }
                else if (name == "G")
                {
                    bgStartColor.g = atoi(value.c_str());
                }
                else if (name == "B")
                {
                    bgStartColor.b = atoi(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "ColorVector")
        {
            attribute = child->FirstAttribute();
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "ScaleX")
                {
                    colorVector.x = atof(value.c_str());
                }
                else if (name == "ScaleY")
                {
                    colorVector.y = atof(value.c_str());
                }
                
                attribute = attribute->Next();
            }
        }
        else if (name == "FileData")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    Color f_bgColor(255, bgColor.r, bgColor.g, bgColor.b);
    Color f_bgStartColor(255, bgStartColor.r, bgStartColor.g, bgStartColor.b);
    Color f_bgEndColor(255, bgEndColor.r, bgEndColor.g, bgEndColor.b);
    ColorVector f_colorVector(colorVector.x, colorVector.y);
    CapInsets f_capInsets(capInsets.origin.x, capInsets.origin.y, capInsets.size.width, capInsets.size.height);
    FlatSize f_scale9Size(scale9Size.width, scale9Size.height);
    FlatSize f_innerSize(innerSize.width, innerSize.height);
    
    return CreateListViewOptions(*_builder,
                                 CreateResourceData(*_builder,
                                                    _builder->CreateString(path),
                                                    _builder->CreateString(plistFile),
                                                    resourceType),
                                 clipEnabled,
                                 &f_bgColor,
                                 &f_bgStartColor,
                                 &f_bgEndColor,
                                 colorType,
                                 bgColorOpacity,
                                 &f_colorVector,
                                 &f_capInsets,
                                 &f_scale9Size,
                                 backGroundScale9Enabled,
                                 &f_innerSize,
                                 direction,
                                 bounceEnabled,
                                 gravity,
                                 itemMargin);
}

Offset<ProjectNodeOptions> FlatBuffersSerialize::createProjectNodeOptions(const tinyxml2::XMLElement *objectData)
{
    std::string filename = "";
    
    // FileData
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
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
                    filename = convert;
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateProjectNodeOptions(*_builder,
                                    _builder->CreateString(filename));
}

Offset<ComAudioOptions> FlatBuffersSerialize::createComAudioOptions(const tinyxml2::XMLElement *objectData)
{
    std::string name = "";
    bool enabled = false;
    bool loop = false;
    float volume = 0;
    
    std::string path = "";
    std::string plist = "";
    int resourceType = 0;
    
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string attriname = attribute->Name();
        std::string value = attribute->Value();
        
        if (attriname == "Loop")
        {
            loop = (value == "True") ? true : false;
        }
        else if (attriname == "Volume")
        {
            volume = atof(value.c_str());
        }
        else if (attriname == "Name")
        {
            name = value;
        }
        
        attribute = attribute->Next();
    }
    
    // FileData
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string attriname = child->Name();
        
        if (attriname == "FileData")
        {
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                attriname = attribute->Name();
                std::string value = attribute->Value();
                
                if (attriname == "Path")
                {
                    path = value;
                }
                else if (attriname == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (attriname == "Plist")
                {
                    plist = value;
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateComAudioOptions(*_builder,
                                 _builder->CreateString(name),
                                 enabled,
                                 loop,
                                 volume,
                                 CreateResourceData(*_builder,
                                                    _builder->CreateString(path),
                                                    _builder->CreateString(plist),
                                                    resourceType));
}

Offset<TextAtlasOptions> FlatBuffersSerialize::createTextAltasOptions(const tinyxml2::XMLElement *objectData)
{
    std::string path = "";
    std::string plistFile = "";
    int resourceType = 0;
    
    std::string stringValue = "0123456789";
    int itemWidth = 0;
    int itemHeight = 0;
    std::string startCharMap = "";
    
    // attributes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "LabelText")
        {
            stringValue = value;
        }
        else if (name == "CharWidth")
        {
            itemWidth = atoi(value.c_str());
        }
        else if (name == "CharHeight")
        {
            itemHeight = atoi(value.c_str());
        }
        else if (name == "StartChar")
        {
            startCharMap = value;
        }
        
        attribute = attribute->Next();
    }
    
    // child elements
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        
        if (name == "LabelAtlasFileImage_CNB")
        {
            std::string texture = "";
            std::string texturePng = "";
            
            attribute = child->FirstAttribute();
            
            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();
                
                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                    texture = value;
                }
                
                attribute = attribute->Next();
            }
            
            if (resourceType == 1)
            {
                _textures.push_back(_builder->CreateString(texture));
                
                texturePng = texture.substr(0, texture.find_last_of('.')).append(".png");
                _texturePngs.push_back(_builder->CreateString(texturePng));
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateTextAtlasOptions(*_builder,
                                  CreateResourceData(*_builder,
                                                     _builder->CreateString(path),
                                                     _builder->CreateString(plistFile),
                                                     resourceType),
                                  _builder->CreateString(stringValue),
                                  _builder->CreateString(startCharMap),
                                  itemWidth,
                                  itemHeight
                                  );
}

int FlatBuffersSerialize::getResourceType(std::string key)
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


// NodeAction
Offset<NodeAction> FlatBuffersSerialize::createNodeAction(const tinyxml2::XMLElement *objectData)
{
    int duration = 0;
    float speed = 0.0f;
    
    CCLOG("animation name = %s", objectData->Name());
    
    // ActionTimeline
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    
    // attibutes
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "Duration")
        {
            duration = atoi(value.c_str());
        }
        else if (name == "Speed")
        {
            speed = atof(value.c_str());
        }
        
        attribute = attribute->Next();
    }
    
    // all Timeline
    std::vector<Offset<TimeLine>> timelines;
    const tinyxml2::XMLElement* timelineElement = objectData->FirstChildElement();
    while (timelineElement)
    {
        auto timeLine = createTimeLine(timelineElement);
        timelines.push_back(timeLine);
        
        timelineElement = timelineElement->NextSiblingElement();
    }
    
    return CreateNodeAction(*_builder,
                            duration,
                            speed,
                            _builder->CreateVector(timelines));
}

Offset<TimeLine> FlatBuffersSerialize::createTimeLine(const tinyxml2::XMLElement *objectData)
{
    int actionTag = 0;
    std::string frameType = "";
    
    // TimelineData attrsibutes
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "ActionTag")
        {
            actionTag = atoi(value.c_str());
        }
        else if (name == "FrameType")
        {
            frameType = value;
        }
        
        attribute = attribute->Next();
    }
    
    // all Frame
    std::vector<Offset<flatbuffers::Frame>> frames;
    
    const tinyxml2::XMLElement* frameElement = objectData->FirstChildElement();
    while (frameElement)
    {
        Offset<flatbuffers::Frame> frame;
        
        if (frameType == FrameType_VisibleFrame)
        {
            auto visibleFrame = createTimeLineBoolFrame(frameElement);
            frame = CreateFrame(*_builder,
                                visibleFrame);
            
            frames.push_back(frame);
        }
        else if (frameType == FrameType_ZOrderFrame)
        {
            auto zOrderFrame = createTimeLineIntFrame(objectData);
            frame = CreateFrame(*_builder,
                                0, // VisibleFrame
                                zOrderFrame);
            
            frames.push_back(frame);
        }
        else if (frameType == FrameType_RotationSkewFrame)
        {
            auto rotationSkewFrame = createTimeLinePointFrame(frameElement);
            frame = CreateFrame(*_builder,
                                0, // VisibleFrame
                                0, // ZOrderFrame
                                rotationSkewFrame);
            
            frames.push_back(frame);
        }
        else if (frameType == FrameType_EventFrame)
        {
            auto eventFrame = createTimeLineStringFrame(frameElement);
            frame = CreateFrame(*_builder,
                                0, // VisibleFrame
                                0, // ZOrderFrame
                                0, // RotationSkewFrame
                                eventFrame);
            
            frames.push_back(frame);
        }
        else if (frameType == FrameType_AnchorFrame)
        {
            auto anchorPointFrame = createTimeLinePointFrame(frameElement);
            frame = CreateFrame(*_builder,
                                0, // VisibleFrame
                                0, // ZOrderFrame
                                0, // RotationSkewFrame
                                0, // EventFrame
                                anchorPointFrame);
            
            frames.push_back(frame);
        }
        else if (frameType == FrameType_PositionFrame)
        {
            auto positionFrame = createTimeLinePointFrame(frameElement);
            frame = CreateFrame(*_builder,
                                0, // VisibleFrame
                                0, // ZOrderFrame
                                0, // RotationSkewFrame
                                0, // EventFrame
                                0, // AnchorPointFrame
                                positionFrame);
            
            frames.push_back(frame);
        }
        else if (frameType == FrameType_ScaleFrame)
        {
            auto scaleFrame = createTimeLinePointFrame(objectData);
            frame = CreateFrame(*_builder,
                                0, // VisibleFrame
                                0, // ZOrderFrame
                                0, // RotationSkewFrame
                                0, // EventFrame
                                0, // AnchorPointFrame
                                0, // PositionFrame
                                scaleFrame);
        }
        else if (frameType == FrameType_ColorFrame)
        {
            auto colorFrame = createTimeLineColorFrame(objectData);
            frame = CreateFrame(*_builder,
                                0, // VisibleFrame
                                0, // ZOrderFrame
                                0, // RotationSkewFrame
                                0, // EventFrame
                                0, // AnchorPointFrame
                                0, // PositionFrame
                                0, // ScaleFrame
                                colorFrame);
        }
        else if (frameType == FrameType_TextureFrame)
        {
            auto textureFrame = createTimeLineTextureFrame(objectData);
            frame = CreateFrame(*_builder,
                                0, // VisibleFrame
                                0, // ZOrderFrame
                                0, // RotationSkewFrame
                                0, // EventFrame
                                0, // AnchorPointFrame
                                0, // PositionFrame
                                0, // ScaleFrame
                                0, // ColorFrame
                                textureFrame);
        }
                
        frameElement = frameElement->NextSiblingElement();
    }
    
    return CreateTimeLine(*_builder,
                          _builder->CreateString(frameType),
                          actionTag,
                          _builder->CreateVector(frames));
}

Offset<TimeLineBoolFrame> FlatBuffersSerialize::createTimeLineBoolFrame(const tinyxml2::XMLElement *objectData)
{
    int frameIndex = 0;
    bool tween = true;
    bool value = false;
    
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string attrivalue = attribute->Value();
        
        if (name == "Value")
        {
            attrivalue = (attrivalue == "True") ? true : false;
        }
        else if (name == "FrameIndex")
        {
            frameIndex = atoi(attrivalue.c_str());
        }
        else if (name == "Tween")
        {
            tween = atoi(attrivalue.c_str());
        }
        
        attribute = attribute->Next();
    }
    
    return CreateTimeLineBoolFrame(*_builder,
                                   frameIndex,
                                   tween,
                                   value);
}

Offset<TimeLineIntFrame> FlatBuffersSerialize::createTimeLineIntFrame(const tinyxml2::XMLElement *objectData)
{
    int frameIndex = 0;
    bool tween = true;
    int value = 0;
    
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string attrivalue = attribute->Value();
        
        if (name == "Value") // to be gonna modify
        {
            value = atoi(attrivalue.c_str());
        }
        else if (name == "FrameIndex")
        {
            frameIndex = atoi(attrivalue.c_str());
        }
        else if (name == "Tween")
        {
            tween = (attrivalue == "True") ? true : false;
        }
        
        attribute = attribute->Next();
    }
    
    return CreateTimeLineIntFrame(*_builder,
                                  frameIndex,
                                  tween,
                                  value);
}

Offset<TimeLineStringFrame> FlatBuffersSerialize::createTimeLineStringFrame(const tinyxml2::XMLElement *objectData)
{
    int frameIndex = 0;
    bool tween = true;
    std::string value = "";
    
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string attrivalue = attribute->Value();
        
        if (name == "Value") // to be gonna modify
        {
            value = attrivalue;
        }
        else if (name == "FrameIndex")
        {
            frameIndex = atoi(attrivalue.c_str());
        }
        else if (name == "Tween")
        {
            tween = (attrivalue == "True") ? true : false;
        }
        
        attribute = attribute->Next();
    }
    
    return CreateTimeLineStringFrame(*_builder,
                                     frameIndex,
                                     tween,
                                     _builder->CreateString(value));
}

Offset<TimeLinePointFrame> FlatBuffersSerialize::createTimeLinePointFrame(const tinyxml2::XMLElement *objectData)
{
    int frameIndex = 0;
    bool tween = true;
    Vec2 position;
    
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "X")
        {
            position.x = atof(value.c_str());
        }
        else if (name == "Y")
        {
            position.y = atof(value.c_str());
        }
        else if (name == "FrameIndex")
        {
            frameIndex = atoi(value.c_str());
        }
        else if (name == "Tween")
        {
            tween = (value == "True") ? true : false;
        }
        
        attribute = attribute->Next();
    }
    
    Position f_position(position.x, position.y);
    
    return CreateTimeLinePointFrame(*_builder,
                                    frameIndex,
                                    tween,
                                    &f_position);
}

Offset<TimeLineColorFrame> FlatBuffersSerialize::createTimeLineColorFrame(const tinyxml2::XMLElement *objectData)
{
    int frameIndex = 0;
    bool tween = true;
    Color4B color;
    
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();
        
        if (name == "FrameIndex")
        {
            frameIndex = atoi(value.c_str());
        }
        else if (name == "Alpha")
        {
            color.a = atoi(value.c_str());
        }
        else if (name == "Tween")
        {
            tween = (value == "True") ? true : false;
        }
        
        attribute = attribute->Next();
    }
    
    // color
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        attribute = child->FirstAttribute();
        while (attribute)
        {
            std::string name = attribute->Name();
            std::string value = attribute->Value();
            
            if (name == "R")
            {
                color.r = atoi(value.c_str());
            }
            else if (name == "G")
            {
                color.g = atoi(value.c_str());
            }
            else if (name == "B")
            {
                color.b = atoi(value.c_str());
            }			
            
            attribute = attribute->Next();
        }
        
        child = child->NextSiblingElement();
    }
    
    Color f_color(color.a, color.r, color.g, color.b);
    
    return CreateTimeLineColorFrame(*_builder,
                                    frameIndex,
                                    tween,
                                    &f_color);
}

Offset<TimeLineTextureFrame> FlatBuffersSerialize::createTimeLineTextureFrame(const tinyxml2::XMLElement *objectData)
{
    int frameIndex = 0;
    bool tween = true;
    
    std::string path = "";
    std::string plistFile = "";
    int resourceType = 0;
    
    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string attriname = attribute->Name();
        std::string value = attribute->Value();
        
        if (attriname == "FrameIndex")
        {
            frameIndex = atoi(value.c_str());
        }
        else if (attriname == "Tween")
        {
            tween = (value == "True") ? true : false;
        }
        
        attribute = attribute->Next();
    }
    
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        attribute = child->FirstAttribute();
        while (attribute)
        {
            std::string attriname = attribute->Name();
            std::string value = attribute->Value();
            
            if (attriname == "Path")
            {
                path = value;
            }
            else if (attriname == "Type")
            {
                resourceType = getResourceType(value);
            }
            else if (attriname == "Plist")
            {
                plistFile = value;
            }
            
            attribute = attribute->Next();
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateTimeLineTextureFrame(*_builder,
                                      frameIndex,
                                      tween,
                                      _builder->CreateString(path));
}

/* create flat buffers with XML */
FlatBufferBuilder* FlatBuffersSerialize::createFlatBuffersWithXMLFileForSimulator(const std::string &xmlFileName)
{    
    std::string inFullpath = FileUtils::getInstance()->fullPathForFilename(xmlFileName).c_str();
    
    // xml read
    if (!FileUtils::getInstance()->isFileExist(inFullpath))
    {
        CCLOG(".csd file doesn not exists ");
    }
    
    ssize_t size;
    std::string content =(char*)FileUtils::getInstance()->getFileData(inFullpath, "r", &size);
    
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
            
            //
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
    
    if (serializeEnabled)
    {
        FlatBufferBuilder builder;
        _builder = &builder;
        
        Offset<NodeTree> nodeTree;
        Offset<NodeAction> aciton;
        
        
        const tinyxml2::XMLElement* child = element->FirstChildElement();
        
        while (child)
        {
            std::string name = child->Name();
            
            if (name == "Animation") // action
            {
                const tinyxml2::XMLElement* animation = child;
                aciton = createNodeAction(animation);
            }
            else if (name == "ObjectData") // nodeTree
            {
                const tinyxml2::XMLElement* objectData = child;
                nodeTree = createNodeTree(objectData, rootType);
            }
            
            child = child->NextSiblingElement();
        }
        
        auto csparsebinary = CreateCSParseBinary(builder,
                                                 builder.CreateVector(_textures),
                                                 builder.CreateVector(_texturePngs),
                                                 nodeTree,
                                                 aciton);
        _csparsebinary = &csparsebinary;
        builder.Finish(csparsebinary);
    }
    
    return _builder;
}

Offset<ProjectNodeOptions> FlatBuffersSerialize::createProjectNodeOptionsForSimulator(const tinyxml2::XMLElement *objectData)
{
    std::string filename = "";
    
    // FileData
    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
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
                    filename = value;
                }
                
                attribute = attribute->Next();
            }
        }
        
        child = child->NextSiblingElement();
    }
    
    return CreateProjectNodeOptions(*_builder,
                                    _builder->CreateString(filename));
}
/**/