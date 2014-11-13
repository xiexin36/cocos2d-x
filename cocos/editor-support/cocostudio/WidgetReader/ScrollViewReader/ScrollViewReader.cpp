

#include "ScrollViewReader.h"
#include "ui/UIScrollView.h"
#include "cocostudio/CocoLoader.h"
#include "../../CSParseBinary.pb.h"
#include "tinyxml2/tinyxml2.h"
/* peterson */
#include "flatbuffers/flatbuffers.h"

#include "cocostudio/CSParseBinary_generated.h"
/**/

USING_NS_CC;
using namespace ui;

namespace cocostudio
{
    static const char* P_InnerWidth = "innerWidth";
    static const char* P_InnerHeight = "innerHeight";
    static const char* P_Direction = "direction";
    static const char* P_BounceEnable = "bounceEnable";
    
    static ScrollViewReader* instanceScrollViewReader = nullptr;
    
    IMPLEMENT_CLASS_WIDGET_READER_INFO(ScrollViewReader)
    
    ScrollViewReader::ScrollViewReader()
    {
        
    }
    
    ScrollViewReader::~ScrollViewReader()
    {
        
    }
    
    ScrollViewReader* ScrollViewReader::getInstance()
    {
        if (!instanceScrollViewReader)
        {
            instanceScrollViewReader = new ScrollViewReader();
        }
        return instanceScrollViewReader;
    }
    
    void ScrollViewReader::setPropsFromBinary(cocos2d::ui::Widget *widget, CocoLoader *cocoLoader, stExpCocoNode* cocoNode)
    {
        //TODO::need to refactor...
        LayoutReader::setPropsFromBinary(widget, cocoLoader, cocoNode);
        
        ScrollView* scrollView = static_cast<ScrollView*>(widget);
        
        stExpCocoNode *stChildArray = cocoNode->GetChildArray(cocoLoader);
        float innerWidth;
        float innerHeight;
        for (int i = 0; i < cocoNode->GetChildNum(); ++i) {
            std::string key = stChildArray[i].GetName(cocoLoader);
            std::string value = stChildArray[i].GetValue(cocoLoader);
            if (key == P_InnerWidth) {
                innerWidth = valueToFloat(value);
            }
            else if(key == P_InnerHeight){
                innerHeight = valueToFloat(value);
            }else if(key == P_Direction){
                scrollView->setDirection((ScrollView::Direction)valueToInt(value));
            }else if(key == P_BounceEnable){
                scrollView->setBounceEnabled(valueToBool(value));
            }
            
        } //end of for loop
        scrollView->setInnerContainerSize(Size(innerWidth, innerHeight));

    }
    
    void ScrollViewReader::setPropsFromJsonDictionary(Widget *widget, const rapidjson::Value &options)
    {
        LayoutReader::setPropsFromJsonDictionary(widget, options);
        
        
        ScrollView* scrollView = static_cast<ScrollView*>(widget);
        float innerWidth = DICTOOL->getFloatValue_json(options, P_InnerWidth,200);
        float innerHeight = DICTOOL->getFloatValue_json(options, P_InnerHeight,200);
        scrollView->setInnerContainerSize(Size(innerWidth, innerHeight));
        int direction = DICTOOL->getFloatValue_json(options, P_Direction,1);
        scrollView->setDirection((ScrollView::Direction)direction);
        scrollView->setBounceEnabled(DICTOOL->getBooleanValue_json(options, P_BounceEnable));
        
        
        LayoutReader::setColorPropsFromJsonDictionary(widget, options);
    }
    
    void ScrollViewReader::setPropsFromProtocolBuffers(ui::Widget *widget, const protocolbuffers::NodeTree &nodeTree)
    {
        WidgetReader::setPropsFromProtocolBuffers(widget, nodeTree);
        
        
        ScrollView* scrollView = static_cast<ScrollView*>(widget);
		const protocolbuffers::ScrollViewOptions& options = nodeTree.scrollviewoptions();

		std::string protocolBuffersPath = GUIReader::getInstance()->getFilePath();
        
        scrollView->setClippingEnabled(options.clipable());
        
        bool backGroundScale9Enable = options.backgroundscale9enable();
        scrollView->setBackGroundImageScale9Enabled(backGroundScale9Enable);
        
        
        int cr;
        int cg;
        int cb;
        int scr;
        int scg;
        int scb;
        int ecr;
        int ecg;
        int ecb;
        
        
        
        cr = options.has_bgcolorr() ? options.bgcolorr() : 255;
        cg = options.has_bgcolorg() ? options.bgcolorg() : 150;
        cb = options.has_bgcolorb() ? options.bgcolorb() : 100;
        
        scr = options.has_bgstartcolorr() ? options.bgstartcolorr() : 255;
        scg = options.has_bgstartcolorg() ? options.bgstartcolorg() : 255;
        scb = options.has_bgstartcolorb() ? options.bgstartcolorb() : 255;
        
        ecr = options.has_bgendcolorr() ? options.bgendcolorr() : 255;
        ecg = options.has_bgendcolorg() ? options.bgendcolorg() : 150;
        ecb = options.has_bgendcolorb() ? options.bgendcolorb() : 100;
        
		float bgcv1 = 0.0f;
        float bgcv2 = -0.5f;
		if(options.has_vectorx())
		{
			bgcv1 = options.vectorx();
		}
		if(options.has_vectory())
		{
			bgcv2 = options.vectory();
		}
        scrollView->setBackGroundColorVector(Vec2(bgcv1, bgcv2));
        
        int co = options.has_bgcoloropacity() ? options.bgcoloropacity() : 100;
        
        int colorType = options.has_colortype() ? options.colortype() : 1;
        scrollView->setBackGroundColorType(Layout::BackGroundColorType(colorType));
        
        scrollView->setBackGroundColor(Color3B(scr, scg, scb),Color3B(ecr, ecg, ecb));
        scrollView->setBackGroundColor(Color3B(cr, cg, cb));
        scrollView->setBackGroundColorOpacity(co);
        
        
		const protocolbuffers::ResourceData& imageFileNameDic = options.backgroundimagedata();
        int imageFileNameType = imageFileNameDic.resourcetype();
        std::string imageFileName = this->getResourcePath(imageFileNameDic.path(), (Widget::TextureResType)imageFileNameType);
        scrollView->setBackGroundImage(imageFileName, (Widget::TextureResType)imageFileNameType);
        
        
        if (backGroundScale9Enable)
        {
            float cx = options.capinsetsx();
            float cy = options.capinsetsy();
            float cw = options.has_capinsetswidth() ? options.capinsetswidth() : 1;
            float ch = options.has_capinsetsheight() ? options.capinsetsheight() : 1;
            scrollView->setBackGroundImageCapInsets(Rect(cx, cy, cw, ch));
            bool sw = options.has_scale9width();
            bool sh = options.has_scale9height();
            if (sw && sh)
            {
                float swf = options.scale9width();
                float shf = options.scale9height();
                scrollView->setContentSize(Size(swf, shf));
            }
        }
        
        scrollView->setLayoutType((Layout::Type)options.layouttype());
        
        const protocolbuffers::WidgetOptions& widgetOptions = nodeTree.widgetoptions();
        
        int red = widgetOptions.has_colorr() ? widgetOptions.colorr() : 255;
        int green = widgetOptions.has_colorg() ? widgetOptions.colorg() : 255;
        int blue = widgetOptions.has_colorb() ? widgetOptions.colorb() : 255;
        scrollView->setColor(Color3B(red, green, blue));
        
        int opacity = widgetOptions.has_alpha() ? widgetOptions.alpha() : 255;
        scrollView->setOpacity(opacity);
        
//        int bgimgcr = widgetOptions.has_colorr() ? widgetOptions.colorr() : 255;
//        int bgimgcg = widgetOptions.has_colorg() ? widgetOptions.colorg() : 255;
//        int bgimgcb = widgetOptions.has_colorb() ? widgetOptions.colorb() : 255;
//        scrollView->setBackGroundImageColor(Color3B(bgimgcr, bgimgcg, bgimgcb));
//        
//        int bgimgopacity = widgetOptions.has_opacity() ? widgetOptions.opacity() : 255;
//        scrollView->setBackGroundImageOpacity(bgimgopacity);
        
        
        
        
        float innerWidth = options.has_innerwidth() ? options.innerwidth() : 200;
        float innerHeight = options.has_innerheight() ? options.innerheight() : 200;
        scrollView->setInnerContainerSize(Size(innerWidth, innerHeight));
        int direction = options.has_direction() ? options.direction() : 1;
        scrollView->setDirection((ScrollView::Direction)direction);
        scrollView->setBounceEnabled(options.bounceenable());
        
        
        // other commonly protperties
        setAnchorPointForWidget(widget, nodeTree);
        
        bool flipX = widgetOptions.flipx();
        bool flipY = widgetOptions.flipy();
        widget->setFlippedX(flipX);
        widget->setFlippedY(flipY);
    }        
    
    /* peterson */
    void ScrollViewReader::setPropsWithFlatBuffers(cocos2d::ui::Widget *widget, const flatbuffers::Options *options)
    {
        WidgetReader::setPropsWithFlatBuffers(widget, options);
        
        ScrollView* scrollView = static_cast<ScrollView*>(widget);
        auto scrolop = options->scrollViewOptions();
        
        bool clipEnabled = scrolop->clipEnabled();
        scrollView->setClippingEnabled(clipEnabled);
        
        bool backGroundScale9Enabled = scrolop->backGroundScale9Enabled();
        scrollView->setBackGroundImageScale9Enabled(backGroundScale9Enabled);
        
        
        auto f_bgColor = scrolop->bgColor();
        Color3B bgColor(f_bgColor->r(), f_bgColor->g(), f_bgColor->b());
        auto f_bgStartColor = scrolop->bgStartColor();
        Color3B bgStartColor(f_bgStartColor->r(), f_bgStartColor->g(), f_bgStartColor->b());
        auto f_bgEndColor = scrolop->bgEndColor();
        Color3B bgEndColor(f_bgEndColor->r(), f_bgEndColor->g(), f_bgEndColor->b());
        
        auto f_colorVecor = scrolop->colorVector();
        Vec2 colorVector(f_colorVecor->vectorX(), f_colorVecor->vectorY());
        scrollView->setBackGroundColorVector(colorVector);
        
        int bgColorOpacity = scrolop->bgColorOpacity();
        
        int colorType = scrolop->colorType();
        scrollView->setBackGroundColorType(Layout::BackGroundColorType(colorType));
        
        scrollView->setBackGroundColor(bgStartColor, bgEndColor);
        scrollView->setBackGroundColor(bgColor);
        scrollView->setBackGroundColorOpacity(bgColorOpacity);
        
        
        auto imageFileNameDic = scrolop->backGroundImageData();
        int imageFileNameType = imageFileNameDic->resourceType();
        std::string imageFileName = this->getResourcePath(imageFileNameDic->path()->c_str(), (Widget::TextureResType)imageFileNameType);
        scrollView->setBackGroundImage(imageFileName, (Widget::TextureResType)imageFileNameType);
        
        
        if (backGroundScale9Enabled)
        {
            auto f_capInsets = scrolop->capInsets();
            Rect capInsets(f_capInsets->x(), f_capInsets->y(), f_capInsets->width(), f_capInsets->height());
            scrollView->setBackGroundImageCapInsets(capInsets);
            
            auto f_scale9Size = scrolop->scale9Size();
            Size scale9Size(f_scale9Size->width(), f_scale9Size->height());
            scrollView->setContentSize(scale9Size);
        }
        
        auto widgetOptions = options->widgetOptions();
        auto f_color = widgetOptions->color();
        Color3B color(f_color->r(), f_color->g(), f_color->b());
        scrollView->setColor(color);
        
        int opacity = widgetOptions->alpha();
        scrollView->setOpacity(opacity);
        
        auto f_innerSize = scrolop->innerSize();
        Size innerSize(f_innerSize->width(), f_innerSize->height());
        scrollView->setInnerContainerSize(innerSize);
        int direction = scrolop->direction();
        scrollView->setDirection((ScrollView::Direction)direction);
        bool bounceEnabled = scrolop->bounceEnabled();
        scrollView->setBounceEnabled(bounceEnabled);
        
        
        // other commonly protperties
        WidgetReader::setColorPropsWithFlatBuffers(widget, options);
    }
    /**/

}
