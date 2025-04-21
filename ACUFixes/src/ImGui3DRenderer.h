#pragma once

#include <vmath/vmath.h>
#include "ImGui3D.h"

namespace ImGui3D
{

// Call once per frame during ImGui drawing.
void Draw3DLayer(const World2ScreenParams& w2sParams);

}
