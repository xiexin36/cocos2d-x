

#include "PageViewReader.h"
#include "ui/UIPageView.h"
#include "ui/UILayout.h"
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
    static PageViewReader* instancePageViewReader = nullptr;
    
    IMPLEMENT_CLASS_WIDGET_READER_INFO(PageViewReader)
    
    PageViewReader::PageViewReader()
    {
        
    }
    
    PageViewReader::~PageViewReader()
    {
        
    }
    
    PageViewReader* PageViewReader::getInstance()
    {
        if (!instancePageViewReader)
        {
            instancePageViewReader = new PageViewReader();
        }
        return instancePageViewReader;
    }
    
    void PageViewReader::setPropsFromBinary(cocos2d::ui::Widget *widget, CocoLoader *cocoLoader, stExpCocoNode *cocoNode)
	{
		LayoutReader::setPropsFromBinary(widget, cocoLoader, cocoNode);
    }
    
    void PageViewReader::setPropsFromJsonDictionary(Widget *widget, const rapidjson::Value &options)
    {
        LayoutReader::setPropsFromJsonDictionary(widget, options);
    }
    
    void PageViewReader::setPropsFromProtocolBuffers(ui::Widget *widget, const protocolbuffers::NodeTree &nodeTree)
    {
        WidgetReader::setPropsFromProtocolBuffers(widget, nodeTree);
        
        
        PageView* pageView = static_cast<PageView*>(widget);
        const protocolbuffers::PageViewOptions& options = nodeTree.pageviewoptions();

		std::string protocolBuffersPath = GUIReader::getInstance()->getFilePath();
        
        CCLOG("options.clipable() = %d", options.clipable());
        pageView->setClippingEnabled(options.clipable());
        
        bool backGroundScale9Enable = options.backgroundscale9enable();
        pageView->setBackGroundImageScale9Enabled(backGroundScale9Enable);
        
        
        int cr;
        int cg;
        int cb;
        int scr;
        int scg;
        int scb;
        int ecr;
        int ecg;
        int ecb;
        
        cr = options.has_bgcolorr() ? options.bgcolorr() : 150;
        cg = options.has_bgcolorg() ? options.bgcolorg() : 150;
        cb = options.has_bgcolorb() ? options.bgcolorb() : 150;
        
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
        pageView->setBackGroundColorVector(Vec2(bgcv1, bgcv2));
        
        int co = options.has_bgcoloropacity() ? options.bgcoloropacity() : 100;
        
        int colorType = options.has_colortype() ? options.colortype() : 1;
        pageView->setBackGroundColorType(Layout::BackGroundColorType(colorType));
        
        pageView->setBackGroundColor(Color3B(scr, scg, scb),Color3B(ecr, ecg, ecb));
        pageView->setBackGroundColor(Color3B(cr, cg, cb));
        pageView->setBackGroundColorOpacity(co);
        
        
		const protocolbuffers::ResourceData& imageFileNameDic = options.backgroundimagedata();
        int imageFileNameType = imageFileNameDic.resourcetype();
        std::string imageFileName = this->getResourcePath(imageFileNameDic.path(), (Widget::TextureResType)imageFileNameType);
        pageView->setBackGroundImage(imageFileName, (Widget::TextureResType)imageFileNameType);
        
        
        if (backGroundScale9Enable)
        {
            float cx = options.capinsetsx();
            float cy = options.capinsetsy();
            float cw = options.has_capinsetswidth() ? options.capinsetswidth() : 1;
            float ch = options.has_capinsetsheight() ? options.capinsetsheight() : 1;
            pageView->setBackGroundImageCapInsets(Rect(cx, cy, cw, ch));
            bool sw = options.has_scale9width();
            bool sh = options.has_scale9height();
            if (sw && sh)
            {
                float swf = options.scale9width();
                float shf = options.scale9height();
                pageView->setContentSize(Size(swf, shf));
            }
        }
        
        const protocolbuffers::WidgetOptions& widgetOptions = nodeTree.widgetoptions();
        
        int red = widgetOptions.has_colorr() ? widgetOptions.colorr() : 255;
        int green = widgetOptions.has_colorg() ? widgetOptions.colorg() : 255;
        int blue = widgetOptions.has_colorb() ? widgetOptions.colorb() : 255;
        pageView->setColor(Color3B(red, green, blue));
        
        int opacity = widgetOptions.has_alpha() ? widgetOptions.alpha() : 255;
        pageView->setOpacity(opacity);
        
//        int bgimgcr = widgetOptions.has_colorr() ? widgetOptions.colorr() : 255;
//        int bgimgcg = widgetOptions.has_colorg() ? widgetOptions.colorg() : 255;
//        int bgimgcb = widgetOptions.has_colorb() ? widgetOptions.colorb() : 255;
//        pageView->setBackGroundImageColor(Color3B(bgimgcr, bgimgcg, bgimgcb));
//        
//        int bgimgopacity = widgetOptions.has_opacity() ? widgetOptions.opacity() : 255;
//        pageView->setBackGroundImageOpacity(bgimgopacity);
        
        
        // other commonly protperties
        setAnchorPointForWidget(widget, nodeTree);
        
        bool flipX = widgetOptions.flipx();
        bool flipY = widgetOptions.flipy();
        widget->setFlippedX(flipX);
        widget->setFlippedY(flipY);
    }
    
    /* peterson */
    void PageViewReader::setPropsWithFlatBuffers(cocos2d::ui::Widget *widget, const flatbuffers::Options *options)
    {
        WidgetReader::setPropsWithFlatBuffers(widget, options);
        
        PageView* pageView = static_cast<PageView*>(widget);
        auto pageop = options->pageViewOptions();
        
        bool clipEnabled = pageop->clipEnabled();
        pageView->setClippingEnabled(clipEnabled);
        
        bool backGroundScale9Enabled = pageop->backGroundScale9Enabled();
        pageView->setBackGroundImageScale9Enabled(backGroundScale9Enabled);
        
        
        auto f_bgColor = pageop->bgColor();
        Color3B bgColor(f_bgColor->r(), f_bgColor->g(), f_bgColor->b());
        auto f_bgStartColor = pageop->bgStartColor();
        Color3B bgStartColor(f_bgStartColor->r(), f_bgStartColor->g(), f_bgStartColor->b());
        auto f_bgEndColor = pageop->bgEndColor();
        Color3B bgEndColor(f_bgEndColor->r(), f_bgEndColor->g(), f_bgEndColor->b());
        
        auto f_colorVecor = pageop->colorVector();
        Vec2 colorVector(f_colorVecor->vectorX(), f_colorVecor->vectorY());
        pageView->setBackGroundColorVector(colorVector);
        
        int bgColorOpacity = pageop->bgColorOpacity();
        
        int colorType = pageop->colorType();
        pageView->setBackGroundColorType(Layout::BackGroundColorType(colorType));
        
        pageView->setBackGroundColor(bgStartColor, bgEndColor);
        pageView->setBackGroundColor(bgColor);
        pageView->setBackGroundColorOpacity(bgColorOpacity);
        
        
        auto imageFileNameDic = pageop->backGroundImageData();
        int imageFileNameType = imageFileNameDic->resourceType();
        std::string imageFileName = this->getResourcePath(imageFileNameDic->path()->c_str(), (Widget::TextureResType)imageFileNameType);
        pageView->setBackGroundImage(imageFileName, (Widget::TextureResType)imageFileNameType);
        
        
        if (backGroundScale9Enabled)
        {
            auto f_capInsets = pageop->capInsets();
            Rect capInsets(f_capInsets->x(), f_capInsets->y(), f_capInsets->width(), f_capInsets->height());
            pageView->setBackGroundImageCapInsets(capInsets);
            
            auto f_scale9Size = pageop->scale9Size();
            Size scale9Size(f_scale9Size->width(), f_scale9Size->height());
            pageView->setContentSize(scale9Size);
        }
        
        auto widgetOptions = options->widgetOptions();
        auto f_color = widgetOptions->color();
        Color3B color(f_color->r(), f_color->g(), f_color->b());
        pageView->setColor(color);
        
        int opacity = widgetOptions->alpha();
        pageView->setOpacity(opacity);
        
        
        // other commonly protperties
        WidgetReader::setColorPropsWithFlatBuffers(widget, options);
    }
    /**/        

}
