/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 
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

#include "GameNode3DReader.h"

#include "cocostudio/CCObjectExtensionData.h"
#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/CSParse3DBinary_generated.h"

#include "cocostudio/FlatBuffersSerialize.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"

#include "tinyxml2.h"
#include "flatbuffers/flatbuffers.h"

USING_NS_CC;
using namespace flatbuffers;

namespace cocostudio
{
    IMPLEMENT_CLASS_NODE_READER_INFO(GameNode3DReader)
    
    GameNode3DReader::GameNode3DReader()
    {
        
    }
    
    GameNode3DReader::~GameNode3DReader()
    {
        
    }
    
    static GameNode3DReader* _instanceNode3DReader = nullptr;
    
    GameNode3DReader* GameNode3DReader::getInstance()
    {
        if (!_instanceNode3DReader)
        {
            _instanceNode3DReader = new GameNode3DReader();
        }
        
        return _instanceNode3DReader;
    }
    
    void GameNode3DReader::purge()
    {
        CC_SAFE_DELETE(_instanceNode3DReader);
    }
    
    void GameNode3DReader::destroyInstance()
    {
        CC_SAFE_DELETE(_instanceNode3DReader);
    }
    
    Offset<Table> GameNode3DReader::createOptionsWithFlatBuffers(const tinyxml2::XMLElement *objectData,
        flatbuffers::FlatBufferBuilder *builder)
    {
        std::string name = "";
        bool skyBoxEnabled = false;

        std::string leftPath = "";
        std::string leftPlistFile = "";
        int leftResourceType = 0;

        std::string rightPath = "";
        std::string rightPlistFile = "";
        int rightResourceType = 0;

        std::string upPath = "";
        std::string upPlistFile = "";
        int upResourceType = 0;

        std::string downPath = "";
        std::string downPlistFile = "";
        int downResourceType = 0;

        std::string forwardPath = "";
        std::string forwardPlistFile = "";
        int forwardResourceType = 0;

        std::string backPath = "";
        std::string backPlistFile = "";
        int backResourceType = 0;

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
            else if (attriname == "SkyBoxEnabled")
            {
                skyBoxEnabled = (value == "True") ? true : false;
            }
            else if (attriname == "UserData")
            {
                customProperty = value;
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
            if (name == "LeftImage")
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
                        leftPath = value;
                    }
                    else if (name == "Type")
                    {
                        leftResourceType = getResourceType(value);;
                    }
                    else if (name == "Plist")
                    {
                        leftPlistFile = value;
                        texture = value;
                    }

                    attribute = attribute->Next();
                }

                if (leftResourceType == 1)
                {
                    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                    fbs->_textures.push_back(builder->CreateString(texture));
                }
            }
            else if (name == "RightImage")
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
                        rightPath = value;
                    }
                    else if (name == "Type")
                    {
                        rightResourceType = getResourceType(value);;
                    }
                    else if (name == "Plist")
                    {
                        rightPlistFile = value;
                        texture = value;
                    }

                    attribute = attribute->Next();
                }

                if (rightResourceType == 1)
                {
                    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                    fbs->_textures.push_back(builder->CreateString(texture));
                }
            }
            else if (name == "UpImage")
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
                        upPath = value;
                    }
                    else if (name == "Type")
                    {
                        upResourceType = getResourceType(value);;
                    }
                    else if (name == "Plist")
                    {
                        upPlistFile = value;
                        texture = value;
                    }

                    attribute = attribute->Next();
                }

                if (upResourceType == 1)
                {
                    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                    fbs->_textures.push_back(builder->CreateString(texture));
                }
            }
            else if (name == "DownImage")
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
                        downPath = value;
                    }
                    else if (name == "Type")
                    {
                        downResourceType = getResourceType(value);;
                    }
                    else if (name == "Plist")
                    {
                        downPlistFile = value;
                        texture = value;
                    }

                    attribute = attribute->Next();
                }

                if (downResourceType == 1)
                {
                    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                    fbs->_textures.push_back(builder->CreateString(texture));
                }
            }
            else if (name == "ForwardImage")
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
                        forwardPath = value;
                    }
                    else if (name == "Type")
                    {
                        forwardResourceType = getResourceType(value);;
                    }
                    else if (name == "Plist")
                    {
                        forwardPlistFile = value;
                        texture = value;
                    }

                    attribute = attribute->Next();
                }

                if (forwardResourceType == 1)
                {
                    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                    fbs->_textures.push_back(builder->CreateString(texture));
                }
            }
            else if (name == "BackImage")
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
                        backPath = value;
                    }
                    else if (name == "Type")
                    {
                        backResourceType = getResourceType(value);;
                    }
                    else if (name == "Plist")
                    {
                        backPlistFile = value;
                        texture = value;
                    }

                    attribute = attribute->Next();
                }

                if (backResourceType == 1)
                {
                    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                    fbs->_textures.push_back(builder->CreateString(texture));
                }
            }

            child = child->NextSiblingElement();
        }

        auto options = CreateGameNode3DOption(*builder,
            builder->CreateString(name),
            skyBoxEnabled,
            CreateResourceData(*builder,
                                builder->CreateString(leftPath),
                                builder->CreateString(leftPlistFile),
                                leftResourceType),
            CreateResourceData(*builder,
                                builder->CreateString(rightPath),
                                builder->CreateString(rightPlistFile),
                                rightResourceType),
            CreateResourceData(*builder,
                                builder->CreateString(upPath),
                                builder->CreateString(upPlistFile),
                                upResourceType),
            CreateResourceData(*builder,
                                builder->CreateString(downPath),
                                builder->CreateString(downPlistFile),
                                downResourceType),
            CreateResourceData(*builder,
                                builder->CreateString(forwardPath),
                                builder->CreateString(forwardPlistFile),
                                forwardResourceType),
            CreateResourceData(*builder,
                                builder->CreateString(backPath),
                                builder->CreateString(backPlistFile),
                                backResourceType),
            builder->CreateString(frameEvent),
            builder->CreateString(customProperty)
            );

        return *(Offset<Table>*)(&options);
    }
    
    void GameNode3DReader::setPropsWithFlatBuffers(cocos2d::Node *node,
                                                   const flatbuffers::Table* node3DOptions)
    {
        auto options = (GameNode3DOption*)node3DOptions;
        
        std::string name = options->name()->c_str();
        node->setName(name);

        bool skyBoxEnabled = options->skyBoxEnabled() != 0;
        if (skyBoxEnabled)
        {
            auto leftFileData = options->leftFileData()->path()->c_str();
            auto rightFileData = options->rightFileData()->path()->c_str();
            auto upFileData = options->upFileData()->path()->c_str();
            auto downFileData = options->downFileData()->path()->c_str();
            auto forwardFileData = options->forwardFileData()->path()->c_str();
            auto backFileData = options->backFileData()->path()->c_str();

            Skybox* childBox = Skybox::create(leftFileData,rightFileData,upFileData,downFileData,forwardFileData,backFileData);
            node->addChild(childBox);
        }

        std::string customProperty = options->customProperty()->c_str();
        ObjectExtensionData* extensionData = ObjectExtensionData::create();
        extensionData->setCustomProperty(customProperty);
        node->setUserObject(extensionData);
    }
    
    Node* GameNode3DReader::createNodeWithFlatBuffers(const flatbuffers::Table *node3DOptions)
    {
        Node* node = Node::create();
        
        setPropsWithFlatBuffers(node, (Table*)node3DOptions);
        
        return node;
    }

    int GameNode3DReader::getResourceType(std::string key)
    {
        if (key == "Normal" || key == "Default")
        {
            return 	0;
        }

        FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
        if (fbs->_isSimulator)
        {
            if (key == "MarkedSubImage")
            {
                return 0;
            }
        }
        return 1;
    }
}
