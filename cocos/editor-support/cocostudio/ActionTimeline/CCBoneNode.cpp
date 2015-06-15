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
#include "renderer/CCGLProgram.h"
#include "2d/CCDrawingPrimitives.h"

#include "CCBoneNode.h"
#include "CCSkeletonNode.h"

using namespace cocos2d;

NS_TIMELINE_BEGIN

BoneNode::BoneNode()
    : _length(50)
    , _width(20)
    , _showRack(true)
    , _skinCascadeBlendFunc(true)
    , _skeletonDraw(nullptr)
    , _rackColor(.1f, .1f, .1f, .8f)
{
    init();
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
        ret->setBoneRackColor(color);
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
    Node::addChild(bone); // assert has no parent
    bone->resetSkeletonDrawNode(this->_skeletonDraw);
    signSkeletonDrawDirty();
   
    _childBones.push_back(bone);
}

void BoneNode::removeChildBone(BoneNode* bone, bool cleaup /*= false*/)
{
    Node::removeChild(bone, cleaup);
    resetSkeletonDrawNode(nullptr);
    signSkeletonDrawDirty();

    auto iterBone = std::find(_childBones.begin(), _childBones.end(), bone);
    if (iterBone != _childBones.end())
        _childBones.erase(iterBone);
}

void BoneNode::clearChildBones(bool cleanup, bool recursive /*= false*/)
{
    if (recursive)
    {
        for (const auto &childbone : _childBones)
            childbone->clearChildBones(recursive, cleanup);
    }
    for (const auto &childbone : _childBones)
    {
        Node::removeChild(childbone, cleanup);
        childbone->resetSkeletonDrawNode(nullptr);
    }
    signSkeletonDrawDirty();
    _childBones.clear();
}

//std::vector<BoneNode*> BoneNode::getChildBones(bool recursive /*= false*/)
//{
//    return _childBones;
//}

void BoneNode::removeFromParentBone(bool cleanup)
{
    auto parentBone = dynamic_cast<BoneNode*>(_parent);
    CCASSERT (nullptr != _parent, "Not a child of a BoneNode");

    parentBone->removeChildBone(this, cleanup);
}

void BoneNode::addSkin(SkinNode* skin, bool hide /*= false*/)
{
    Node::addChild(skin);
    if (_skinCascadeBlendFunc)
    {
        auto blendnode = dynamic_cast<cocos2d::BlendProtocol*>(skin);
        if (nullptr != blendnode)
            blendnode->setBlendFunc(_blendFunc);
    }
    _boneSkins.push_back(skin);
}

void BoneNode::removeSkin(SkinNode* skin)
{
    Node::removeChild(skin);
    auto iterskin = std::find(_boneSkins.begin(), _boneSkins.end(), skin);
    if (iterskin != _boneSkins.end())
        _boneSkins.erase(iterskin);
}

void BoneNode::clearSkins(bool cleanup)
{
    for (const auto &skin : _boneSkins)
    {
        Node::removeChild(skin, cleanup);
    }
    _boneSkins.clear();
}

void BoneNode::display(SkinNode* skin, bool hideOthers /*= false*/)
{
    if (!skin->isVisible())
        skin->setVisible(true);

    if (hideOthers)
    {
        for (auto &boneskin : _boneSkins)
        {
            if (skin != boneskin)
            {
                boneskin->setVisible(false);
            }
        }
    }
}

std::vector<SkinNode*> BoneNode::getDisplaying() const
{
    std::vector<SkinNode*> displayingSkins;
    for (auto &boneskin : _boneSkins)
    {
        if (boneskin->isVisible())
        {
            displayingSkins.push_back(boneskin);
        }
    }
    return displayingSkins;
}

void BoneNode::setBlendFunc(const cocos2d::BlendFunc &blendFunc)
{
    _blendFunc = blendFunc;
    if (_skinCascadeBlendFunc)
    {
        for (auto &skin : _boneSkins)
        {
            auto blendnode = dynamic_cast<cocos2d::BlendProtocol*>(skin);
            if (nullptr != blendnode)
                blendnode->setBlendFunc(_blendFunc);
        }
    }
}

void BoneNode::setLength(float length)
{
    _length = length;
    _contentSize.width = length;
    updateVertices();
}

void BoneNode::setWidth(float width)
{
    _width = width;
    _contentSize.height = width;
    updateVertices();
}

void BoneNode::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    if (!_showRack)
        return;

    if (  nullptr == _skeletonDraw)
    {
        _customCommand.init(_globalZOrder, transform, flags);
        _customCommand.func = CC_CALLBACK_0(BoneNode::onDraw, this, transform, flags);
        renderer->addCommand(&_customCommand);
    }
    else
    {
        if (isSkeletonDrawDirty())
            drawBoneRack();
    }
}

void BoneNode::setBoneRackColor(const cocos2d::Color4F &color)
{
    _rackColor = color;
    updateVecticesColor();
    signSkeletonDrawDirty();
}

BoneNode::~BoneNode()
{
}

void BoneNode::setContentSize(const cocos2d::Size &size)
{
    // super
    if (!size.equals(_contentSize))
    {
        _contentSize = size;
    }
    _length = size.width;
    _width  = size.height;
    updateVertices();
}

bool BoneNode::init()
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
    _anchorPoint = Vec2(0, .5f);
    setContentSize(cocos2d::Size(_length, _width));
    setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(cocos2d::GLProgram::SHADER_NAME_POSITION_COLOR_NO_MVP));
    return true;
}

void BoneNode::updateVertices()
{
    if (_length != _squareVertices[2].y)
    {
        _transformUpdated = _transformDirty = _inverseDirty = _contentSizeDirty = true;
    }

    _squareVertices[1].x = _squareVertices[3].x = _length / 10.f;
    _squareVertices[3].y = _width / 2;
    _squareVertices[1].y = -_squareVertices[3].y;
    _squareVertices[2].x = _length;

    signSkeletonDrawDirty();
}

void BoneNode::signSkeletonDrawDirty(bool dirty)
{
    if (nullptr != _skeletonDraw)
        _skeletonDraw->setVisible(!dirty);
}

bool BoneNode::isSkeletonDrawDirty()
{
    return nullptr != _skeletonDraw && !_skeletonDraw->isVisible();
}

void BoneNode::updateVecticesColor()
{
    if (_squareColors[0] == _rackColor) // judge [0] is ok
    {
        return;
    }

    for (auto &cvecti : _squareColors)
    {
        cvecti.r = _rackColor.r;
        cvecti.g = _rackColor.g;
        cvecti.b = _rackColor.b;
        cvecti.a = _rackColor.a;
    }
    _transformUpdated = _transformDirty = _inverseDirty = _contentSizeDirty = true;
}

void BoneNode::onDraw(const Mat4& transform, uint32_t flags)
{
    Director* director = cocos2d::Director::getInstance();
    CCASSERT(nullptr != director, "Director is null when seting matrix stack");
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

    CHECK_GL_ERROR_DEBUG();
    glLineWidth(1);
    DrawPrimitives::setDrawColor4B(_rackColor.r * 255, _rackColor.g * 255, _rackColor.b * 255, _rackColor.a * 255);
    DrawPrimitives::drawCircle(_squareVertices[0], _width / 4, 0, 30, false);
    DrawPrimitives::drawSolidPoly(_squareVertices, 4, _rackColor);
    CHECK_GL_ERROR_DEBUG();

    //end draw
    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void BoneNode::drawBoneRack()
{
    Vec2 skeletonDrawVs[4];
    for (int i = 0; i < 4; i++)
    {
        skeletonDrawVs[i] = this->convertToWorldSpaceAR(_squareVertices[i]);
        skeletonDrawVs[i] = _skeletonDraw->convertToNodeSpaceAR(skeletonDrawVs[i]); // _anchorPointInPoints is (0, 0)
    }
    const float radius = _width / 4;
    //_skeletonDraw->setBlendFunc(_blendFunc); //how to make it effect
    //_skeletonDraw->drawCircle(skeletonDrawVs[0], radius - 1, 0, 50, false, _rackColor);
    _skeletonDraw->drawCircle(skeletonDrawVs[0], radius, 0, 30, false, _rackColor);
    _skeletonDraw->drawPolygon(skeletonDrawVs, 4, _rackColor, 0, _rackColor);
}



void BoneNode::resetSkeletonDrawNode(cocos2d::DrawNode* skeletonDrawNode)
{
    _skeletonDraw = skeletonDrawNode;
    signSkeletonDrawDirty();
}

// for editor
void BoneNode::setPosition(float x, float y)
{
    LayerColor::setPosition(x, y);
    signSkeletonDrawDirty();
}

void BoneNode::setRotation(float rotation)
{
    LayerColor::setRotation(rotation);
    signSkeletonDrawDirty();
}

void BoneNode::setRotationSkewX(float rotationX)
{
    LayerColor::setRotationSkewX(rotationX);
    signSkeletonDrawDirty();
}

void BoneNode::setRotationSkewY(float rotationY)
{
    LayerColor::setRotationSkewY(rotationY);
    signSkeletonDrawDirty();
}

void BoneNode::setScale(float scaleX, float scaleY)
{
    LayerColor::setScale(scaleX, scaleY);
    signSkeletonDrawDirty();
}

NS_TIMELINE_END