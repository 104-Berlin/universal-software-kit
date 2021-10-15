#include "editor_extension.h"
#include "RtMidi.h"


using namespace Engine;
using namespace Graphics;
using namespace Renderer;

static std::atomic<bool> midiRunning = false;
static std::thread* fMidiThread = nullptr;
static RtMidiIn* midi = nullptr;


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