#include "editor_extension.h"

#define NANOSVG_IMPLEMENTATION	// Expands implementation
#include "../../deps/usk-graphics/deps/nanosvg/src/nanosvg.h"

using namespace Editor;

void ESvgResource::FromBuffer(Engine::ESharedBuffer input)
{
    if (input.IsNull() || input.GetSizeInByte() == 0) { return; }
    char* buffer = new char[input.GetSizeInByte() + 1];
    memcpy(buffer, input.Data(), input.GetSizeInByte());
    buffer[input.GetSizeInByte()] = 0;

    NSVGimage* image;
    image = nsvgParse(buffer, "px", 32.0f);
    if (image)
    {
        Width = image->width;
        Height = image->height;
        
        float halfWidth = Width / 2.0f;
        float halfHeight = Height / 2.0f;
        for (auto shape = image->shapes; shape != NULL; shape = shape->next) {
            for (auto path = shape->paths; path != NULL; path = path->next) {
                for (int i = 0; i < path->npts-1; i += 3) {
                    float* p = &path->pts[i*2];
                    Editor::ECurveSegment curve;
                    curve.Start = { p[0] - halfWidth, p[1] - halfHeight, 0.0f };
                    curve.Controll1 = { p[2] - halfWidth, p[3] - halfHeight, 0.0f };
                    curve.Controll2 = { p[4] - halfWidth, p[5] - halfHeight, 0.0f };
                    curve.End = { p[6] - halfWidth, p[7] - halfHeight, 0.0f };
                    CurveSegments.push_back(curve);
                }
            }
        }
    }
    nsvgDelete(image);
    delete[] buffer;    
}
