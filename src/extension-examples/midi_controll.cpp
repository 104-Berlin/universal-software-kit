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
static size_t selectedMidiInputIndex = 0;
static EResource::t_ID currentResourceId;


enum class MidiInputType : size_t
{
    BUTTON = 0,
    SLIDER = 1,
    KNOB = 2,
    MODE_SWITCH = 3
};

struct MidiInput
{
    u8 KeyCode;
    MidiInputType InputType;
    EString Name;
    float Min;
    float Max;

    MidiInput()
        : MidiInput(0)
        {}
    MidiInput(u8 keyCode)
        : KeyCode(keyCode), Name(), Min(0.0f), Max(100.0f), InputType(MidiInputType::BUTTON)
        {}
};


struct MidiMode
{
    EVector<MidiInput> Inputs;
};

struct MidiController
{
    EVector<MidiMode> Modes;
};

static MidiController CurrentMidiController;

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
    EResourceDescription midiResource("MidiConfig", {"midicfg"});
    midiResource.CanCreate = true;
    info.GetResourceRegister().RegisterItem(extensionName, midiResource);
}


void LoadFromResource(ESharedBuffer buffer)
{
    
}




void CreateControllerEdit(ERef<EUIPanel> editPanel)
{
    CurrentMidiController.Modes.push_back(MidiMode());

    ERef<EUIField> menuBar = EMakeRef<EUIMenu>("File");
    EWeakRef<EUIField> saveMenuItem = menuBar->AddChild(EMakeRef<EUIMenuItem>("Save"));
    saveMenuItem.lock()->AddEventListener<events::EButtonEvent>([](){

    });

    editPanel->SetMenuBar(menuBar);

    EWeakRef<EUIField> resourceSelect = editPanel->AddChild(EMakeRef<EUIResourceSelect>("MidiConfig"));


    EWeakRef<EUIField> editContainer = editPanel->AddChild(EMakeRef<EUIContainer>());
    editContainer.lock()->SetVisible(false);

    EWeakRef<EUIField> addModeButton = editContainer.lock()->AddChild(EMakeRef<EUIButton>("Add Mode"));
    EWeakRef<EUIField> addInputButton = editContainer.lock()->AddChild(EMakeRef<EUIButton>("Add Input"));
    EWeakRef<EUIField> buttonSelectContainer = editContainer.lock()->AddChild(EMakeRef<EUIContainer>());
    editContainer.lock()->AddChild(EMakeRef<EUISameLine>());
    EWeakRef<EUIField> buttonOptionContainer = editContainer.lock()->AddChild(EMakeRef<EUIContainer>());
    buttonOptionContainer.lock()->SetDirty();
    EWeakRef<EUISelectionList> selectionList = std::dynamic_pointer_cast<EUISelectionList>(buttonSelectContainer.lock()->AddChild(EMakeRef<EUISelectionList>()).lock());
    buttonSelectContainer.lock()->SetWidth(200.0f);


    resourceSelect.lock()->AddEventListener<events::EResourceSelectChangeEvent>([editContainer](events::EResourceSelectChangeEvent event){
        editContainer.lock()->SetVisible(event.ResourceID);
        currentResourceId = event.ResourceID;
    });

    addModeButton.lock()->AddEventListener<events::EButtonEvent>([addInputButton](){
        CurrentMidiController.Modes.push_back(MidiMode());
    });

    addInputButton.lock()->AddEventListener<events::EButtonEvent>([selectionList](){
        u8 midiInput = WaitForNextMidiInput();
        if (std::find_if(CurrentMidiController.Modes[0].Inputs.begin(), CurrentMidiController.Modes[0].Inputs.end(), [midiInput](MidiInput in){return midiInput == in.KeyCode;}) != CurrentMidiController.Modes[0].Inputs.end()) { return; }
        CurrentMidiController.Modes[0].Inputs.push_back(MidiInput(midiInput));
        selectionList.lock()->AddOption(std::to_string((int)midiInput));
    });

    selectionList.lock()->AddEventListener<events::ESelectChangeEvent>([buttonOptionContainer](events::ESelectChangeEvent event){
        selectedMidiInputIndex = event.Index;
        if (!buttonOptionContainer.expired())
        {
            buttonOptionContainer.lock()->SetDirty();
        }
    });

    buttonOptionContainer.lock()->SetCustomUpdateFunction([buttonOptionContainer](){
        if (buttonOptionContainer.expired()) { return; }
        buttonOptionContainer.lock()->Clear();
        if (CurrentMidiController.Modes[0].Inputs.size() == 0) { return; }
        
        EWeakRef<EUIDropdown> buttonTypeSelect = std::static_pointer_cast<EUIDropdown>(buttonOptionContainer.lock()->AddChild(EMakeRef<EUIDropdown>("", EVector<EString>({"Button", "Slider", "Knob", "ModeSwitch"}))).lock());
        buttonTypeSelect.lock()->SetSelectedIndex((size_t)CurrentMidiController.Modes[0].Inputs[selectedMidiInputIndex].InputType);
        EWeakRef<EUITextField> nameField = std::static_pointer_cast<EUITextField>(buttonOptionContainer.lock()->AddChild(EMakeRef<EUITextField>("Name")).lock());
        nameField.lock()->SetValue(CurrentMidiController.Modes[0].Inputs[selectedMidiInputIndex].Name);
        
        EWeakRef<EUIFloatEdit> minInput = std::static_pointer_cast<EUIFloatEdit>(buttonOptionContainer.lock()->AddChild(EMakeRef<EUIFloatEdit>("Min")).lock());
        minInput.lock()->SetVisible(CurrentMidiController.Modes[0].Inputs[selectedMidiInputIndex].InputType != MidiInputType::BUTTON);
        minInput.lock()->SetValue(CurrentMidiController.Modes[0].Inputs[selectedMidiInputIndex].Min);
        EWeakRef<EUIFloatEdit> maxInput = std::static_pointer_cast<EUIFloatEdit>(buttonOptionContainer.lock()->AddChild(EMakeRef<EUIFloatEdit>("Max")).lock());
        maxInput.lock()->SetVisible(CurrentMidiController.Modes[0].Inputs[selectedMidiInputIndex].InputType != MidiInputType::BUTTON);
        maxInput.lock()->SetValue(CurrentMidiController.Modes[0].Inputs[selectedMidiInputIndex].Max);


        buttonTypeSelect.lock()->AddEventListener<events::ESelectChangeEvent>([minInput, maxInput](events::ESelectChangeEvent e){
            MidiInputType type = (MidiInputType)e.Index;
            CurrentMidiController.Modes[0].Inputs[selectedMidiInputIndex].InputType = type;
            minInput.lock()->SetVisible(type != MidiInputType::BUTTON);
            maxInput.lock()->SetVisible(type != MidiInputType::BUTTON);
        });
        nameField.lock()->AddEventListener<events::ETextCompleteEvent>([](events::ETextCompleteEvent event){
            CurrentMidiController.Modes[0].Inputs[selectedMidiInputIndex].Name = event.Value;
        });
        minInput.lock()->AddEventListener<events::EFloatCompleteEvent>([](events::EFloatCompleteEvent event){
            CurrentMidiController.Modes[0].Inputs[selectedMidiInputIndex].Min = event.Value;
        });
        maxInput.lock()->AddEventListener<events::EFloatCompleteEvent>([](events::EFloatCompleteEvent event){
            CurrentMidiController.Modes[0].Inputs[selectedMidiInputIndex].Max = event.Value;
        });
    });
}
