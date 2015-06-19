/****************************************************************************
Copyright (c) 2015 cocos2d-x.org

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

#include "base/CCDirector.h"
#include "renderer/CCRenderer.h"
#include "renderer/ccGLStateCache.h"
#include "renderer/CCGLProgram.h"

#include "CCBoneNode.h"
#include "CCSkeletonNode.h"

using namespace cocos2d;

NS_TIMELINE_BEGIN


BoneNode::BoneNode()
: _length(50)
, _width(20)
, _isRackShow(true)
, _rootBoneNode(nullptr)
, _blendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED)
{
}


BoneNode* BoneNode::create()
{
    BoneNode* ret = new (std::nothrow) BoneNode();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}


BoneNode* BoneNode::create(const int &length)
{
    BoneNode* ret = new (std::nothrow) BoneNode();
    ret->_length = length;
    if (ret && ret->init())
    {
        ret->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}

BoneNode* BoneNode::create(const int &length, const cocos2d::Color4F & color)
{
    BoneNode* ret = new (std::nothrow) BoneNode();
    if (ret && ret->init())
    {
        ret->setLength(length);
        ret->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}

void BoneNode::addChildBone(BoneNode* bone)
{
    Node::addChild(bone);
    _childBones.emplace(bone->getName(), bone);
    bone->_rootBoneNode = _rootBoneNode;
}

void BoneNode::removeChildBone(BoneNode* bone, bool cleaup /*= false*/)
{
    Node::removeChild(bone, cleaup);
    _childBones.erase(bone->getName());
    bone->_rootBoneNode = nullptr;
}

void BoneNode::clearChildBones(bool cleanup, bool recursive /*= false*/)
{
    if (recursive)
    {
        for (const auto &childbone : _childBones)
            childbone.second->clearChildBones(recursive, cleanup);
    }
    for (const auto &childbone : _childBones)
    {
        Node::removeChild(childbone.second, cleanup);
        childbone.second->_rootBoneNode = nullptr;
    }
    _childBones.clear();
}

void BoneNode::removeFromParentBone(bool cleanup)
{
    auto parentBone = dynamic_cast<BoneNode*>(_parent);
    CCASSERT (nullptr != _parent, "Not a child of a BoneNode");

    parentBone->removeChildBone(this, cleanup);
}

void BoneNode::addSkin(SkinNode* skin, bool hide /*= false*/)
{
    std::string name = skin->getName();
    if (_boneSkins.find(name) == _boneSkins.end())
    {
        Node::addChild(skin);
        _boneSkins.emplace(name, skin);
    }
    else
    {
        CCLOGINFO("Skin named %s has been added to this bone", name);
    }
}

void BoneNode::removeSkin(SkinNode* skin)
{
    Node::removeChild(skin);
    _boneSkins.erase(skin->getName());
}

void BoneNode::clearSkins(bool cleanup)
{
    for (const auto &skin : _boneSkins)
    {
        Node::removeChild(skin.second, cleanup);
    }
    _boneSkins.clear();
}

void BoneNode::display(SkinNode* skin, bool hideOthers /*= false*/)
{
    display(skin->getName(), hideOthers);
}

void BoneNode::display(const std::string& skinName, bool hideOthers /*= false*/)
{
    auto skinToDisp = _boneSkins.find(skinName);
    if (_boneSkins.end() != skinToDisp)
    {
        if (!skinToDisp->second->isVisible())
        {
            skinToDisp->second->setVisible(true);
        }

        if (hideOthers)
        {
            auto skinIter = _boneSkins.begin();
            for (; skinIter != _boneSkins.end(); skinIter ++)
            {
                if (skinIter != skinToDisp)
                    skinIter->second->setVisible(false);
            }
        }
    }
    else
    {
        CCLOGINFO("No skin named %s in this BoneNode", skinName);
    }
}

std::map<std::string, SkinNode*> BoneNode::getDisplaying() const
{
    std::map<std::string, SkinNode*> displayingSkins;
    for (const auto &boneskin : _boneSkins)
    {
        if (boneskin.second->isVisible())
        {
            displayingSkins.emplace(boneskin);
        }
    }
    return displayingSkins;
}

void BoneNode::setBlendFunc(const BlendFunc &blendFunc)
{
    _blendFunc = blendFunc;
}

void BoneNode::setLength(float length)
{
    _length = length;
    _contentSize.width = length;
    _anchorPointInPoints.set(_contentSize.width * _anchorPoint.x, _contentSize.height * _anchorPoint.y);
    updateVertices();
}

void BoneNode::setWidth(float width)
{
    _width = width;
    _contentSize.height = width;
    _anchorPointInPoints.set(_contentSize.width * _anchorPoint.x, _contentSize.height * _anchorPoint.y);
    updateVertices();
}

void BoneNode::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    if (!_isRackShow)
        return;
    
    // render depends _rootBoneNode's global Zorder + this globalZorder
    auto rackGlobalZorder = _globalZOrder;
    if (nullptr != _rootBoneNode)
    {
        if (_rootBoneNode->getAllRackShow())
            rackGlobalZorder += _rootBoneNode->getGlobalZOrder();
        else
            return;
    }

    _customCommand.init(rackGlobalZorder, transform, flags);
    _customCommand.func = CC_CALLBACK_0(BoneNode::onDraw, this, transform, flags);
    renderer->addCommand(&_customCommand);

    for (int i = 0; i < 4; ++i)
    {
        Vec4 pos;
        pos.x = _squareVertices[i].x; pos.y = _squareVertices[i].y; pos.z = _positionZ;
        pos.w = 1;
        _modelViewTransform.transformVector(&pos);
        _noMVPVertices[i] = Vec3(pos.x, pos.y, pos.z) / pos.w;
    }
}

void BoneNode::setContentSize(const cocos2d::Size &size)
{
    if (!size.equals(_contentSize))
    {
        _contentSize = size;
        _anchorPointInPoints.set(_contentSize.width * _anchorPoint.x, _contentSize.height * _anchorPoint.y);
    }
    _length = _contentSize.width;
    _width  = _contentSize.height;
    updateVertices();
}

bool BoneNode::init()
{
    _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
    _anchorPoint = Vec2(0, .5f);
    setContentSize(cocos2d::Size(_length, _width));
    setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(cocos2d::GLProgram::SHADER_NAME_POSITION_COLOR_NO_MVP));
    return true;
}

void BoneNode::updateVertices()
{
    if (_length != _squareVertices[2].x || _squareVertices[3].y != _width / 2)
    {
        _squareVertices[0].x = _squareVertices[3].x = _length * .1f;
        _squareVertices[1].y = _squareVertices[2].y = _width * .5f;
        _squareVertices[3].y = _width;
        _squareVertices[2].x = _length;
        _transformUpdated = _transformDirty = _inverseDirty = _contentSizeDirty = true;
    }
}

void BoneNode::updateColor()
{
    for (unsigned int i = 0; i < 4; i++)
    {
        _squareColors[i].r = _displayedColor.r / 255.0f;
        _squareColors[i].g = _displayedColor.g / 255.0f;
        _squareColors[i].b = _displayedColor.b / 255.0f;
        _squareColors[i].a = _displayedOpacity / 255.0f;
    }
    _transformUpdated = _transformDirty = _inverseDirty = _contentSizeDirty = true;
}

void BoneNode::onDraw(const Mat4& transform, uint32_t flags)
{
    getGLProgram()->use();
    getGLProgram()->setUniformsForBuiltins(transform);

    GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_COLOR);

    //
    // Attributes
    //
#ifdef EMSCRIPTEN
    setGLBufferData(_noMVPVertices, 4 * sizeof(Vec3), 0);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    setGLBufferData(_squareColors, 4 * sizeof(Color4F), 1);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, _noMVPVertices);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, _squareColors);
#endif // EMSCRIPTEN

    cocos2d::GL::blendFunc(_blendFunc.src, _blendFunc.dst);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, 4);
}

BoneNode::~BoneNode()
{
}

void BoneNode::setRackShow(bool ishow)
{
    _isRackShow = ishow;
}

NS_TIMELINE_END