#include "editor_extension.h"

Renderer::RLine* Editor::CreateRenderObject(const Editor::ELine& line)
{
    Renderer::RLine* result = new Renderer::RLine();
    result->SetStart(line.Start);
    result->SetEnd(line.End);
    return result;
}   

Renderer::RBezierCurveEdit* Editor::CreateRenderObject(const Editor::ECurveSegment& curve)
{
    Renderer::RBezierCurveEdit* result = new Renderer::RBezierCurveEdit();
    result->SetStartPos(curve.Start);
    result->SetEndPos(curve.End);
    result->SetControll1(curve.Controll1);
    result->SetControll2(curve.Controll2);
    return result;
}

Renderer::RBezierCurve* Editor::CreateRenderObject(const Editor::EBezierCurve& curveSegment)
{
    Renderer::RBezierCurve* result = new Renderer::RBezierCurve();
    EVector<Renderer::RBezierCurve::CurveSegment> segments;
    for (const ECurveSegment& seg : curveSegment.Segments)
    {
        Renderer::RBezierCurve::CurveSegment to_push;
        to_push.Start = seg.Start;
        to_push.End = seg.End;
        to_push.Controll1 = seg.Controll1;
        to_push.Controll2 = seg.Controll2;
        segments.push_back(to_push);
    }
    result->SetSegments(segments);
    return result;
}
