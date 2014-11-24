/****************************************************************************
Copyright (c) 2013-2014 Chukong Technologies Inc.

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

#ifndef __cocos2d_libs__LayoutComponent__
#define __cocos2d_libs__LayoutComponent__
#include "2d/CCComponent.h"
#include "ui/GUIExport.h"

NS_CC_BEGIN

namespace ui {
    class CC_GUI_DLL LayoutComponent : public Component
    {
    public:
        LayoutComponent();
        ~LayoutComponent();
        virtual bool init()override;
        CREATE_FUNC(LayoutComponent);
#pragma region OldVersion
        /**
        * When a node has a ReferencePositoin with value equals LEFT_BOTTOM,
        * it will treat the left bottom corner of its parent as the origin(0,0) when positioning itself
        * which is the same as cocos2d-x does. But you can change it by assigning a
        * different ReferencePosition.
        * For example: If you use ReferencePosition with value equals RIGHT_TOP,
        * then it will treat the right top corner of its parent as the origin(0,0) when positioning itself.
        */
        enum class ReferencePoint
        {
            BOTTOM_LEFT,
            TOP_LEFT,
            BOTTOM_RIGHT,
            TOP_RIGHT
        };

        bool isUsingPercentPosition();
        void setUsingPercentPosition(bool flag);

        const Vec2& getPercentPosition();
        void setPercentPosition(const Vec2& percent);

        const Vec2& getRelativePosition();
        void setRelativePosition(const Vec2& position);

        void setReferencePoint(ReferencePoint point);
        ReferencePoint getReferencePoint();

        const Vec2& getOwnerPosition()const;
        void setOwnerPosition(const Vec2& point);

        Vec2 getOwnerContentSize()const;
        void setOwnerContentSize(const Vec2& size);

        const Vec2& getPercentContentSize()const;
        void setPercentContentSize(const Vec2& percent);

        bool isUsingPercentContentSize();
        void setUsingPercentContentSize(bool flag);

        void setActiveEnable(bool enable);
    private:
        Vec2 converPointWithReferencePointAndSize(const Vec2& point, const Size& size);
    private:

        Vec2 _percentContentSize;
        bool _usingPercentContentSize;

        ReferencePoint _referencePoint;
        Vec2 _relativePosition;
        Vec2 _percentPosition;
        bool _usingPercentPosition;
        bool _actived;
#pragma endregion

    public:
        enum class HorizontalEage
        {
            Left,
            Right
        };
        enum class VerticalEage
        {
            Buttom,
            Top
        };
        enum class SizeType
        {
            Normal,
            Inverse
        };

#pragma region Position & Margin
        virtual Point getPosition();
        virtual void setPosition(Point position);

        virtual HorizontalEage getHorizontalEage();
        virtual void setHorizontalEage(HorizontalEage hEage);

        virtual bool isUsingHorizontalPercent();
        virtual void setHorizontalPercentUsedState(bool isUsed);

        virtual float getHorizontalMargin();
        virtual void setHorizontalMargin(float margin);

        virtual float getHorizontalPercentMargin();
        virtual void setHorizontalPercentMargin(float percentMargin);

        virtual VerticalEage getVerticalEage();
        virtual void setVerticalEage(VerticalEage vEage);

        virtual bool isUsingVerticalPercent();
        virtual void setVerticalPercentUsedState(bool isUsed);

        virtual float getVerticalMargin();
        virtual void setVerticalMargin(float margin);

        virtual float getVerticalPercentMargin();
        virtual void setVerticalPercentMargin(float percentMargin);
#pragma endregion

#pragma region Size & Percent
        virtual Size getSize();
        virtual void setSize(Size _size);

        virtual SizeType getSizeType();
        virtual void changeSizeType(SizeType type);

        virtual bool isUsingPercentWidth();
        virtual void setPercentWidthUsedState(bool isUsed);

        virtual float getRelativeWidth();
        virtual void setRelativeWidth(float width);

        virtual float getPercentWidth();
        virtual void setPercentWidth(float percentWidth);

        virtual bool isUsingPercentHeight();
        virtual void setPercentHeightUsedState(bool isUsed);

        virtual float getRelativeHeight();
        virtual void setRelativeHeight(float height);

        virtual float getPercentHeight();
        virtual void setPercentHeight(float percentHeight);
#pragma endregion

        virtual void refreshLayout();

    protected:
        HorizontalEage  _horizontalEage;
        VerticalEage    _verticalEage;

        float           _horizontalMargin;
        bool            _usingHorizontalPercent;
        float           _horizontalPercentMargin;

        float           _verticalMargin;
        bool            _usingVerticalPercnet;
        float           _verticalPercentMargin;

        SizeType        _sizeType;

        float           _relativeWidth;
        float           _percentWidth;
        bool            _usingPercentWidth;

        float           _relativeHeight;
        float           _percentHeight;
        bool            _usingPercentHeight;
    };
}

NS_CC_END
#endif /* defined(__cocos2d_libs__LayoutComponent__) */
