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
#include "CCSkeletonNode.h"
#include "renderer/CCGLProgram.h"
NS_TIMELINE_BEGIN


SkeletonNode* SkeletonNode::create()
{
    SkeletonNode* skeletionNode = new (std::nothrow) SkeletonNode();
    if (skeletionNode && skeletionNode->init())
    {
        skeletionNode->autorelease();
        return skeletionNode;
    }
    CC_SAFE_DELETE(skeletionNode);
    return nullptr;
}


bool SkeletonNode::init()
{
    bool ret = LayerColor::init();
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
    _anchorPoint = Vec2(.5f, .5f);
    setContentSize(cocos2d::Size(30, 30));
    setGLProgramState(cocos2d::GLProgramState::getOrCreateWithGLProgramName(cocos2d::GLProgram::SHADER_NAME_POSITION_COLOR_NO_MVP));
    if (nullptr == _skeletonDraw)
    {
        _skeletonDraw = cocos2d::DrawNode::create();
        signSkeletonDrawDirty();
        this->addChild(_skeletonDraw, INT_MAX);
    }
    return ret;
}

SkeletonNode::SkeletonNode()
{
}

SkeletonNode::~SkeletonNode()
{
}

void SkeletonNode::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags)
{
    if (isSkeletonDrawDirty())
        _skeletonDraw->clear();

    BoneNode::visit(renderer, parentTransform, parentFlags);

    signSkeletonDrawDirty(false);
}


void SkeletonNode::resetSkeletonDrawNode(cocos2d::DrawNode* skeletonDrawNode)
{
    CCASSERT(nullptr != _skeletonDraw, "Skeleton's _skeletonDraw canot be null");
    if ( nullptr != skeletonDrawNode)
    {
        this->removeChild(_skeletonDraw);
        _skeletonDraw = skeletonDrawNode;
    }
    else // init a new draw node
    {
        if (nullptr == _skeletonDraw)
        {
            _skeletonDraw = cocos2d::DrawNode::create();
            this->addChild(_skeletonDraw, INT_MAX);
        }
    }
    signSkeletonDrawDirty();
}

void SkeletonNode::setLength(float length)
{
    setContentSize(cocos2d::Size(length, length));
}

void SkeletonNode::setContentSize(const cocos2d::Size &size)
{
    if (!size.equals(_contentSize))
    {
        _contentSize = size;
    }
    BoneNode::setLength(size.height);
}

void SkeletonNode::updateVertices()
{
    const float radius = _contentSize.width * .5f;
    if (radius != _squareVertices[1].x * 2)
    {
        _transformUpdated = _transformDirty = _inverseDirty = _contentSizeDirty = true;
    }

    _squareVertices[0].x = _squareVertices[1].y = _squareVertices[2].x = _squareVertices[3].y = .0f;
    _squareVertices[0].y = _squareVertices[3].x = - radius;
    _squareVertices[1].x = _squareVertices[2].y = radius;
    signSkeletonDrawDirty();
}

void SkeletonNode::drawBoneRack()
{
    const float offset = getLength() / 10.f;
    cocos2d::Vec2 v1(0, offset), v2(0, -offset), v3(offset, 0), v4(-offset, 0);
    if (nullptr == _skeletonDraw)
    {
        CCLOG(" SkeletonNode's _skeletonDraw can not be nullptr");
    }
    else
    {
        if (_skeletonDraw != nullptr)
        {
            _skeletonDraw->drawTriangle(v3, v4, _squareVertices[0], getBoneRackColor());
            _skeletonDraw->drawTriangle(v1, v2, _squareVertices[1], getBoneRackColor());
            _skeletonDraw->drawTriangle(v3, v4, _squareVertices[2], getBoneRackColor());
            _skeletonDraw->drawTriangle(v1, v2, _squareVertices[3], getBoneRackColor());
        }
    }
}

NS_TIMELINE_END