#include "editor_extension.h"



using namespace Engine;
using namespace Graphics;
using namespace Renderer;



E_STORAGE_STRUCT(ImageLayer,
    (bool, Visible),
    (EResourceLink, resourceLink, "Image"),
    (EString, SomeString)
)



class ImageLayerView : public EUIField
{
private:
    EUnorderedMap<ERegister::Entity, EWeakRef<EUIImageView>> fImageViews;
public:
    ImageLayerView()
        : EUIField("ImageView")
    {
        shared::Events().AddComponentCreateEventListener(ImageLayer::_dsc, [this](ERegister::Entity handle){
            ImageLayer imageLayer = shared::GetValue<ImageLayer>(handle);

            ERef<EUIImageView> newImageView = EMakeRef<EUIImageView>();
            newImageView->SetSize(250, 250);
            fImageViews[handle] = newImageView;
            AddChild(newImageView);
        }, this);

        shared::Events().AddComponentDeleteEventListener(ImageLayer::_dsc, [this](ERegister::Entity handle){
            EUnorderedMap<ERegister::Entity, EWeakRef<EUIImageView>>::iterator it = fImageViews.find(handle);
            if (it != fImageViews.end())
            {
                RemoveChild(it->second);
                fImageViews.erase(it);
            }
        }, this);

        shared::Events().AddEntityChangeEventListener("ImageLayer.resourceLink", [this](ERegister::Entity handle, const EString&){
            ImageLayer imageLayer = shared::GetValue<ImageLayer>(handle);
            ERef<EResourceData> data = shared::GetResource(imageLayer.resourceLink.ResourceId);
            
            if (data && fImageViews.find(handle) != fImageViews.end())
            {
                Editor::EImageUserData* userData = (Editor::EImageUserData*)data->UserData;
                if (userData)
                {
                    fImageViews[handle].lock()->SetTextureData(data->Data, userData->width, userData->height);
                }
            }
        }, this);
    }
};

APP_ENTRY
{
    ERef<EUIPanel> showPanel = EMakeRef<EUIPanel>("Show Panel");
    showPanel->AddChild(EMakeRef<ImageLayerView>());
    info.PanelRegister->RegisterItem(extensionName, showPanel);
}

EXT_ENTRY
{
   info.GetTypeRegister().RegisterItem(extensionName, ImageLayer::_dsc);
}