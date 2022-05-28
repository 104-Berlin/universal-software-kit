#pragma once

namespace Editor {


    E_STORAGE_STRUCT(ELine, 
        (EVec3, Start),
        (EVec3, End)
    )
    
    E_STORAGE_STRUCT(ECurveSegment, 
        (EVec3, Start, -10, 0, 0),
        (EVec3, End, 10, 0, 0),
        (EVec3, Controll1, -10, 10, 0),
        (EVec3, Controll2, 10, 10, 0)
    )

}