#pragma once

namespace Editor {


    E_STORAGE_STRUCT(ELine, 
        (EVec3, Start),
        (EVec3, End)
    )
    
    E_STORAGE_STRUCT(ECurveSegment, 
        (EVec3, Start, -1, 0, 0),
        (EVec3, End, 1, 0, 0),
        (EVec3, Controll1, -1, 1, 0),
        (EVec3, Controll2, 1, 1, 0)
    )

    E_STORAGE_STRUCT(EBezierCurve, 
        (EVector<ECurveSegment>, Segments)
    )

    E_EDEXAPI Renderer::RLine* CreateRenderObject(const ELine& line);
    E_EDEXAPI Renderer::RBezierCurveEdit* CreateRenderObject(const ECurveSegment& curveSegment);
    E_EDEXAPI Renderer::RBezierCurve* CreateRenderObject(const EBezierCurve& curveSegment);


}