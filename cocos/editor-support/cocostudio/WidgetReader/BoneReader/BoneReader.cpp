#include "tinyxml2/tinyxml2.h"
#include "flatbuffers/flatbuffers.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"
#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/CSBoneBinary_generated.h"
#include "cocostudio/WidgetReader/BoneReader/BoneReader.h"
#include "cocostudio/ActionTimeline/CCBoneNode.h"


USING_NS_CC;
USING_NS_TIMELINE;
using namespace cocostudio;
using namespace flatbuffers;

IMPLEMENT_CLASS_NODE_READER_INFO(BoneReader)

BoneReader::BoneReader()
{

}

BoneReader::~BoneReader()
{

}

static BoneReader* _instanceBoneNodeReader = nullptr;

BoneReader* BoneReader::getInstance()
{
    if (_instanceBoneNodeReader == nullptr)
    {
        _instanceBoneNodeReader = new (std::nothrow) BoneReader();
    }
    return _instanceBoneNodeReader;
}

void BoneReader::destroyInstance()
{
    CC_SAFE_DELETE(_instanceBoneNodeReader);
}

Offset<Table> BoneReader::createOptionsWithFlatBuffers(const tinyxml2::XMLElement *objectData,
    flatbuffers::FlatBufferBuilder *builder)
{

    auto temp = NodeReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

    float length = 0;
    cocos2d::BlendFunc blendFunc = cocos2d::BlendFunc::ALPHA_PREMULTIPLIED;

    const tinyxml2::XMLAttribute* attribute = objectData->FirstAttribute();
    while (attribute)
    {
        std::string name = attribute->Name();
        std::string value = attribute->Value();

        if (name == "Length")
        {
            length = atof(value.c_str());
        }
        attribute = attribute->Next();
    }

    const tinyxml2::XMLElement* child = objectData->FirstChildElement();
    while (child)
    {
        std::string name = child->Name();
        if (name == "BlendFunc")
        {
            const tinyxml2::XMLAttribute* attribute = child->FirstAttribute();

            while (attribute)
            {
                name = attribute->Name();
                std::string value = attribute->Value();

                if (name == "Src")
                {
                    blendFunc.src = atoi(value.c_str());
                }
                else if (name == "Dst")
                {
                    blendFunc.dst = atoi(value.c_str());
                }

                attribute = attribute->Next();
            }
        }

        child = child->NextSiblingElement();
    }
    flatbuffers::BlendFunc f_blendFunc(blendFunc.src, blendFunc.dst);

    auto options = CreateBoneOptions(*builder,
        nodeOptions,
        length,
        &f_blendFunc);

    return *(Offset<Table>*)(&options);
}

void BoneReader::setPropsWithFlatBuffers(cocos2d::Node *node,
    const flatbuffers::Table *nodeOptions)
{

    auto* bone = static_cast<BoneNode*>(node);
    auto options = (flatbuffers::BoneOptions*)nodeOptions;

    float length = options->length();
    bone->setLength(length);

    auto f_blendFunc = options->blendFunc();
    if (f_blendFunc)
    {
        cocos2d::BlendFunc blendFunc = cocos2d::BlendFunc::ALPHA_PREMULTIPLIED;
        blendFunc.src = f_blendFunc->src();
        blendFunc.dst = f_blendFunc->dst();
        bone->setBlendFunc(blendFunc);
    }
}

cocos2d::Node*  BoneReader::createNodeWithFlatBuffers(const flatbuffers::Table *nodeOptions)
{
    auto bone = BoneNode::create();

    auto options = (flatbuffers::BoneOptions*)nodeOptions;
    setPropsWithFlatBuffers(bone, (Table*)options);

    return bone;
}
