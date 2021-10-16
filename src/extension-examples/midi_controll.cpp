#include "editor_extension.h"
#include "RtMidi.h"


using namespace Engine;
using namespace Graphics;
using namespace Renderer;

static std::atomic<bool> midiRunning = false;
static std::thread* fMidiThread = nullptr;
static RtMidiIn* midi = nullptr;
static std::condition_variable waitForInput;
static u8 lastUsedInput = 0;

u8 WaitForNextMidiInput()
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    waitForInput.wait(lock);
    return lastUsedInput;
}

void CreateControllerEdit(ERef<EUIPanel> editPanel);
void HandleMidi()
{
    while (midiRunning)
    {
        if (!midi)
        {
            break;
        }
        std::vector<u8> message;
        double stamp;

        stamp = midi->getMessage(&message);
        size_t nBytes = message.size();
        lastUsedInput = 0;
        if (nBytes > 0)
        {
            if (nBytes >= 2)
            {
                lastUsedInput = message[1];
            }
            waitForInput.notify_all();
        }
        for ( size_t i=0; i<nBytes; i++ )
            std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
        if ( nBytes > 0 )
            std::cout << "stamp = " << stamp << std::endl;
    }
}

APP_ENTRY
{
    midi = new RtMidiIn();
    u32 portCount = midi->getPortCount();
    if (portCount == 0)
    {
        E_ERROR("No MIDI-Ports found!");
        delete midi;
        midi = nullptr;
        return;
    }
    midi->openPort();
    midi->ignoreTypes(false, false, false);
    

    midiRunning = true;
    fMidiThread = new std::thread(HandleMidi);

    ERef<EUIPanel> midiPanel = EMakeRef<EUIPanel>("Midi Edit");
    CreateControllerEdit(midiPanel);


    info.PanelRegister->RegisterItem(extensionName, midiPanel);
}

APP_CLEANUP
{
    midiRunning = false;
    if (fMidiThread->joinable())
    {
        fMidiThread->join();
    }
    delete fMidiThread;
    fMidiThread = nullptr;
}

EXT_ENTRY
{
    
}






void CreateControllerEdit(ERef<EUIPanel> editPanel)
{
    EWeakRef<EUIField> addModeButton = editPanel->AddChild(EMakeRef<EUIButton>("Add Mode"));
    EWeakRef<EUIField> addInputButton = editPanel->AddChild(EMakeRef<EUIButton>("Add Input"));
    EWeakRef<EUISelectionList> selectionList = std::dynamic_pointer_cast<EUISelectionList>(editPanel->AddChild(EMakeRef<EUISelectionList>()).lock());

    addInputButton.lock()->SetVisible(false);

    addModeButton.lock()->AddEventListener<events::EButtonEvent>([addInputButton](){

        addInputButton.lock()->SetVisible(true);
    });

    addInputButton.lock()->AddEventListener<events::EButtonEvent>([selectionList](){
        u8 midiInput = WaitForNextMidiInput();
        selectionList.lock()->AddOption(std::to_string(midiInput));
    });
}
