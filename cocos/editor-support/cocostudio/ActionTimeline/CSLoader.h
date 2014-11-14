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

#ifndef __cocos2d_libs__CSLoader__
#define __cocos2d_libs__CSLoader__

#include "cocostudio/DictionaryHelper.h"
#include "cocostudio/CocosStudioExport.h"
#include "cocos2d.h"

/* peterson */
namespace flatbuffers
{
    struct FlatBufferBuilder;
    
    struct NodeTree;
    
    struct WidgetOptions;
    struct SingleNodeOptions;
    struct SpriteOptions;
    struct ParticleSystemOptions;
    struct TMXTiledMapOptions;
    struct ProjectNodeOptions;
    
    struct ComponentOptions;
    struct ComAudioOptions;
}
/**/

namespace protocolbuffers
{
    class CSParseBinary;
    class NodeTree;
	class WidgetOptions;
    class SpriteOptions;
    class ProjectNodeOptions;
    class ParticleSystemOptions;
    class TMXTiledMapOptions;
    class ComponentOptions;
    class ComAudioOptions;
}

namespace tinyxml2
{
    class XMLElement;
}

namespace cocostudio {
    class ComAudio;
}

NS_CC_BEGIN

class CC_STUDIO_DLL CSLoader
{
public:
    static CSLoader* getInstance();
    static void destroyInstance();
    
    CSLoader();
    void purge();
    
    void init();
    
    static cocos2d::Node* createNode(const std::string& filename);
    
    cocos2d::Node* createNodeFromJson(const std::string& filename);
    cocos2d::Node* loadNodeWithFile(const std::string& fileName);
    cocos2d::Node* loadNodeWithContent(const std::string& content);
    
    void setRecordJsonPath(bool record) { _recordJsonPath = record; }
    bool isRecordJsonPath() const { return _recordJsonPath; }
    
    void setJsonPath(std::string jsonPath) { _jsonPath = jsonPath; }
    std::string getJsonPath() const { return _jsonPath; }
    
    cocos2d::Node* createNodeFromProtocolBuffers(const std::string& filename);
    cocos2d::Node* nodeFromProtocolBuffersFile(const std::string& fileName);
    cocos2d::Node* nodeFromProtocolBuffers(const protocolbuffers::NodeTree& nodetree);
    
    /* peterson */
    cocos2d::Node* createNodeWithFlatBuffersFile(const std::string& filename);
    cocos2d::Node* nodeWithFlatBuffersFile(const std::string& fileName);
    cocos2d::Node* nodeWithFlatBuffers(const flatbuffers::NodeTree* nodetree);
    /**/
    /* peterson create node with flat buffers from .csd for simulator of cocosstudio editor */
	cocos2d::Node* createNodeWithFlatBuffersForSimulator(const std::string& filename);
    cocos2d::Node* nodeWithFlatBuffersForSimulator(const flatbuffers::NodeTree* nodetree);
    /**/
    
    void setRecordProtocolBuffersPath(bool record) { _recordProtocolBuffersPath = record; }
    bool isRecordProtocolBuffersPath() const { return _recordProtocolBuffersPath; }
    
    void setProtocolBuffersPath(std::string protocolBuffersPath) { _protocolBuffersPath = protocolBuffersPath; }
    std::string getProtocolBuffersPath() const { return _protocolBuffersPath; }
    
    /* create node from protocol buffers for simulator of cocosstudio editor */
    cocos2d::Node* createNodeFromProtocolBuffersForSimulator(protocolbuffers::CSParseBinary* protobuf);
    cocos2d::Node* nodeFromProtocolBuffersForSimulator(const protocolbuffers::NodeTree& nodetree);
    /**/
    
protected:
    
    cocos2d::Node* loadNode(const rapidjson::Value& json);
    
    void locateNodeWithMulresPosition(cocos2d::Node* node, const rapidjson::Value& json);
    
    void initNode(cocos2d::Node* node, const rapidjson::Value& json);
    
    // load nodes
    cocos2d::Node* loadSimpleNode(const rapidjson::Value& json);
    cocos2d::Node* loadSubGraph  (const rapidjson::Value& json);
    cocos2d::Node* loadSprite    (const rapidjson::Value& json);
    cocos2d::Node* loadParticle  (const rapidjson::Value& json);
    cocos2d::Node* loadTMXTiledMap(const rapidjson::Value& json);
    
    // load gui
    cocos2d::Node* loadWidget(const rapidjson::Value& json);
    
    // load component
    cocos2d::Component* loadComponent(const rapidjson::Value& json);
    cocos2d::Component* loadComAudio(const rapidjson::Value& json);
    
    void setPropsForNodeFromProtocolBuffers(cocos2d::Node* node, const protocolbuffers::WidgetOptions& nodeOptions);
    void setPropsForSingleNodeFromProtocolBuffers(cocos2d::Node* node, const protocolbuffers::WidgetOptions& nodeOptions);
    void setPropsForSpriteFromProtocolBuffers(cocos2d::Node* node,
                                              const protocolbuffers::SpriteOptions& spriteOptions,
                                              const protocolbuffers::WidgetOptions& nodeOptions);
	cocos2d::Node* createParticleFromProtocolBuffers(const protocolbuffers::ParticleSystemOptions& particleSystemOptions,
													 const protocolbuffers::WidgetOptions& nodeOptions);    
	cocos2d::Node* createTMXTiledMapFromProtocolBuffers(const protocolbuffers::TMXTiledMapOptions& tmxTiledMapOptions,
														const protocolbuffers::WidgetOptions& nodeOptions);    
    void setPropsForProjectNodeFromProtocolBuffers(cocos2d::Node* node,
                                                   const protocolbuffers::ProjectNodeOptions& projectNodeOptions,
                                                   const protocolbuffers::WidgetOptions& nodeOptions);
    void setPropsForSimpleAudioFromProtocolBuffers(cocos2d::Node* node, const protocolbuffers::WidgetOptions& nodeOptions);
    
    cocos2d::Component* createComponentFromProtocolBuffers(const protocolbuffers::ComponentOptions& componentOptions);
    void setPropsForComponentFromProtocolBuffers(cocos2d::Component* component, const protocolbuffers::ComponentOptions& componentOptions);
    
    void setPropsForComAudioFromProtocolBuffers(cocos2d::Component* component,
                                                const protocolbuffers::ComAudioOptions& comAudioOptions);
    
    /* peterson */
    void setPropsForNodeWithFlatBuffers(const flatbuffers::WidgetOptions* nodeOptions,
                                        cocos2d::Node* node);
    void setPropsForSingleNodeWithFlatBuffers(const flatbuffers::SingleNodeOptions* singleNodeOptions,
                                              const flatbuffers::WidgetOptions* nodeOptions,
                                              cocos2d::Node* node);
    void setPropsForSpriteWithFlatBuffers(const flatbuffers::SpriteOptions* spriteOptions,
                                          const flatbuffers::WidgetOptions* nodeOptions,
                                          cocos2d::Node* node);
    cocos2d::Node* createParticleWithFlatBuffers(const flatbuffers::ParticleSystemOptions* particleSystemOptions,
                                                 const flatbuffers::WidgetOptions* nodeOptions);
    cocos2d::Node* createTMXTiledMapWithFlatBuffers(const flatbuffers::TMXTiledMapOptions* tmxTiledMapOptions,
                                                    const flatbuffers::WidgetOptions* nodeOptions);
    void setPropsForProjectNodeWithFlatBuffers(const flatbuffers::ProjectNodeOptions* projectNodeOptions,
                                               const flatbuffers::WidgetOptions* nodeOptions,
                                               cocos2d::Node* node);
    void setPropsForSimpleAudioWithFlatBuffers(cocos2d::Node* node,
                                               const flatbuffers::WidgetOptions* nodeOptions);
    
    cocos2d::Component* createComponentWithFlatBuffers(const flatbuffers::ComponentOptions* componentOptions);
    void setPropsForComponentWithFlatBuffers(cocos2d::Component* component,
                                             const flatbuffers::ComponentOptions* componentOptions);
    void setPropsForComAudioWithFlatBuffers(cocos2d::Component* component,
                                            const flatbuffers::ComAudioOptions* comAudioOptions);
    /**/
    
    bool isWidget(const std::string& type);
    bool isCustomWidget(const std::string& type);
    
    std::string getGUIClassName(const std::string &name);
    std::string getWidgetReaderClassName(cocos2d::ui::Widget *widget);
    
    typedef std::function<cocos2d::Node*(const rapidjson::Value& json)> NodeCreateFunc;
    typedef std::pair<std::string, NodeCreateFunc> Pair;
    
    std::unordered_map<std::string, NodeCreateFunc> _funcs;
    
    typedef std::function<cocos2d::Component*(const rapidjson::Value& json)> ComponentCreateFunc;
    typedef std::pair<std::string, ComponentCreateFunc> ComponentPair;
    
    std::unordered_map<std::string, ComponentCreateFunc> _componentFuncs;
    
    bool _recordJsonPath;
    
    std::string _jsonPath;
    
    bool _recordProtocolBuffersPath;
    std::string _protocolBuffersPath;
    
    std::string _monoCocos2dxVersion;
};

NS_CC_END

#endif /* defined(__cocos2d_libs__CSLoader__) */
