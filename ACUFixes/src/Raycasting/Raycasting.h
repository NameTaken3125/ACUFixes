#pragma once

struct MyRaycastSuccessfulHit
{
    Vector3f m_HitLocation;
    Vector3f m_Normal;
    Vector3f m_EntityLocation;
    ACU::StrongRef<Entity> m_Entity;
};
namespace Raycasting
{
std::optional<MyRaycastSuccessfulHit> CastRayToMouseOnScreen(float range);
}
