#include "editor_extension.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


using namespace Engine;
using namespace Graphics;
using namespace Renderer;

static ERegister* activeScene;

struct ImageUserData
{
    int width;
    int height;
    int channels;
};

E_STORAGE_STRUCT(ImageLayer,
    (bool, Visible),
    (EResourceLink, resourceLink, "Image")
)

EResourceDescription::ResBuffer ImportImage(const EResourceDescription::RawBuffer data)
{
    EResourceDescription::ResBuffer result;
    int x, y, n;
    result.Data = stbi_load_from_memory(data.Data, data.Size, &x, &y, &n, 4);
    result.Size = x * y * 4;

    ImageUserData* userData = new ImageUserData;
    userData->width = x;
    userData->height = y;
    userData->channels = 4;//n; ?

    result.UserData = (byte*) userData;
    result.UserDataSize = sizeof(ImageUserData);

    return result;
}

class ImageLayerView : public EUIField
{
private:
    EUnorderedMap<ERegister::Entity, EWeakRef<EUIImageView>> fImageViews;
public:
    ImageLayerView()
        : EUIField("ImageView")
    {
        activeScene->AddComponentCreateEventListener(ImageLayer::_dsc, [this](ERegister::Entity handle){
            ERef<EUIImageView> newImageView = EMakeRef<EUIImageView>();
            newImageView->SetSize(100, 100);
            fImageViews[handle] = newImageView;
            AddChild(newImageView);
        });

        activeScene->AddEntityChangeEventListener("ImageLayer.resourceLink",[this](ERegister::Entity handle, const EString&){
            ImageLayer imageLayer = activeScene->GetComponent<ImageLayer>(handle);

            EResourceData* data = EExtensionManager::instance().GetActiveScene()->GetResourceManager().GetResource(imageLayer.resourceLink.ResourceId);
            if (data && fImageViews.find(handle) != fImageViews.end())
            {
                ImageUserData* userData = (ImageUserData*)data->UserData;
                if (userData)
                {
                    fImageViews[handle].lock()->SetTextureData(data->Data, userData->width, userData->height);
                }
            }
        });
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
    activeScene = info.GetActiveScene();

    EResourceDescription imageDsc("Image",{"png", "jpeg", "bmp"});
    imageDsc.ImportFunction = &ImportImage;




    info.GetResourceRegister().RegisterItem(extensionName, imageDsc);
    info.GetTypeRegister().RegisterItem(extensionName, ImageLayer::_dsc);
}