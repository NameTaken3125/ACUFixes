#include "pch.h"

#include "vmath/vmath.h"
#include "ACU/basic_types.h"
#include "ACU/SmallArray.h"
#include "ACU/SharedPtr.h"
#include "ACU/Entity.h"
#include "ACU/ACUPlayerCameraComponent.h"
#include "ACU/World.h"
#include "ACU/RenderValuesHolder.h"
#include "ACU/Letterbox.h"
#include "ACU_DefineNativeFunction.h"
#include "ACU/ManagedPtrs/ManagedPtrs.h"

#include "ImGui3D/ImGui3D.h"
#include "ImGuiCTX.h"

#include "Raycasting.h"

class PhysicComponent;
class RaycastResult
{
public:
    Vector4f hitLocation; //0x0000
    Vector4f targetNormal; //0x0010
    PhysicComponent* physicCpnt; //0x0020
    SharedPtrNew<Entity>* shared_targetEntity; //0x0028
    uint32 entityId_mb; //0x0030
    char pad_0034[4]; //0x0034
    float distance_mb; //0x0038
    char pad_003C[12]; //0x003C
    uint32 dword_48; //0x0048
    char pad_004C[20]; //0x004C
}; //Size: 0x0060
assert_sizeof(RaycastResult, 0x60);
struct RaycastStruct
{
    char pad_0[8];
    SmallArray<RaycastResult> raycastResults;
    char pad_14[0x620 - 0x14];
    SmallArray<SharedPtrNew<Entity>*> excludedEntities_mb;
    char pad_62C[0x650 - 0x62C];
    uint64 raycastFlags;
    char pad_658[8];
    int16 word_660;
    char pad_662[0x680 - 0x662];
};
assert_offsetof(RaycastStruct, raycastResults, 8);
assert_offsetof(RaycastStruct, word_660, 0x660);
assert_offsetof(RaycastStruct, excludedEntities_mb, 0x620);
assert_sizeof(RaycastStruct, 0x680);
DEFINE_GAME_FUNCTION(RaycastStruct__ctor, 0x1425EC5A0, RaycastStruct*, __fastcall, (RaycastStruct* this_));
class World;
DEFINE_GAME_FUNCTION(g_RaycastStruct__CastRay_impl, 0x1425FD7D0, char, __fastcall, (RaycastStruct* this_, World* world, __m128* p_origin, __m128* p_direction, float p_rayLength, unsigned __int8 p_1forGunshot, char p_0forGunshot))
DEFINE_GAME_FUNCTION(ArrRaycastResults__SortByDistance, 0x1425DF220, void, __fastcall, (SmallArray<RaycastResult>* arr));
DEFINE_GAME_FUNCTION(ArrRaycastResults__Destroy, 0x140010340, void, __fastcall, (SmallArray<RaycastResult>* arr));
void CalculateViewportForCurrentFrame(Vector2f& topLeftOut, Vector2f& bottomRightOut);
namespace Raycasting
{
inline Vector2f UVToClipSpace(Vector2f uv) {
    //TL == (0, 0) -> (-1, 1);
    //TR == (1, 0) -> (1, 1);
    //BL == (0, 1) -> (-1, -1);
    //BR == (1, 1) -> (1, -1);

    Vector2f xy = uv * 2 - 1;
    xy.y = -xy.y;
    return xy;
}
Matrix4f& GetGameMatrix_InverseProjection()
{
    return RenderValuesHolder::GetSingleton()->matInvProjection;
}
Matrix4f GetGameMatrix_View()
{
    Matrix4f cameraMat = RenderValuesHolder::GetSingleton()->cameraTransform;
    cameraMat = cameraMat * Matrix4f::createRotationAroundAxis(-90, 0, 0);
    return cameraMat.inverse();
}
Matrix3f GetGameMatrix_InverseViewRotation()
{
    Matrix4f view = GetGameMatrix_View();
    return view.getRotation().inverse();
}
struct Viewport
{
    Vector2f m_TopLeft;
    Vector2f m_BottomRight;

    std::optional<Vector2f> PixelsToUVIfWithinViewport(Vector2f pixels)
    {
        if (pixels.x < m_TopLeft.x
            || pixels.y < m_TopLeft.y
            || pixels.x > m_BottomRight.x
            || pixels.y > m_BottomRight.y)
            return {};
        const Vector2f viewportSize = (m_BottomRight - m_TopLeft);
        return (pixels - m_TopLeft) / viewportSize;
    }
};
Vector3f DirectionFromCameraToPointOnScreen(Vector2f screenUV)
{
    float _z = .5f;
    Matrix4f inverseProjectionMatrix = GetGameMatrix_InverseProjection();

    Vector2f clipSpacePos = UVToClipSpace(screenUV);
    Vector4f projectedPos = Vector4f(clipSpacePos.x, clipSpacePos.y, _z, 1);
    Vector4f viewSpacePos = inverseProjectionMatrix * projectedPos;
    viewSpacePos /= viewSpacePos.w;
    Vector3f worldSpaceDirection = GetGameMatrix_InverseViewRotation() * (Vector3f&)viewSpacePos;
    return worldSpaceDirection.normalized();
}
std::optional<Vector3f> CalculateWorldDirectionFromCameraToMousePosition()
{
    // ImGui::GetMousePos() returns (0, 0) for top left, (width - 1, height - 1) for bottom right
    // corners _of_the_window_. So it can return (-100, 500) if game is windowed
    // and mouse is to the left of the window.
    Vector2f mousePosInWindowPixels = (Vector2f&)ImGui::GetMousePos();
    Viewport viewport;
    CalculateViewportForCurrentFrame(viewport.m_TopLeft, viewport.m_BottomRight);
    std::optional<Vector2f> mousePosInViewportUV = viewport.PixelsToUVIfWithinViewport(mousePosInWindowPixels);
    if (!mousePosInViewportUV) return {};
    Vector3f rayDir = DirectionFromCameraToPointOnScreen(*mousePosInViewportUV);
    return rayDir;
}
// Max range is some 30 meters, probably.
void RaycastStruct_CastRay(RaycastStruct& nativeRaycast, World& world, Vector3f rayOrigin, Vector3f direction, float range)
{
    // (Partially) recreating part of the setup in `1405790B0 == FirearmComponent_80::Raycast()`
    RaycastStruct__ctor(&nativeRaycast);
    constexpr uint64 raycastFlags_usedForGunshot = 0x1a00805247;
    constexpr uint64 raycastFlags_everythingMaybe = uint64(-1);
    nativeRaycast.raycastFlags = raycastFlags_everythingMaybe;

    __m128 rayOriginAlign{ 0 };
    __m128 directionAlign{ 0 };
    (Vector3f&)rayOriginAlign.m128_f32 = rayOrigin;
    (Vector3f&)directionAlign.m128_f32 = direction;
    g_RaycastStruct__CastRay_impl(&nativeRaycast, &world, &rayOriginAlign, &directionAlign, range, 1, 0);
    if (nativeRaycast.raycastResults.size)
    {
        ArrRaycastResults__SortByDistance(&nativeRaycast.raycastResults);
    }
}
// Max range is some 30 meters, probably.
std::optional<MyRaycastSuccessfulHit> CastRayToMouseOnScreen(float range)
{
    auto* world = World::GetSingleton();
    if (!world) return {};
    auto* cam = ACUPlayerCameraComponent::GetSingleton();
    if (!cam) return {};
    std::optional<Vector3f> mouseDirection = Raycasting::CalculateWorldDirectionFromCameraToMousePosition();
    if (!mouseDirection) return {};

    Vector3f direction = *mouseDirection;
    Vector3f& rayOrigin = (Vector3f&)cam->positionLookFrom;

    RaycastStruct nativeRaycast;
    RaycastStruct__ctor(&nativeRaycast);
    RaycastStruct_CastRay(nativeRaycast, *world, rayOrigin, direction, range);

    std::optional<MyRaycastSuccessfulHit> result;
    if (nativeRaycast.raycastResults.size)
    {
        RaycastResult& hit = nativeRaycast.raycastResults[0];
        result = MyRaycastSuccessfulHit{
            (Vector3f&)hit.hitLocation
            , hit.shared_targetEntity->GetPtr()->GetPosition()
            , ACU::StrongRef<Entity>(*hit.shared_targetEntity)
        };
    }
    ArrRaycastResults__Destroy(&nativeRaycast.raycastResults);
    return result;
}
} // namespace Raycasting
