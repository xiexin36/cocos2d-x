

#include "CheckBoxReader.h"
#include "ui/UICheckBox.h"
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
    static const char* P_BackGroundBoxData = "backGroundBoxData";
    static const char* P_BackGroundBoxSelectedData = "backGroundBoxSelectedData";
    static const char* P_FrontCrossData = "frontCrossData";
    static const char* P_BackGroundBoxDisabledData = "backGroundBoxDisabledData";
    static const char* P_FrontCrossDisabledData = "frontCrossDisabledData";
    
    static CheckBoxReader* instanceCheckBoxReader = nullptr;
    
    IMPLEMENT_CLASS_WIDGET_READER_INFO(CheckBoxReader)
    
    CheckBoxReader::CheckBoxReader()
    {
        
    }
    
    CheckBoxReader::~CheckBoxReader()
    {
        
    }
    
    CheckBoxReader* CheckBoxReader::getInstance()
    {
        if (!instanceCheckBoxReader)
        {
            instanceCheckBoxReader = new CheckBoxReader();
        }
        return instanceCheckBoxReader;
    }
    
    void CheckBoxReader::setPropsFromBinary(cocos2d::ui::Widget *widget, CocoLoader *cocoLoader, stExpCocoNode *cocoNode)
    {
        
        CheckBox *checkBox = static_cast<CheckBox*>(widget);
        this->beginSetBasicProperties(widget);
        stExpCocoNode *stChildArray = cocoNode->GetChildArray(cocoLoader);
        
        for (int i = 0; i < cocoNode->GetChildNum(); ++i) {
            std::string key = stChildArray[i].GetName(cocoLoader);
            std::string value = stChildArray[i].GetValue(cocoLoader);
//            CCLOG("key = %s, index : %d", key.c_str(), i);
            //read all basic properties of widget
            CC_BASIC_PROPERTY_BINARY_READER
            //read all color related properties of widget
            CC_COLOR_PROPERTY_BINARY_READER
            
            else if (key == P_BackGroundBoxData){
                
                stExpCocoNode *backGroundChildren = stChildArray[i].GetChildArray(cocoLoader);
                std::string resType = backGroundChildren[2].GetValue(cocoLoader);;
                
                Widget::TextureResType imageFileNameType = (Widget::TextureResType)valueToInt(resType);
                
                std::string backgroundValue = this->getResourcePath(cocoLoader, &stChildArray[i], imageFileNameType);
                
                checkBox->loadTextureBackGround(backgroundValue, imageFileNameType);
            }else if(key == P_BackGroundBoxSelectedData){
                stExpCocoNode *backGroundChildren = stChildArray[i].GetChildArray(cocoLoader);
                std::string resType = backGroundChildren[2].GetValue(cocoLoader);;
                
                Widget::TextureResType imageFileNameType = (Widget::TextureResType)valueToInt(resType);
                
                std::string backgroundValue = this->getResourcePath(cocoLoader, &stChildArray[i], imageFileNameType);
                
                checkBox->loadTextureBackGroundSelected(backgroundValue, imageFileNameType);
            }else if(key == P_FrontCrossData){
                stExpCocoNode *backGroundChildren = stChildArray[i].GetChildArray(cocoLoader);
                std::string resType = backGroundChildren[2].GetValue(cocoLoader);;
                
                Widget::TextureResType imageFileNameType = (Widget::TextureResType)valueToInt(resType);
                
                std::string backgroundValue = this->getResourcePath(cocoLoader, &stChildArray[i], imageFileNameType);
                
                checkBox->loadTextureFrontCross(backgroundValue, imageFileNameType);
            }else if(key == P_BackGroundBoxDisabledData){
                stExpCocoNode *backGroundChildren = stChildArray[i].GetChildArray(cocoLoader);
                std::string resType = backGroundChildren[2].GetValue(cocoLoader);;
                
                Widget::TextureResType imageFileNameType = (Widget::TextureResType)valueToInt(resType);
                
                std::string backgroundValue = this->getResourcePath(cocoLoader, &stChildArray[i], imageFileNameType);
                
                checkBox->loadTextureBackGroundDisabled(backgroundValue, imageFileNameType);
            }else if (key == P_FrontCrossDisabledData){
                stExpCocoNode *backGroundChildren = stChildArray[i].GetChildArray(cocoLoader);
                std::string resType = backGroundChildren[2].GetValue(cocoLoader);;
                
                Widget::TextureResType imageFileNameType = (Widget::TextureResType)valueToInt(resType);
                
                std::string backgroundValue = this->getResourcePath(cocoLoader, &stChildArray[i], imageFileNameType);
                
                checkBox->loadTextureFrontCrossDisabled(backgroundValue, imageFileNameType);
            }
//            else if (key == "selectedState"){
//                checkBox->setSelectedState(valueToBool(value));
//            }
        }
        
        this->endSetBasicProperties(widget);
        
        
    }
    
    void CheckBoxReader::setPropsFromJsonDictionary(Widget *widget, const rapidjson::Value &options)
    {
        WidgetReader::setPropsFromJsonDictionary(widget, options);
        
        CheckBox* checkBox = static_cast<CheckBox*>(widget);
       
        //load background image
        const rapidjson::Value& backGroundDic = DICTOOL->getSubDictionary_json(options, P_BackGroundBoxData);
        int backGroundType = DICTOOL->getIntValue_json(backGroundDic,P_ResourceType);
        std::string backGroundTexturePath = this->getResourcePath(backGroundDic, P_Path, (Widget::TextureResType)backGroundType);
         checkBox->loadTextureBackGround(backGroundTexturePath, (Widget::TextureResType)backGroundType);
        
       //load background selected image
        const rapidjson::Value& backGroundSelectedDic = DICTOOL->getSubDictionary_json(options, P_BackGroundBoxSelectedData);
        int backGroundSelectedType = DICTOOL->getIntValue_json(backGroundSelectedDic, P_ResourceType);
        std::string backGroundSelectedTexturePath = this->getResourcePath(backGroundSelectedDic, P_Path, (Widget::TextureResType)backGroundSelectedType);
        checkBox->loadTextureBackGroundSelected(backGroundSelectedTexturePath, (Widget::TextureResType)backGroundSelectedType);
        
        //load frontCross image
        const rapidjson::Value& frontCrossDic = DICTOOL->getSubDictionary_json(options, P_FrontCrossData);
        int frontCrossType = DICTOOL->getIntValue_json(frontCrossDic, P_ResourceType);
        std::string frontCrossFileName = this->getResourcePath(frontCrossDic, P_Path, (Widget::TextureResType)frontCrossType);
        checkBox->loadTextureFrontCross(frontCrossFileName, (Widget::TextureResType)frontCrossType);
        
       //load backGroundBoxDisabledData
        const rapidjson::Value& backGroundDisabledDic = DICTOOL->getSubDictionary_json(options, P_BackGroundBoxDisabledData);
        int backGroundDisabledType = DICTOOL->getIntValue_json(backGroundDisabledDic, P_ResourceType);
        std::string backGroundDisabledFileName = this->getResourcePath(backGroundDisabledDic, P_Path, (Widget::TextureResType)backGroundDisabledType);
        checkBox->loadTextureBackGroundDisabled(backGroundDisabledFileName, (Widget::TextureResType)backGroundDisabledType);
        
        ///load frontCrossDisabledData
        const rapidjson::Value& frontCrossDisabledDic = DICTOOL->getSubDictionary_json(options, P_FrontCrossDisabledData);
        int frontCrossDisabledType = DICTOOL->getIntValue_json(frontCrossDisabledDic, P_ResourceType);
        std::string frontCrossDisabledFileName = this->getResourcePath(frontCrossDisabledDic, P_Path, (Widget::TextureResType)frontCrossDisabledType);
        checkBox->loadTextureFrontCrossDisabled(frontCrossDisabledFileName, (Widget::TextureResType)frontCrossDisabledType);
        
        
        WidgetReader::setColorPropsFromJsonDictionary(widget, options);
    }
    
    void CheckBoxReader::setPropsFromProtocolBuffers(ui::Widget *widget, const protocolbuffers::NodeTree &nodeTree)
    {
        WidgetReader::setPropsFromProtocolBuffers(widget, nodeTree);
        
        CheckBox* checkBox = static_cast<CheckBox*>(widget);
        const protocolbuffers::CheckBoxOptions& options = nodeTree.checkboxoptions();

		std::string protocolBuffersPath = GUIReader::getInstance()->getFilePath();
        
        //load background image
		const protocolbuffers::ResourceData& backGroundDic = options.backgroundboxdata();
        int backGroundType = backGroundDic.resourcetype();
        std::string backGroundTexturePath = this->getResourcePath(backGroundDic.path(), (Widget::TextureResType)backGroundType);
        checkBox->loadTextureBackGround(backGroundTexturePath, (Widget::TextureResType)backGroundType);
        
        //load background selected image
        const protocolbuffers::ResourceData& backGroundSelectedDic = options.backgroundboxselecteddata();
        int backGroundSelectedType = backGroundSelectedDic.resourcetype();
        std::string backGroundSelectedTexturePath = this->getResourcePath(backGroundSelectedDic.path(), (Widget::TextureResType)backGroundSelectedType);
        checkBox->loadTextureBackGroundSelected(backGroundSelectedTexturePath, (Widget::TextureResType)backGroundSelectedType);
        
        //load frontCross image
        const protocolbuffers::ResourceData& frontCrossDic = options.frontcrossdata();
        int frontCrossType = frontCrossDic.resourcetype();
        std::string frontCrossFileName = this->getResourcePath(frontCrossDic.path(), (Widget::TextureResType)frontCrossType);
        checkBox->loadTextureFrontCross(frontCrossFileName, (Widget::TextureResType)frontCrossType);
        
        //load backGroundBoxDisabledData
        const protocolbuffers::ResourceData& backGroundDisabledDic = options.backgroundboxdisableddata();
        int backGroundDisabledType = backGroundDisabledDic.resourcetype();
        std::string backGroundDisabledFileName = this->getResourcePath(backGroundDisabledDic.path(), (Widget::TextureResType)backGroundDisabledType);
        checkBox->loadTextureBackGroundDisabled(backGroundDisabledFileName, (Widget::TextureResType)backGroundDisabledType);
        
        ///load frontCrossDisabledData
        const protocolbuffers::ResourceData& frontCrossDisabledDic = options.frontcrossdisableddata();
        int frontCrossDisabledType = frontCrossDisabledDic.resourcetype();
        std::string frontCrossDisabledFileName = this->getResourcePath(frontCrossDisabledDic.path(), (Widget::TextureResType)frontCrossDisabledType);
        checkBox->loadTextureFrontCrossDisabled(frontCrossDisabledFileName, (Widget::TextureResType)frontCrossDisabledType);
        
        checkBox->setSelectedState(options.selectedstate());
        
		bool displaystate = true;
		if(options.has_displaystate())
		{
			displaystate = options.displaystate();
		}
		checkBox->setBright(displaystate);
        checkBox->setEnabled(displaystate);
        
        // other commonly protperties
        WidgetReader::setColorPropsFromProtocolBuffers(widget, nodeTree);
    }
    
    /* peterson */
    void CheckBoxReader::setPropsWithFlatBuffers(cocos2d::ui::Widget *widget, const flatbuffers::Options *options)
    {
        WidgetReader::setPropsWithFlatBuffers(widget, options);
        
        CheckBox* checkBox = static_cast<CheckBox*>(widget);
        auto cbop = options->checkBoxOptions();
        
        //load background image
        auto backGroundDic = cbop->backGroundBoxData();
        int backGroundType = backGroundDic->resourceType();
        std::string backGroundTexturePath = this->getResourcePath(backGroundDic->path()->c_str(), (Widget::TextureResType)backGroundType);
        checkBox->loadTextureBackGround(backGroundTexturePath, (Widget::TextureResType)backGroundType);
        
        //load background selected image
        auto backGroundSelectedDic = cbop->backGroundBoxSelectedData();
        int backGroundSelectedType = backGroundSelectedDic->resourceType();
        std::string backGroundSelectedTexturePath = this->getResourcePath(backGroundSelectedDic->path()->c_str(), (Widget::TextureResType)backGroundSelectedType);
        checkBox->loadTextureBackGroundSelected(backGroundSelectedTexturePath, (Widget::TextureResType)backGroundSelectedType);
        
        //load frontCross image
        auto frontCrossDic = cbop->frontCrossData();
        int frontCrossType = frontCrossDic->resourceType();
        std::string frontCrossFileName = this->getResourcePath(frontCrossDic->path()->c_str(), (Widget::TextureResType)frontCrossType);
        checkBox->loadTextureFrontCross(frontCrossFileName, (Widget::TextureResType)frontCrossType);
        
        //load backGroundBoxDisabledData
        auto backGroundDisabledDic = cbop->backGroundBoxDisabledData();
        int backGroundDisabledType = backGroundDisabledDic->resourceType();
        std::string backGroundDisabledFileName = this->getResourcePath(backGroundDisabledDic->path()->c_str(), (Widget::TextureResType)backGroundDisabledType);
        checkBox->loadTextureBackGroundDisabled(backGroundDisabledFileName, (Widget::TextureResType)backGroundDisabledType);
        
        ///load frontCrossDisabledData
        auto frontCrossDisabledDic = cbop->frontCrossDisabledData();
        int frontCrossDisabledType = frontCrossDisabledDic->resourceType();
        std::string frontCrossDisabledFileName = this->getResourcePath(frontCrossDisabledDic->path()->c_str(), (Widget::TextureResType)frontCrossDisabledType);
        checkBox->loadTextureFrontCrossDisabled(frontCrossDisabledFileName, (Widget::TextureResType)frontCrossDisabledType);
        
        bool selectedstate = cbop->selectedState();
        checkBox->setSelected(selectedstate);
        
        bool displaystate = cbop->displaystate();
        checkBox->setBright(displaystate);
        
        // other commonly protperties
        WidgetReader::setColorPropsWithFlatBuffers(widget, options);
    }
    /**/        

    int CheckBoxReader::getResourceType(std::string key)
	{
		if(key == "Normal" || key == "Default" || key == "MarkedSubImage")
		{
			return 	0;	
		}
	
		return 1;
	}
}
