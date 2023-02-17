#pragma once

#include <vmath/vmath.h>

namespace ImGui3D
{

void DrawStuff();
// To be implemented by the user.
void CalculateViewProjectionForCurrentFrame(Matrix4f& viewProjOut);
void WhatIsActuallyDrawnForFrame();

}
