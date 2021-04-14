#include "engine.h"
#include "prefix_shared.h"
#include "prefix_editor.h"

using namespace Editor;

#ifdef EWIN

Engine::ESharedBuffer GetFilterList(const EVector<EString>& filters)
{
    // Example filter string: "(*.jpg;*.png;*.bmp)\0*.jpg;*.png;*.bmp\0"
        // Get the buffer length
        //               2 * "("     all the ";"             all the "*"       Null Termination
    size_t bufferLength = 2 + (filters.size() - 1) * 2 + filters.size() * 2 + 2;
    for (const EString& f : filters)
    {
        bufferLength += f.length() * 2;
    }
    // (*esc)\0*esc\0
    Engine::ESharedBuffer filter;
    filter.InitWith<char>(bufferLength);
    char* ptr = filter.Data<char>();

    *ptr = '(';
    ptr++;

    for (size_t i = 0; i < filters.size(); i++)
    {
        const EString& f = filters[i];

        *ptr = '*';
        ptr++;
        memcpy(ptr, f.c_str(), f.length());
        ptr += f.length();

        if (i < filters.size() - 1)
        {
            *ptr = ';';
            ptr++;
        }
    }

    *ptr = ')';
    ptr++;

    *ptr = '\0';
    ptr++;

    for (size_t i = 0; i < filters.size(); i++)
    {
        const EString& f = filters[i];

        *ptr = '*';
        ptr++;
        memcpy(ptr, f.c_str(), f.length());
        ptr += f.length();

        if (i < filters.size() - 1)
        {
            *ptr = ';';
            ptr++;
        }
    }


    *ptr = '\0';
    ptr++;
    return filter;
}



EVector<EString> Platform::OpenFileDialog(const EString& title, const EVector<EString>& filters, const EString& defaultPath)
{
    Engine::ESharedBuffer filter = GetFilterList(filters);

    OPENFILENAME ofn = {0}; 
    TCHAR szFile[260]={0};
    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(ofn); 
    ofn.hwndOwner = NULL; 
    ofn.lpstrFile = szFile; 
    ofn.nMaxFile = sizeof(szFile); 
    ofn.lpstrFilter = filter.Data<char>();
    ofn.nFilterIndex = 1; 
    ofn.lpstrFileTitle = NULL; 
    ofn.nMaxFileTitle = 0; 
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if(GetOpenFileName(&ofn) == TRUE)
    { 
    // use ofn.lpstrFile here
        return {ofn.lpstrFile};
    }
    return {};
}

EString Platform::SaveFileDialog(const EString& title, const EVector<EString>& allowedEndings)
{
    Engine::ESharedBuffer filter = GetFilterList(allowedEndings);

    OPENFILENAME ofn = { 0 };
    TCHAR szFile[260] = { 0 };
    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter.Data<char>();
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    //ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetSaveFileName(&ofn) == TRUE)
    {
        // use ofn.lpstrFile here
        return ofn.lpstrFile;
    }
    return {};
}
#endif