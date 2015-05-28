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

class CC_STUDIO_DLL BoneNode : public cocos2d::LayerColor//, public cocos2d::BlendProtocol
{
public:
    static BoneNode* create();
    static BoneNode* create(const int &length);
    static BoneNode* create(const int &length, const cocos2d::Color4F & color);

 // Skeleton bones
    virtual void addChildBone(BoneNode* bone);

    /*
    * @param cleaup : True if all running actions and callbacks on the child node will be cleanup, false otherwise.
    */
    virtual void removeChildBone(BoneNode* bone, bool cleaup = false);
    virtual void clearChildBones(bool cleaup, bool recursive = false);

    /*
    *@return a new vector 
    */
    virtual std::vector<BoneNode*> getChildrenBones(bool recursive = false);  // need a BoneNodeChain

    virtual const std::vector<BoneNode*>& getChildrenBones() const { return _boneChildren; }
    virtual std::vector<BoneNode*>&  getChildrenBones() { return _boneChildren; }

    virtual void removeFromParentBone(bool cleanup = false);

 // Skins
    /* !Add skin to bone
    /* @param hide this skin
    */
    virtual void addSkin(SkinNode* skin, bool hide = false);
    virtual void removeSkin(SkinNode* skin);

//     virtual void insertSkin(int index, SkinNode* skin, bool hide = false);
//     virtual void removeSkin(int index);
// 
//     virtual int getIndexOfSkin(SkinNode* skin);
//     virtual SkinNode* getSkinAt(int index) const;
    
    virtual const std::vector<SkinNode*>& getSkins() const { return _boneSkins; }
    virtual std::vector<SkinNode*>&  getSkins() { return _boneSkins; }

    /*
    * @param cleaup : True if all running actions and callbacks on the child node will be cleanup, false otherwise.
    */
    virtual void clearSkins(bool cleanup);


    virtual void display(SkinNode* skin, bool hideOthers = false);
//     virtual void Display(int index, bool hideOthers = false);

    virtual std::vector<SkinNode*> getDisplaying() const;

//blendfunc getter,setter
    virtual void setBlendFunc(const cocos2d::BlendFunc &blendFunc) override;

    // extend functions 

    ////get <bonename, displayingSkinname> of bone tree
    //std::map<std::string, std::string> GetTreeBoneSkinDisplayMap() const;
    //void ChangeDisplays(std::map<std::string, std::string> boneSkinMap);

    // bone operate
    virtual void setLength(float length);
    virtual float getLength() const { return _length; }
    virtual void setWidth(float width);
    virtual float getWidth() const { return _width; }

    // color
    virtual void setBoneRackColor(const cocos2d::Color4F &color);
    virtual const cocos2d::Color4F& getBoneRackColor() const { return _rackColor; }

    virtual void setBoneRackShow(bool isShowRack) { _showRack = isShowRack; }
    virtual bool isBoneRackShow() const { return _showRack; }

    virtual void setPosition(float x, float y) override;
protected:
    BoneNode();
    virtual ~BoneNode();
    // override
    virtual void setContentSize(const cocos2d::Size &size) override;
    virtual bool init() override;

    virtual void updateVertices();
    virtual void updateVecticesColor();
    virtual void resetSkeletonDrawNode(cocos2d::DrawNode* skeletonDrawNode);
    virtual void signSkeletonDrawDirty(bool dirty = true);
    virtual bool isSkeletonDrawDirty();
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
    virtual void onDraw(const cocos2d::Mat4 &transform, uint32_t flags); // for test
    virtual void drawBoneRack();  // draw rack on _skeletonDraw


    std::vector<BoneNode*> _boneChildren;
    std::vector<SkinNode*> _boneSkins;
    // bone draw
    cocos2d::DrawNode* _skeletonDraw;
private:
    float _length;
    float _width;

    bool _showRack;
    bool _skinCascadeBlendFunc;

    cocos2d::Color4F _rackColor;
};

NS_TIMELINE_END
#endif //__CCBONENODE_H__