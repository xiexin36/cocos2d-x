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


#ifndef __CCBONENODE_H__
#define __CCBONENODE_H__

#include "base/CCProtocols.h"
#include "2d/CCLayer.h"
#include "CCTimelineMacro.h"
#include "cocostudio/CocosStudioExport.h"

#include "CCSkinNode.h"

NS_TIMELINE_BEGIN
class SkeletonNode;

class CC_STUDIO_DLL BoneNode : public Node, public cocos2d::BlendProtocol
{
public:
    static BoneNode* create();
    static BoneNode* create(const int &length);
    static BoneNode* create(const int &length, const cocos2d::Color4F & color);

    virtual void addChildBone(BoneNode* bone);

    /*
    * @param cleaup : True if all running actions and callbacks 
    *                 on the child node will be cleanup, false otherwise.
    */
    virtual void removeChildBone(BoneNode* bone, bool cleaup = false);
    virtual void clearChildBones(bool cleaup, bool recursive = false);

    virtual const std::map<std::string, BoneNode*>& getChildBones() const { return _childBones; }
    virtual std::map<std::string, BoneNode*>&  getChildBones() { return _childBones; }

    virtual void removeFromParentBone(bool cleanup = false);

 // Skins
    /* !Add skin to bone
    /* @param hide this skin
    */
    virtual void addSkin(SkinNode* skin, bool hide = false);
    virtual void removeSkin(SkinNode* skin);

    virtual const std::map<std::string, SkinNode*>& getSkins() const { return _boneSkins; }
    virtual std::map<std::string, SkinNode*>&  getSkins() { return _boneSkins; }

    /*
    * @param cleaup : True if all running actions and callbacks on
                      the child node will be cleanup, false otherwise.
    */
    virtual void clearSkins(bool cleanup);


    virtual void display(SkinNode* skin, bool hideOthers = false);
    virtual void display(const std::string& skinName, bool hideOthers = false);

    virtual std::map<std::string, SkinNode*> getDisplaying() const;

    // blendFunc
    virtual void setBlendFunc(const cocos2d::BlendFunc &blendFunc) override;
    virtual const BlendFunc & getBlendFunc() const override { return _blendFunc; }


    // bone operate
    virtual void setLength(float length);
    virtual float getLength() const { return _length; }

    // no need to change this at usual
    virtual void setWidth(float width);
    virtual float getWidth() const { return _width; }

    // is rack show
    virtual void setRackShow(bool ishow);
    virtual bool isRackShow() const { return _isRackShow; }
protected:
    BoneNode();
    virtual ~BoneNode();
    // override
    virtual void setContentSize(const cocos2d::Size &size) override;
    virtual bool init() override;

    virtual void updateVertices();
    virtual void updateColor() override;

    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
    virtual void onDraw(const cocos2d::Mat4 &transform, uint32_t flags); // for test
protected:
    CustomCommand _customCommand;
    BlendFunc     _blendFunc;

    float            _length;
    float            _width;
    bool             _isRackShow;

    std::map<std::string, BoneNode*> _childBones;
    std::map<std::string, SkinNode*> _boneSkins;
    SkeletonNode*                    _rootBoneNode;

private:
    Vec2          _squareVertices[4];
    Color4F       _squareColors[4];
    Vec3          _noMVPVertices[4];
    CC_DISALLOW_COPY_AND_ASSIGN(BoneNode);
};

NS_TIMELINE_END
#endif //__CCBONENODE_H__