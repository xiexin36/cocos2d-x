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

#include "UILayoutComponent.h"
#include "2d/CCNode.h"
#include "GUIDefine.h"


NS_CC_BEGIN

namespace ui {
    LayoutComponent::LayoutComponent()
    : _usingPercentContentSize(false)
    , _referencePoint(ReferencePoint::BOTTOM_LEFT)
    , _usingPercentPosition(false)
    , _actived(true)
    {
        _name = __LAYOUT_COMPONENT_NAME;
    }
    
    LayoutComponent::~LayoutComponent()
    {
        
    }
    
    bool LayoutComponent::init()
    {
        bool ret = true;
        do
        {
            if (!Component::init())
            {
                ret = false;
                break;
            }
            
            //put layout component initalized code here
            
        } while (0);
        return ret;
    }
    
#pragma region OldVersion
    //Size
    Vec2 LayoutComponent::getOwnerContentSize()const
    {
        return this->getOwner()->getContentSize();
    }
    void LayoutComponent::setOwnerContentSize(const Vec2& size)
    {
        this->getOwner()->setContentSize(Size(size.x,size.y));

        Node* parentNode = this->getOwner()->getParent();
        if (parentNode != NULL && _actived)
        {
            Size parentSize = parentNode->getContentSize();

            if (parentSize.width != 0 && parentSize.height != 0)
            {
                _percentContentSize = Point(size.x/parentSize.width,size.y/parentSize.height);
            }
            else
            {
                _percentContentSize = Point(0,0);
            }
        }
    }

    const Vec2& LayoutComponent::getPercentContentSize()const
    {
        return _percentContentSize;
    }
    
    void LayoutComponent::setPercentContentSize(const Vec2& percent)
    {
        _percentContentSize = percent;

        Node* parentNode = this->getOwner()->getParent();
        if (parentNode != NULL && _actived)
        {
            Size parentSize = parentNode->getContentSize();
            if (_usingPercentContentSize)
            {
                this->getOwner()->setContentSize(Size(percent.x*parentSize.width,percent.y*parentSize.height));
            }
        }
    }
    
    bool LayoutComponent::isUsingPercentContentSize()
    {
        return _usingPercentContentSize;
    }

    void LayoutComponent::setUsingPercentContentSize(bool flag)
    {
        _usingPercentContentSize = flag;

        Node* parentNode = this->getOwner()->getParent();
        if (parentNode != NULL && _actived)
        {
            Size parentSize = parentNode->getContentSize();
            if (_usingPercentContentSize)
            {
                Size baseSize = this->getOwner()->getContentSize();
                if (parentSize.width != 0)
                {
                    _percentContentSize.x = baseSize.width/parentSize.width;
                }
                else
                {
                    _percentContentSize.x = 0;
                    baseSize.width = 0;
                }

                if (parentSize.height != 0)
                {
                    _percentContentSize.y = baseSize.height/parentSize.height;
                }
                else
                {
                    _percentContentSize.y = 0;
                    baseSize.height = 0;
                }

                this->getOwner()->setContentSize(baseSize);
            }
        }
    }

    //Position
    bool LayoutComponent::isUsingPercentPosition()
    {
        return _usingPercentPosition;
    }
    void LayoutComponent::setUsingPercentPosition(bool flag)
    {
        _usingPercentPosition = flag;

        Node* parentNode = this->getOwner()->getParent();
        if (parentNode != NULL && _actived)
        {
            Size parentSize = parentNode->getContentSize();

            if (_usingPercentPosition)
            {
                if (parentSize.width != 0)
                {
                    _percentPosition.x = _relativePosition.x/parentSize.width;
                }
                else
                {
                    _percentPosition.x = 0;
                    _relativePosition.x = 0;
                }

                if (parentSize.height != 0)
                {
                    _percentPosition.y = _relativePosition.y/parentSize.height;
                }
                else
                {
                    _percentPosition.y = 0;
                    _relativePosition.y = 0;
                }
            }

            Point inversePoint = this->converPointWithReferencePointAndSize(_relativePosition,parentSize);
            this->getOwner()->setPosition(inversePoint);
        }
    }

    const Vec2& LayoutComponent::getPercentPosition()
    {
        return _percentPosition;
    }
    void LayoutComponent::setPercentPosition(const Vec2& percent)
    {
        _percentPosition = percent;

        Node* parentNode = this->getOwner()->getParent();
        if (parentNode != NULL && _actived)
        {
            Size parentSize = parentNode->getContentSize();
            _relativePosition = Point(_percentPosition.x*parentSize.width,_percentPosition.y*parentSize.height);
            Point inversePoint = this->converPointWithReferencePointAndSize(_relativePosition,parentSize);
            this->getOwner()->setPosition(inversePoint);
        }
    }

    const Vec2& LayoutComponent::getOwnerPosition()const
    {
        return this->getOwner()->getPosition();
    }
    void LayoutComponent::setOwnerPosition(const Vec2& point)
    {
        Node* parentNode = this->getOwner()->getParent();
        if (parentNode != NULL && _actived)
        {
            Size parentSize = parentNode->getContentSize();

            Point inversePoint = this->converPointWithReferencePointAndSize(point,parentSize);
            this->getOwner()->setPosition(point);
            _relativePosition = inversePoint;
            if (parentSize.width != 0 && parentSize.height != 0)
            {
                _percentPosition = Point(_relativePosition.x/parentSize.width,_relativePosition.y/parentSize.height);
            }
            else
            {
                _percentPosition = Point(0,0);
            }
        }
        else
        {
            this->getOwner()->setPosition(point);
            if (_referencePoint == ReferencePoint::BOTTOM_LEFT)
            {
                _relativePosition = point;
            }
        }

    }

    const Vec2& LayoutComponent::getRelativePosition()
    {
        return _relativePosition;
    }
    void LayoutComponent::setRelativePosition(const Vec2& position)
    {
        _relativePosition = position;

        Node* parentNode = this->getOwner()->getParent();
        if (parentNode != NULL && _actived)
        {
            Size parentSize = parentNode->getContentSize();

            Point inversePoint = this->converPointWithReferencePointAndSize(_relativePosition,parentSize);
            this->getOwner()->setPosition(inversePoint);
            if (parentSize.width != 0 && parentSize.height != 0)
            {
                _percentPosition = Point(_relativePosition.x/parentSize.width,_relativePosition.y/parentSize.height);
            }
            else
            {
                _percentPosition = Point(0,0);
            }
        }
    }

    LayoutComponent::ReferencePoint LayoutComponent::getReferencePoint()
    {
        return _referencePoint;
    }
    void LayoutComponent::setReferencePoint(ReferencePoint point)
    {
        _referencePoint = point;
        this->setRelativePosition(_relativePosition);
    }

    void LayoutComponent::setActiveEnable(bool enable)
    {
        _actived = enable;
    }

    Vec2 LayoutComponent::converPointWithReferencePointAndSize(const Vec2& point,const Size& size)
    {
        Point inversePoint = point;
        switch (_referencePoint)
        {
        case ReferencePoint::TOP_LEFT:
            inversePoint.y = size.height - inversePoint.y;
            break;
        case ReferencePoint::BOTTOM_RIGHT:
            inversePoint.x = size.width - inversePoint.x;
            break;
        case ReferencePoint::TOP_RIGHT:
            inversePoint.x = size.width - inversePoint.x;
            inversePoint.y = size.height - inversePoint.y;
            break;
        default:
            break;
        }
        return inversePoint;
    }
#pragma endregion

#pragma region Position & Margin
    Point LayoutComponent::getPosition()
    {
        return _owner->getPosition();
    }
    void LayoutComponent::setPosition(Point position)
    {
        _owner->setPosition(position);

        Node* parent = _owner->getParent();

        switch (this->_horizontalEage)
        {
        case HorizontalEage::Left:
            _horizontalMargin = position.x;
            break;
        case HorizontalEage::Right:
            if (parent != nullptr)
            {
                _horizontalMargin = parent->getContentSize().width - position.x;
            }
            break;
        default:
            break;
        }

        switch (this->_verticalEage)
        {
        case VerticalEage::Buttom:
            _verticalMargin = position.y;
            break;
        case VerticalEage::Top:
            if (parent != nullptr)
            {
                _verticalMargin = parent->getContentSize().height - position.y;
            }
            break;
        default:
            break;
        }

        if (parent != nullptr)
        {
            Size parentSize = parent->getContentSize();
            if (parentSize.width != 0)
                _horizontalPercentMargin = _horizontalMargin / parentSize.width;
            else
                _horizontalPercentMargin = 0;

            if (parentSize.height != 0)
                _verticalPercentMargin = _verticalMargin / parentSize.height;
            else
                _verticalPercentMargin = 0;
        }
    }

    LayoutComponent::HorizontalEage LayoutComponent::getHorizontalEage()
    {
        return _horizontalEage;
    }
    void LayoutComponent::setHorizontalEage(HorizontalEage hEage)
    {
        _horizontalEage = hEage;

        Node* parent = _owner->getParent();
        switch (this->_horizontalEage)
        {
        case HorizontalEage::Left:
            _horizontalMargin = _owner->getPosition().x;
            break;
        case HorizontalEage::Right:
            if (parent != nullptr)
            {
                _horizontalMargin = parent->getContentSize().width - _owner->getPosition().x;
            }
            break;
        default:
            break;
        }

        if (parent != nullptr)
        {
            Size parentSize = parent->getContentSize();
            if (parentSize.width != 0)
                _horizontalPercentMargin = _horizontalMargin / parentSize.width;
            else
                _horizontalPercentMargin = 0;
        }
    }

    bool LayoutComponent::isUsingHorizontalPercent()
    {
        return _usingHorizontalPercent;
    }
    void LayoutComponent::setHorizontalPercentUsedState(bool isUsed)
    {
        _usingHorizontalPercent = isUsed;
    }

    float LayoutComponent::getHorizontalMargin()
    {
        return _horizontalMargin;
    }
    void LayoutComponent::setHorizontalMargin(float margin)
    {
        _horizontalMargin = margin;

        Node* parent = _owner->getParent();
        switch (this->_horizontalEage)
        {
        case HorizontalEage::Left:
            _owner->setPositionX(_horizontalMargin);
            break;
        case HorizontalEage::Right:
            if (parent != nullptr)
            {
                _owner->setPositionX(parent->getContentSize().width - _horizontalMargin);
            }
            break;
        default:
            break;
        }

        if (parent != nullptr)
        {
            Size parentSize = parent->getContentSize();
            if (parentSize.width != 0)
                _horizontalPercentMargin = _horizontalMargin / parentSize.width;
            else
                _horizontalPercentMargin = 0;
        }
    }

    float LayoutComponent::getHorizontalPercentMargin()
    {
        return _horizontalPercentMargin;
    }
    void LayoutComponent::setHorizontalPercentMargin(float percentMargin)
    {
        _horizontalPercentMargin = percentMargin;

        Node* parent = _owner->getParent();
        if (parent != nullptr)
        {
            Size parentSize = parent->getContentSize();
            _horizontalMargin = parentSize.width * _horizontalPercentMargin;

            switch (this->_horizontalEage)
            {
            case HorizontalEage::Left:
                _owner->setPositionX(_horizontalMargin);
                break;
            case HorizontalEage::Right:
                if (parent != nullptr)
                {
                    _owner->setPositionX(parentSize.width - _horizontalMargin);
                }
                break;
            default:
                break;
            }
        }
    }

    LayoutComponent::VerticalEage LayoutComponent::getVerticalEage()
    {
        return _verticalEage;
    }
    void LayoutComponent::setVerticalEage(VerticalEage vEage)
    {
        _verticalEage = vEage;

        Node* parent = _owner->getParent();
        switch (this->_verticalEage)
        {
        case VerticalEage::Buttom:
            _verticalMargin = _owner->getPosition().y;
            break;
        case VerticalEage::Top:
            if (parent != nullptr)
            {
                _verticalMargin = parent->getContentSize().height - _owner->getPosition().y;
            }
            break;
        default:
            break;
        }

        if (parent != nullptr)
        {
            Size parentSize = parent->getContentSize();
            if (parentSize.height != 0)
                _verticalPercentMargin = _verticalMargin / parentSize.height;
            else
                _verticalPercentMargin = 0;
        }
    }

    bool LayoutComponent::isUsingVerticalPercent()
    {
        return _usingVerticalPercnet;
    }
    void LayoutComponent::setVerticalPercentUsedState(bool isUsed)
    {
        _usingVerticalPercnet = isUsed;
    }

    float LayoutComponent::getVerticalMargin()
    {
        return _verticalMargin;
    }
    void LayoutComponent::setVerticalMargin(float margin)
    {
        _verticalMargin = margin;

        Node* parent = _owner->getParent();
        switch (this->_verticalEage)
        {
        case VerticalEage::Buttom:
            _owner->setPositionY(_verticalMargin);
            break;
        case VerticalEage::Top:
            if (parent != nullptr)
            {
                _owner->setPositionY(parent->getContentSize().height - _verticalMargin);
            }
            break;
        default:
            break;
        }

        if (parent != nullptr)
        {
            Size parentSize = parent->getContentSize();
            if (parentSize.height != 0)
                _verticalPercentMargin = _verticalMargin / parentSize.height;
            else
                _verticalPercentMargin = 0;
        }
    }

    float LayoutComponent::getVerticalPercentMargin()
    {
        return _verticalPercentMargin;
    }
    void LayoutComponent::setVerticalPercentMargin(float percentMargin)
    {
        _verticalPercentMargin = percentMargin;

        Node* parent = _owner->getParent();
        if (parent != nullptr)
        {
            Size parentSize = parent->getContentSize();
            _verticalMargin = parentSize.height * _verticalPercentMargin;

            switch (this->_verticalEage)
            {
            case VerticalEage::Buttom:
                _owner->setPositionY(_verticalMargin);
                break;
            case VerticalEage::Top:
                if (parent != nullptr)
                {
                    _owner->setPositionY(parentSize.height - _verticalMargin);
                }
                break;
            default:
                break;
            }
        }
    }
#pragma endregion

#pragma region Size & Percent
    Size LayoutComponent::getSize()
    {
        return this->getOwner()->getContentSize();
    }
    void LayoutComponent::setSize(Size _size)
    {

    }

    LayoutComponent::SizeType LayoutComponent::getSizeType()
    {
        return _sizeType;
    }
    void LayoutComponent::changeSizeType(SizeType type)
    {
        _sizeType = type;

        Node* parent = _owner->getParent();

        Size ownerSize = _owner->getContentSize();
        switch (this->_sizeType)
        {
        case SizeType::Normal:
            _relativeWidth = ownerSize.width;
            _relativeHeight = ownerSize.height;
            break;
        case SizeType::Inverse:
            if (parent != nullptr)
            {
                Size parentSize = parent->getContentSize();
                _relativeWidth = parentSize.width - ownerSize.width;
                _relativeHeight = parentSize.height - ownerSize.height;
            }
            break;
        default:
            break;
        }

        if (parent != nullptr)
        {
            Size parentSize = parent->getContentSize();
            if (parentSize.width != 0)
                _percentWidth = _relativeWidth / parentSize.width;
            else
                _percentWidth = 0;

            if (parentSize.height != 0)
                _percentHeight = _relativeHeight / parentSize.height;
            else
                _percentHeight = 0;
        }
    }

    bool LayoutComponent::isUsingPercentWidth()
    {
        return _usingPercentWidth;
    }
    void LayoutComponent::setPercentWidthUsedState(bool isUsed)
    {
        _usingPercentWidth = isUsed;
    }

    float LayoutComponent::getRelativeWidth()
    {
        return _relativeWidth;
    }
    void LayoutComponent::setRelativeWidth(float width)
    {
        _relativeWidth = width;

        Node* parent = _owner->getParent();
        Size ownerSize = _owner->getContentSize();
        switch (this->_sizeType)
        {
        case SizeType::Normal:
            ownerSize.width = _relativeWidth;
            break;
        case SizeType::Inverse:
            if (parent != nullptr)
            {
                ownerSize.width = parent->getContentSize().width - _relativeWidth;
            }
            break;
        default:
            break;
        }

        _owner->setContentSize(ownerSize);
        if (parent != nullptr)
        {
            Size parentSize = parent->getContentSize();
            if (parentSize.width != 0)
                _percentWidth = _relativeWidth / parentSize.width;
            else
                _percentWidth = 0;
        }
    }

    float LayoutComponent::getPercentWidth()
    {
        return _percentWidth;
    }
    void LayoutComponent::setPercentWidth(float percentWidth)
    {
        _percentWidth = percentWidth;

        Node* parent = _owner->getParent();
        if (parent != nullptr)
        {
            _relativeWidth = parent->getContentSize().width * _percentWidth;
            Size ownerSize = _owner->getContentSize();
            switch (this->_sizeType)
            {
            case SizeType::Normal:
                ownerSize.width = _relativeWidth;
                break;
            case SizeType::Inverse:
                if (parent != nullptr)
                {
                    ownerSize.width = parent->getContentSize().width - _relativeWidth;
                }
                break;
            default:
                break;
            }

            _owner->setContentSize(ownerSize);
        }
    }

    bool LayoutComponent::isUsingPercentHeight()
    {
        return _usingPercentHeight;
    }
    void LayoutComponent::setPercentHeightUsedState(bool isUsed)
    {
        _usingPercentHeight = isUsed;
    }

    float LayoutComponent::getRelativeHeight()
    {
        return _relativeHeight;
    }
    void LayoutComponent::setRelativeHeight(float height)
    {
        _relativeHeight = height;

        Node* parent = _owner->getParent();
        Size ownerSize = _owner->getContentSize();
        switch (this->_sizeType)
        {
        case SizeType::Normal:
            ownerSize.height = _relativeHeight;
            break;
        case SizeType::Inverse:
            if (parent != nullptr)
            {
                ownerSize.height = parent->getContentSize().height - _relativeHeight;
            }
            break;
        default:
            break;
        }

        _owner->setContentSize(ownerSize);
        if (parent != nullptr)
        {
            Size parentSize = parent->getContentSize();
            if (parentSize.height != 0)
                _percentHeight = _relativeHeight / parentSize.height;
            else
                _percentHeight = 0;
        }
    }

    float LayoutComponent::getPercentHeight()
    {
        return _percentHeight;
    }
    void LayoutComponent::setPercentHeight(float percentHeight)
    {
        _percentHeight = percentHeight;

        Node* parent = _owner->getParent();
        if (parent != nullptr)
        {
            _relativeHeight = parent->getContentSize().height * _percentHeight;
            Size ownerSize = _owner->getContentSize();
            switch (this->_sizeType)
            {
            case SizeType::Normal:
                ownerSize.height = _relativeHeight;
                break;
            case SizeType::Inverse:
                if (parent != nullptr)
                {
                    ownerSize.height = parent->getContentSize().height - _relativeHeight;
                }
                break;
            default:
                break;
            }

            _owner->setContentSize(ownerSize);
        }
    }
#pragma endregion

    void LayoutComponent::refreshLayout()
    {
        Node* parent = _owner->getParent();
        if (parent == nullptr)
            return;

        Size parentSize = parent->getContentSize();
        Size ownerSize = _owner->getContentSize();
        Point ownerPosition = _owner->getPosition();

        if (_usingHorizontalPercent)
        {
            _horizontalMargin = parentSize.width * _horizontalPercentMargin;
        }
        switch (this->_horizontalEage)
        {
        case HorizontalEage::Left:
            ownerPosition.x = _horizontalMargin;
            break;
        case HorizontalEage::Right:
            ownerPosition.x = parentSize.width - _horizontalMargin;
            break;
        default:
            break;
        }

        if (_usingVerticalPercnet)
        {
            _verticalMargin = parentSize.height * _verticalPercentMargin;
        }
        switch (this->_verticalEage)
        {
        case VerticalEage::Buttom:
            ownerPosition.y = _verticalMargin;
            break;
        case VerticalEage::Top:
            ownerPosition.y = parentSize.height - _verticalMargin;
            break;
        default:
            break;
        }


        if (_usingPercentWidth)
        {
            _relativeWidth = parentSize.width * _percentWidth;
        }
        if (_usingPercentHeight)
        {
            _relativeHeight = parentSize.height * _percentHeight;
        }
        switch (this->_sizeType)
        {
        case SizeType::Normal:
            ownerSize.width = _relativeWidth;
            ownerSize.height = _relativeHeight;
            break;
        case SizeType::Inverse:
            ownerSize.width = parentSize.width - _relativeWidth;
            ownerSize.height = parentSize.height - _relativeHeight;
            break;
        default:
            break;
        }

        _owner->setPosition(ownerPosition);
        _owner->setContentSize(ownerSize);
    }
}

NS_CC_END