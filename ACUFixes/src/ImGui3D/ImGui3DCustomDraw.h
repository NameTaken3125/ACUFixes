#pragma once

namespace ImGui3D::CustomDraw
{
class CustomDrawer
{
public:
    virtual void DoDraw() = 0;
};
void CustomDraw_Subscribe(CustomDrawer& subscriber);
void CustomDraw_Unsubscribe(CustomDrawer& subscriber);

void DrawAllCustom();
}
