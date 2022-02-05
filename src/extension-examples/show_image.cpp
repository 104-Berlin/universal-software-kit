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
    EUnorderedMap<EDataBase::Entity, EWeakRef<EUIImageView>> fImageViews;
public:
    ImageLayerView()
        : EUIField("ImageView")
    {
        shared::Events().Connect<EntityChangeEvent>([this](EntityChangeEvent event){
            if (event.Type == EntityChangeType::COMPONENT_ADDED)
            {
                if (event.Data.Value())
                {
                    ImageLayer imageLayer;
                    if (convert::getter<ImageLayer>(event.Data.Value().get(), &imageLayer))
                    {
                        auto entity = event.Entity.Handle;

                        ERef<EUIImageView> imageView = EMakeRef<EUIImageView>();
                        //imageView->SetTextureData(imageLayer.resourceLink);
                        //imageView->SetVisible(imageLayer.Visible);
                        imageView->SetSize(100, 100);
                        fImageViews[entity] = imageView;
                        EUIField::AddChild(imageView);
                    }
                }
            }
            else if (event.Type == EntityChangeType::COMPONENT_CHANGED)
            {
                if (event.Data.Value())
                {
                    ImageLayer imageLayer;
                    if (convert::getter<ImageLayer>(event.Data.Value().get(), &imageLayer))
                    {
                        ERef<EResourceBase> data = shared::GetResource(imageLayer.resourceLink.ResourceId);
                        
                        if (data && fImageViews.find(event.Entity.Handle) != fImageViews.end())
                        {
                            Editor::EImageUserData* userData = (Editor::EImageUserData*)data->UserData;
                            if (userData)
                            {
                                fImageViews[event.Entity.Handle].lock()->SetTextureData(data->Data, userData->width, userData->height);
                            }
                        }
                    }
                }
            }
            else if (event.Type == EntityChangeType::COMPONENT_REMOVED || event.Type == EntityChangeType::ENTITY_DESTROYED)
            {
                if (fImageViews.find(event.Entity.Handle) != fImageViews.end())
                {
                    EUIField::RemoveChild(fImageViews[event.Entity.Handle]);
                    fImageViews.erase(event.Entity.Handle);
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
   info.GetComponentRegister().RegisterStruct<ImageLayer>(extensionName);
}
