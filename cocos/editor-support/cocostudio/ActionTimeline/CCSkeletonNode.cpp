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
    bool ret = BoneNode::init();
    if (nullptr == _skeletonDraw)
    {
        _skeletonDraw = cocos2d::DrawNode::create();
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

void SkeletonNode::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    if (_skeletonDraw != nullptr)
    {
        if ( this->isBoneRackShow() && !_skeletonDraw->isVisible())
        {
            _skeletonDraw->clear();
            this->updateBoneRackDraw(true);
            _skeletonDraw->setVisible(true);
        }
    }
    else
    {
        BoneNode::draw(renderer, transform, flags);
    }
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
}

NS_TIMELINE_END