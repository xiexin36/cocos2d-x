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

#ifndef __CCSKINNODE_H__
#define  __CCSKINNODE_H__

#include "2d/CCNode.h"
#include "2d/CCSprite.h"
#include "2d/CCParticleSystemQuad.h"
#include "base/CCProtocols.h"


#include "CCFrame.h"
#include "CCTimelineMacro.h"
#include "cocostudio/CocosStudioExport.h"


NS_TIMELINE_BEGIN

typedef cocos2d::Node SkinNode;
///************************************************************************/
///*                                                                      */
///************************************************************************/
//
//class CC_STUDIO_DLL SkinNode: public cocos2d::Node, public cocos2d::TextureProtocol 
//{
//public:
//    SkinNode(){}
//    template<typename NodeType>
//    SkinNode(NodeType node) = 0;
//protected:
//    virtual ~SkinNode(){}
//
//private:
//
//};
//
//class CC_STUDIO_DLL SpriteSkinNode : public cocos2d::Sprite, public SkinNode
//{
//public:
//    static SpriteSkinNode* create();
//    static SpriteSkinNode* create(const std::string& filename);
//    SpriteSkinNode(cocos2d::Sprite * sprite);
//
//    virtual void setBlendFunc(const cocos2d::BlendFunc &blendFunc);
//    virtual const cocos2d::BlendFunc &getBlendFunc() const;
//
//    virtual cocos2d::Texture2D* getTexture() const;
//    virtual void setTexture(cocos2d::Texture2D *texture);
//protected:
//    SpriteSkinNode();
//    virtual ~SpriteSkinNode();
//};
//
//class CC_STUDIO_DLL ParticleSkinNode : public cocos2d::ParticleSystemQuad, public SkinNode
//{
//public:
//    ParticleSkinNode(cocos2d::ParticleSystem* particle);
//
//    virtual void setBlendFunc(const cocos2d::BlendFunc &blendFunc);
//    virtual const cocos2d::BlendFunc &getBlendFunc() const;
//
//
//    virtual cocos2d::Texture2D* getTexture() const;
//    virtual void setTexture(cocos2d::Texture2D *texture);
//protected:
//    ParticleSkinNode();
//    virtual ~ParticleSkinNode();
//};

NS_TIMELINE_END

#endif //__CCSKINNODE_H__
