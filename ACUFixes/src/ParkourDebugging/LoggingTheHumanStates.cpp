#include "pch.h"

#include "ParkourDebugging/LoggingTheHumanStates.h"
#include "ACU/HumanStatesHolder.h"

#include "ImGuiCTX.h"

std::string g_Indent;

class Indent
{
public:
    Indent()
    {
        g_Indent = g_Indent + "  ";
    }
    ~Indent()
    {
        g_Indent = g_Indent.substr(0, g_Indent.size() - 2);
    }
};

#define dprintf(fmt, ...) ImGui::LogText((g_Indent + fmt "\n").c_str(), __VA_ARGS__)
void print_node(FunctorBase* obj, const char* symbol)
{
    const bool hasOverride = obj->directChild_mb != nullptr;
    //uint64 value = (uint64)obj;
    uint64 value = (uint64)obj->Enter;
    dprintf("%s%s %llX", symbol, hasOverride ? "" : "*", value);
    //dprintf("%s", symbol);
}
/*
The player's Human State Machine is represented as a node tree.
Each node is a subclass of `FunctorBase`.
Each node is some "state".
It has an `Enter()` callback, called shortly after creation
(if the created node isn't discarded before that happens),
and an `Exit()` callback, called shortly before the node is discarded
(only if the node was `Enter()`ed first).
The `HumanStatesHolder` is the single root node.
Each node may or may not have a 0 or 1 direct/overriding child and 0 or more nonoverriding children.
It seems like the nonoverriding children are always created together with their parent
and are only destroyed just before the parent is.

Many of the player's mechanics, "states" interact with each other without directly referring to each other
by using "FancyVFunction queries", previously referred to as "one of those" functions.
Such "queries" gather a list of the most-overriding nodes in the node system,
check which ones have non-null callbacks for a specific FancyVFunction index,
then run those callbacks.

Below are a few examples of what the overall tree structure looks like in various situations.
Each line represents a single "state" node.
The hexadecimal numbers are the addresses of corresponding `Enter()` functions. It's the easiest way
to identify the type of a node (vtables aren't good enough, they can be shared by completely different states).
The symbols in the prefixes mean the following:
r - the root node == HumanStatesHolder
$ - overriding child (the node directly above is the overridden parent)
+ - nonoverriding child
* - the most-overriding nodes, the leaf nodes, the primary receivers of FancyVFunction queries.

When player is standing still on the ground, Low Profile:
1.  r 141B08500
2.    $ 141A98A50
3.      $ 141A8D220
4.        $ 141A7CBC0
5.          $ 141A7AE90
6.            $* 141A775C0
7.          + 141E6D4E0
8.            $* 141E6D4E0
9.          + 141E6D4E0
10.           $* 141E6D4E0
11.         + 141A7C980
12.           $* 141E6D4E0
13.     +* 1419E67F0
14.   + 141AD7E40
15.     $* 141AD5E60
16.   + 141B05D30
17.     $* 141AFF6A0
18.   + 141E6D4E0
19.     $* 1419D8860
20.   + 141E6D4E0
21.     $* 141E6D4E0
22.   + 141AD4460
23.     $* 141E6D4E0
24.   + 141E6D4E0
25.     $* 141E6D4E0
26.   + 141E6D4E0
27.     $* 141E6D4E0
28.   + 141E6D4E0
29.     $* 141E6D4E0
30.   + 141E6D4E0
31.     $* 141AE4AB0
32.   + 141AA68F0
33.     $* 141E6D4E0
34.   + 141988510
35.     $* 141988150
36.   + 141E6D4E0
37.     $* 141E6D4E0
38.   + 141E6D4E0
39.     $* 141990450
40.   + 141AFBC70
41.     $* 141E6D4E0
42.   +* 141A33880
43.   + 141E6D4E0
44.     $* 141AE62C0
45.   + 141E6D4E0
46.     $* 141E6D4E0
In this case there are 46 different "state nodes".
Of them 22 are the the leaf nodes, the "primary receivers" (marked with '*').

Below is an example of how the node tree changes, how the player transitions between various states.

Let's say we have a parent node `P` with an overriding child `A` and non-overriding children `N1`, `N2`
Let's say `A` has its own direct child `B` and non-overriding child `AN1`.
Let's say neither of `N1`, `N2`, `B`, `AN1` have any children at all.
The corresponding tree structure is the following:
r P
  $ A
    $* B
    +* AN1
  +* N1
  +* N2
Let's say that all of the above mentioned nodes have had `Enter()` called for them, they have been "activated".
Let's say something has happened that caused a state change in this tree, e.g. player has jumped off a building and is now airborne.
Let's say new node `X` with a cascade of direct children `X1`, `X2`, `X3`
will replace `A` as the direct child of `P`.
Let's say none of `X1`, `X2`, `X3` have any nonoverriding children. `X3` has no children at all.
The tree structure that will correspond to this is the following:
r P
  $ X
    $ X1
      $ X2
        $* X3
  +* N1
  +* N2
What happens during the transition from the previous state tree to this one:
- The nodes are _created_ in the following order: X, X1, X2, X3 (from the most shallow to the most nested)
- X is set as the pending direct child of P
- The nodes are _exited_ in the following order: B, AN1, A      (from the most nested to the most shallow)
- X is set as the direct child of P
- The nodes are _entered_ in the following order: X, X1, X2, X3 (from the most shallow to the most nested)

All `Enter()` functions are called at 1427555D2 (call RAX).
This is a good place to catch various player state transitions. You monitor the value of RAX here,
perform an action (e.g. a Quickshot) and see what new results you get.
This is the general approach to how the various changes in player mechanics
("tools everywhere", "nonsticky cover") were achieved:
- Find the appropriate state transitions (e.g. which `Enter()` functions trigger exactly when you start a Quickshot/when you leave cover?)
- Find the constructors of the state nodes that correspond to these `Enter()` functions (seems like there is exactly one LEA instruction that refers to each Enter() function)
- Find a way to transition to these state nodes under the desired conditions.


Some more examples of the player state tree in various situations:

Standing on the ground + crouching:
1.  r 141B08500
2.    $ 141A98A50
3.      $ 141A8D220
4.        $ 141A7CBC0
5.          $ 141A7AE90
6.            $* 141A775C0
7.          + 141E6D4E0
8.            $* 141E6D4E0
9.          + 141E6D4E0
10.           $* 141E6D4E0
11.         + 141A7C980
12.           $* 141E6D4E0
13.     +* 1419E67F0
14.   + 141AD7E40
15.     $* 141AD5E60
16.   + 141B05D30
17.     $* 141AFF6A0
18.   + 141E6D4E0
19.     $* 1419D8860
20.   + 141E6D4E0
21.     $* 141E6D4E0
22.   + 141AD4460
23.     $* 141E6D4E0
24.   + 141E6D4E0
25.     $* 141E6D4E0
26.   + 141E6D4E0
27.     $* 141E6D4E0
28.   + 141E6D4E0
29.     $* 141E6D4E0
30.   + 141E6D4E0
31.     $* 141AE4AB0
32.   + 141AA68F0
33.     $* 141E6D4E0
34.   + 141988510
35.     $* 141988150
36.   + 141E6D4E0
37.     $* 141E6D4E0
38.   + 141E6D4E0
39.     $* 141990450
40.   + 141AFBC70
41.     $* 141AFB7E0
42.   +* 141A33880
43.   + 141E6D4E0
44.     $* 141AE62C0
45.   + 141E6D4E0
46.     $* 141E6D4E0
Note that the only difference from the "standing on ground" example is the node 41.

Standing on the ground + phantom blade quickshot:
1.  r 141B08500
2.    $ 141A98A50
3.      $ 141A8D220
4.        $ 141A7CBC0
5.          $ 141A7AE90
6.            $* 141A775C0
7.          + 141E6D4E0
8.            $* 141E6D4E0
9.          + 141E6D4E0
10.           $* 141E6D4E0
11.         + 141A7C980
12.           $* 141E6D4E0
13.     +* 1419E67F0
14.   + 141AD7E40
15.     $* 141AD5E60
16.   + 141B05D30
17.     $* 141AFF6A0
18.   + 141E6D4E0
19.     $* 1419D8860
20.   + 141E6D4E0
21.     $* 141E6D4E0
22.   + 141AD4460
23.     $* 141E6D4E0
24.   + 141E6D4E0
25.     $* 141E6D4E0
26.   + 141E6D4E0
27.     $* 141E6D4E0
28.   + 141E6D4E0
29.     $* 141E6D4E0
30.   + 141E6D4E0
31.     $* 141AE4AB0
32.   + 141AA68F0
33.     $ 141AA6350
34.       $* 141AA4DD0
35.   + 141988510
36.     $* 141988150
37.   + 141E6D4E0
38.     $* 141E6D4E0
39.   + 141E6D4E0
40.     $* 141990450
41.   + 141AFBC70
42.     $* 141E6D4E0
43.   +* 141A33880
44.   + 141E6D4E0
45.     $* 141AE62C0
46.   + 141E6D4E0
47.     $* 141E6D4E0
Note that the only difference from just standing still is that
the node 33
is replaced with
a new node 33 with a child 34.

When just hanging on a wall:
1.  r 141B08500
2.    $ 141A98A50
3.      $* 141A4EC10
4.      +* 1419E67F0
5.    + 141AD7E40
6.      $* 141AD5E60
7.    + 141B05D30
8.      $* 141AFF6A0
9.    + 141E6D4E0
10.     $* 1419D8860
11.   + 141E6D4E0
12.     $* 141E6D4E0
13.   + 141AD4460
14.     $* 141E6D4E0
15.   + 141E6D4E0
16.     $* 141E6D4E0
17.   + 141E6D4E0
18.     $* 141E6D4E0
19.   + 141E6D4E0
20.     $* 141E6D4E0
21.   + 141E6D4E0
22.     $* 141AE4AB0
23.   + 141AA68F0
24.     $* 141E6D4E0
25.   + 141988510
26.     $* 141988150
27.   + 141E6D4E0
28.     $* 141E6D4E0
29.   + 141E6D4E0
30.     $* 141E6D4E0
31.   + 141AFBC70
32.     $* 141E6D4E0
33.   +* 141A33880
34.   + 141E6D4E0
35.     $* 141AE62C0
36.   + 141E6D4E0
37.     $* 141E6D4E0

During a safe backeject:
1.  r 141B08500
2.    $ 141A98A50
3.      $ 141A4EC10
4.        $* 141A4BC20
5.      +* 1419E67F0
6.    + 141AD7E40
7.      $* 141AD5E60
8.    + 141B05D30
9.      $* 141AFF6A0
10.   + 141E6D4E0
11.     $* 1419D8860
12.   + 141E6D4E0
13.     $* 141E6D4E0
14.   + 141AD4460
15.     $* 141E6D4E0
16.   + 141E6D4E0
17.     $* 141E6D4E0
18.   + 141E6D4E0
19.     $* 141E6D4E0
20.   + 141E6D4E0
21.     $* 141E6D4E0
22.   + 141E6D4E0
23.     $* 141AE4AB0
24.   + 141AA68F0
25.     $* 141E6D4E0
26.   + 141988510
27.     $* 141988150
28.   + 141E6D4E0
29.     $* 141E6D4E0
30.   + 141E6D4E0
31.     $* 141E6D4E0
32.   + 141AFBC70
33.     $* 141E6D4E0
34.   +* 141A33880
35.   + 141E6D4E0
36.     $* 141AE62C0
37.   + 141E6D4E0
38.     $* 141E6D4E0
*/
void PrintHumanStateNodeWithChildren(FunctorBase* obj, const char* symbol)
{
    if (!obj)
    {
        dprintf("NULL");
        return;
    }
    print_node(obj, symbol);
    Indent _ind;
    if (obj->directChild_mb)
    {
        PrintHumanStateNodeWithChildren(obj->directChild_mb, "$");
    }
    for (FunctorBase* child : obj->someNodes_B8)
    {
        PrintHumanStateNodeWithChildren(child, "?");
    }
    for (FunctorBase* child : obj->nonoverridingChildren)
    {
        PrintHumanStateNodeWithChildren(child, "+");
    }
}
void PlayerHumanStateTreeToClipboard()
{
    HumanStatesHolder* humanStates = HumanStatesHolder::GetForPlayer();
    ImGui::LogToClipboard();
    PrintHumanStateNodeWithChildren((FunctorBase*)humanStates, "r");
    ImGui::LogFinish();
}

#include "MyLog.h"
DEFINE_LOGGER_CONSOLE_AND_FILE(HumanStatesLog, "[HumanStates]");
class HumanStatesEnterLog
{
    std::vector<uint64> m_recordedEnterFunctions;
public:
    void LogIfNew(uint64 enterFunction)
    {
        for (uint64 fn : m_recordedEnterFunctions)
        {
            if (fn == enterFunction)
            {
                return;
            }
        }
        m_recordedEnterFunctions.push_back(enterFunction);
        LOG_DEBUG(HumanStatesLog
            , L"New state: %16llX (total %u)"
            , enterFunction
            , m_recordedEnterFunctions.size()
        );
    }
    void UpdateAndDrawLog()
    {
        if (ImGuiCTX::WindowChild frame{ "HumanStatesLogFrame", ImVec2(0, 0), true })
        {
            for (uint64 fn : m_recordedEnterFunctions)
            {
                ImGui::Text("%llX", fn);
            }
        }
    }
    void Reset()
    {
        m_recordedEnterFunctions.clear();
    }
};
HumanStatesEnterLog g_EnteredHumanStates;
#include "ACU/World.h"
class SavedHState
{
public:
    FunctorBase* m_RawPtr;
    void* m_FnEnter;
    void* m_FancyVTable;
    std::unique_ptr<SavedHState> m_OverridingChild;
    std::vector<std::unique_ptr<SavedHState>> m_NonoverridingChildren;

    float m_TimestampRecordedWhen;
    float m_ImguiCursorY; // When displaying the node, try to retain height if the nodes above it shrunk down.

    SavedHState(FunctorBase& hstate, float timestampRecordedWhen)
        : m_RawPtr(&hstate)
        , m_FnEnter(hstate.Enter)
        , m_FancyVTable(hstate.fancyVTable)
        , m_TimestampRecordedWhen(timestampRecordedWhen)
        , m_ImguiCursorY(0)
    {
        if (hstate.directChild_mb)
        {
            m_OverridingChild = std::make_unique<SavedHState>(*hstate.directChild_mb, timestampRecordedWhen);
        }
        for (FunctorBase* nonoverriding : hstate.nonoverridingChildren)
        {
            m_NonoverridingChildren.emplace_back(std::make_unique<SavedHState>(*nonoverriding, timestampRecordedWhen));
        }
    }
};
#define dprintf2(fmt, ...) ImGui::Text((g_Indent + fmt "\n").c_str(), __VA_ARGS__)
void print_node(SavedHState* obj, const char* symbol)
{
    const bool hasOverride = obj->m_OverridingChild != nullptr;
    dprintf2(
        "%s%s %llX (%16llX)"
        , symbol
        , hasOverride ? "" : "*"
        , obj->m_FnEnter
        , obj->m_RawPtr
    );
}
void PrintHumanStateNodeWithChildren(SavedHState* obj, const char* symbol)
{
    if (!obj)
    {
        dprintf2("NULL");
        return;
    }
    print_node(obj, symbol);
    Indent _ind;
    if (obj->m_OverridingChild)
    {
        PrintHumanStateNodeWithChildren(obj->m_OverridingChild.get(), "$");
    }
    for (std::unique_ptr<SavedHState>& child : obj->m_NonoverridingChildren)
    {
        PrintHumanStateNodeWithChildren(child.get(), "+");
    }
}
std::unique_ptr<SavedHState> g_HSLog_UpdateableRecord;
std::unique_ptr<SavedHState> g_HSLog_MostRecentSnapshot;
std::mutex g_HSLog_mutex;

void UpdateRecordOfHumanState(std::unique_ptr<SavedHState>& current, std::unique_ptr<SavedHState>&& newer)
{
    const bool isThisNodeChanged =
        !current
        || (current->m_RawPtr != newer->m_RawPtr)
        || (current->m_FnEnter != newer->m_FnEnter)
        ;
    if (isThisNodeChanged)
    {
        current = std::move(newer);
        return;
    }
    if (newer->m_OverridingChild)
    {
        UpdateRecordOfHumanState(current->m_OverridingChild, std::move(newer->m_OverridingChild));
    }
    for (size_t i = 0; i < current->m_NonoverridingChildren.size(); i++)
    {
        UpdateRecordOfHumanState(
            current->m_NonoverridingChildren[i],
            std::move(newer->m_NonoverridingChildren[i])
        );
    }
}
const std::string* FindHumanStateName(const SavedHState& hstate)
{
    static const std::unordered_map<uint64, std::string> humanStateName_by_fnEnter{
        {0x141B08500, "Root"},
        {0x141A98A50, "NotInCinematic"},
        {0x141B05D30, "EquipWeapon_Parent"},
        {0x141AFF6A0, "EquipWeapon_NotTransition"},
        {0x141B04C30, "EquipWeapon_PendingTransition"},
        {0x141B00D00, "EquipWeapon_SwordSheathed_LMBPressedButNotLongEnoughYet"},
        {0x141B03750, "EquipWeapon_Equipping"},
        {0x141B02AF0, "EquipWeapon_Sheathing"},
        //{0x1419D8860, "ChangesConstantly_idkWhat"},
        {0x141A7AE90, "OnGroundLowProfile"},
        {0x141A78CE0, "OnGroundLowProfile_AimGun"},
        {0x141A77D70, "OnGroundLowProfile_AimGun2"},
        {0x141A775C0, "OnGroundLowProfile_NotHitAWall"},
        {0x141A7A750, "OnGroundLowProfile_HitAWall"},
        {0x141A79930, "OnGroundLowProfile_AimBomb"},
        {0x141A798E0, "OnGroundLowProfile_AimBomb_Deploying"},
        {0x141A79580, "OnGroundLowProfile_AimBomb_Deployed"},
        {0x141A79800, "OnGroundLowProfile_AimBomb_Stopping"},
        {0x141A6A900, "OnGroundHighProfile"},
        {0x141A62BD0, "OnGroundHighProfileHitAWall"},
        {0x141A6A900, "OnGroundOverlookingEdgeHighProfile"},
        {0x141AA7D90, "DroppingBomb"},
        {0x141A8D220, "OnGroundLedgePeakVShapeSlopeHaystack_P"},
        {0x141A7CBC0, "OnGroundLedgePeakVShapeSlopeHaystack"},
        {0x141A60B80, "OnPeakOrLedge"},
        {0x141A5DC00, "OnPeakOrLedge_Peak"},
        {0x141A5ACB0, "OnPeakOrLedge_Ledge"},
        {0x141A59E60, "OnPeakOrLedge_Ledge_AimGun"},
        {0x141A59110, "OnPeakOrLedge_LandingOnLedge"},
        {0x141A76540, "InVShape"},
        {0x141A751D0, "InVShape_RecreatedOnTurn"},
        {0x141A706A0, "OnSlope"},
        {0x141A6DEA0, "OnSlope_FacingUp_P"},
        {0x141A6D3A0, "OnSlope_FacingUp"},
        {0x141A6FC80, "OnSlope_FacingDown_P"},
        {0x141A6EFB0, "OnSlope_FacingDown"},
        {0x1419E16B0, "InHaystack_P"},
        {0x1419DECC0, "InHaystack"},
        {0x141AE0EC0, "InSwingBackAndForth"},
        {0x141AE07F0, "DetachFromSwing"},
        {0x141ADD2D0, "InSafeJumpOrLoweringToHang"},
        {0x141A3BB20, "BreakfallToHang"},
        {0x141A93DF0, "Hanging"},
        {0x141AD8920, "HangToWallOrFeet"},
        {0x141A3DFD0, "InUnsafeJump"},
        {0x141A3BB20, "InBreakfall"},
        {0x141A89A30, "SteppingUpOrVault"},
        {0x141A770C0, "OverlookingEdge"},
        {0x141991F00, "UsingLift"},
        {0x141991270, "UsingLift_CuttingRope"},
        {0x141990DD0, "UsingLift_Rising"},
        {0x141A3D2D0, "UsingLift_Falling"},
        {0x141A4EC10, "Walling"},
        {0x141A4BC20, "DetachingFromWall"},
        {0x141A8CF40, "WallrunUp"},
        {0x141A69680, "GroundRoll_mb"},
        {0x141977A60, "TakeCover"},
        {0x14195A0F0, "TakeCover_TransitionToCover"},
        {0x14195C4F0, "TakeCover_InCover"},
        {0x14195C230, "TakeCover_InCover_LowProfile"},
        {0x14195B010, "TakeCover_InCover_HighProfile"},
        {0x14195A510, "TakeCover_LeavingCover"},
        {0x141AD7E40, "Disguise_Parent"},
        {0x141AD5E60, "Disguise_ReadyToActivate"},
        {0x141AD6920, "Disguise_Activating"},
        {0x141AD6260, "Disguise_Activated"},
        {0x141AD7BB0, "Disguise_BreakingDisguise"},
        {0x141AD5920, "Disguise_InactiveInCooldown"},
        {0x141990450, "WhenOnGround"},
        {0x141A4A4B0, "Assassination_PP"},
        {0x141A462F0, "Assassination_P"},
        {0x141A42F60, "Assassination_FirstHalf_mb"},
        {0x141A40BE0, "Assassination_SecondHalf_mb"},
        {0x1419BAA40, "Combat"},
        {0x1419AA430, "Combat_DoingNothing"},
        {0x1419ABA10, "Combat_BlockTooEarly"},
        {0x1419AB450, "Combat_Parry_P"},
        {0x1419AAD80, "Combat_Parrying"},
        {0x1419AAA90, "Combat_Parrying_PunchEnemy"},
        {0x1419A98D0, "Combat_Stabbed_P"},
        {0x141999D30, "Combat_Stabbed"},
        {0x1419B39D0, "Combat_ChargedShove_Charging"},
        {0x1419B3460, "Combat_ChargedShove_Shoving"},
        {0x1419AF160, "Combat_Shoot_P"},
        {0x1419AEA00, "Combat_Shoot"},
        {0x1419C8960, "CinematicAnimation_P"},
        {0x1419C8910, "CinematicAnimation"},
        {0x1419D4450, "Dead_mb_PP"},
        {0x1419D3820, "Dead_mb_P"},
        {0x1419D33D0, "Dead_mb"},
        {0x141A65C40, "LowProfilePushAgainstNPC"},
        {0x141A3A1E0, "LootingBody_Parent"},
        {0x141A39C00, "LootingBody_IsLooting"},
        {0x141A39EA0, "LootingBody_CancelingOrFinishing"},
    };
    static const std::unordered_map<uint64, std::string> humanStateName_by_fancyVTable{
        {0x144BAE250, "Swimming_PP"},
        {0x144CD7290, "Swimming_P"},
        {0x144B6A840, "Swimming"},
        {0x144835690, "AimGun_Parent"},
        {0x144813DD0, "AimGun_NotAiming"},
        {0x1448280B0, "AimGun_AimingP"},
        {0x14496B420, "AimGun_Aiming"},
        {0x144D4FE70, "Crouch_Parent"},
        {0x144DC1C70, "Crouch_Crouching"},
        {0x144DBB190, "Crouch_NotCrouching"},
        {0x1449860E0, "LootingChestOrPickingLock_P"},
        {0x144A2E4B0, "LootingChestOrPickingLock"},
        {0x144935680, "LootingChestOrPickingLock_PickingLock"},
        {0x144AB5080, "LootingChest_PickingLock_ReplacingBrokenLockpick"},
        {0x1449504E0, "LootingChest_PickingLockFinished_OpeningLock"},
        {0x144AC2B20, "LootingChest_PickingLockFinished_OpeningChest"},
        {0x144565B40, "HidespotCloset_PP"},
        {0x14425ECC0, "HidespotCloset_Entering_P"},
        {0x14473C800, "HidespotCloset_Entering"},
        {0x144222720, "HidespotCloset_Entered"},
        {0x144494980, "HidespotCloset_Leaving"},
        {0x1448A8500, "Assassination_CancelingChokeBeforeStarted"},
        {0x144AAE5A0, "Assassination_CancelingWhileChokingEnemy"},
        {0x14413E5C0, "ScriptAnimationLikeCutAlarmBell"},
        {0x144379CA0, "MovingToCorrectPositionForScriptAnimationLikeCoverChimneys"},
        {0x144978A40, "GameLoading_P"},
        {0x144821550, "GameLoading"},
        {0x144108B80, "TakeCover_Aim_P"},
        {0x144500B40, "TakeCover_Aim"},
        {0x1444CADD0, "TakeCover_AimWithoutWeapon"},
        {0x144373090, "TakeCover_AimWithWeapon_P"},
        {0x144207940, "TakeCover_AimWithWeapon_Unholstering"},
        {0x144793EC0, "TakeCover_AimWithWeapon_WeaponOutAiming"},
        {0x1445DE960, "TakeCover_AimWithWeapon_Reholstering"},
    };
    auto foundIt = humanStateName_by_fnEnter.find((uint64)hstate.m_FnEnter);
    const std::string* result = (foundIt == humanStateName_by_fnEnter.end()) ? nullptr : &foundIt->second;
    if (!result)
    {
        foundIt = humanStateName_by_fancyVTable.find((uint64)hstate.m_FancyVTable);
        result = (foundIt == humanStateName_by_fancyVTable.end()) ? nullptr : &foundIt->second;
    }
    return result;
}
class HSLogDrawer
{
public:
    void DrawHumanStateWithAnimation(SavedHState& hstate, const char* symbol, float currentTimestamp)
    {
        const bool hasOverride = hstate.m_OverridingChild != nullptr;

        float timeElapsed = currentTimestamp - hstate.m_TimestampRecordedWhen;
        const float fadeoutDuration = 2.0f;
        float fadeFactor = timeElapsed / fadeoutDuration;
        if (fadeFactor > 1.0f)      fadeFactor = 1.0f;
        else if (fadeFactor < 0.0f) fadeFactor = 0.0f;
        const ImVec4 colorFresh = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        const ImVec4 colorStale = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        const float fadeFactorInv = 1 - fadeFactor;
        const ImVec4 color{
            colorStale.x * fadeFactor + colorFresh.x * fadeFactorInv,
            colorStale.y * fadeFactor + colorFresh.y * fadeFactorInv,
            colorStale.z * fadeFactor + colorFresh.z * fadeFactorInv,
            colorStale.w * fadeFactor + colorFresh.w * fadeFactorInv,
        };
        const ImVec4 readableNameColor = ImVec4(1.0f, 0.8f, 0.3f, 1.0f);

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        const std::string* foundName = FindHumanStateName(hstate);
        //DrawTextWithRetainedHeight(hstate,
        //    (g_Indent + "%s%s %llX (%s%16llX)").c_str()
        //    , symbol
        //    , hasOverride ? "" : "*"
        //    , hstate.m_FnEnter
        //    , foundName ? foundName->c_str() : ""
        //    , hstate.m_RawPtr
        //);
        float cursorY = std::max(hstate.m_ImguiCursorY, ImGui::GetCursorPosY());
        hstate.m_ImguiCursorY = cursorY;
        ImGui::SetCursorPosY(cursorY);
        ImGui::Text(
            (g_Indent + "%s%s %llX/%llX (").c_str()
            , symbol
            , hasOverride ? "" : "*"
            , hstate.m_FnEnter
            , hstate.m_FancyVTable
        );
        if (foundName)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, readableNameColor);
            ImGui::SameLine();
            ImGui::Text(foundName->c_str());
            ImGui::PopStyleColor();
        }
        ImGui::SameLine();
        ImGui::Text(
            " %llX)"
            , hstate.m_RawPtr
        );
        ImGui::PopStyleColor();

        Indent _ind;
        if (hstate.m_OverridingChild)
        {
            DrawHumanStateWithAnimation(*hstate.m_OverridingChild, "$", currentTimestamp);
        }
        for (auto& child : hstate.m_NonoverridingChildren)
        {
            DrawHumanStateWithAnimation(*child, "+", currentTimestamp);
        }
    }
private:
    void DrawTextWithRetainedHeight(SavedHState& hstate, const char* fmt, ...)
    {
        float cursorY = std::max(hstate.m_ImguiCursorY, ImGui::GetCursorPosY());
        hstate.m_ImguiCursorY = cursorY;
        ImGui::SetCursorPosY(cursorY);
        va_list args;
        va_start(args, fmt);
        ImGui::TextV(fmt, args);
        va_end(args);
    }
};
void DrawPlayerHumanStates(bool doCopyToClipboard)
{
    std::lock_guard<std::mutex> _lock{ g_HSLog_mutex };
    if (!g_HSLog_UpdateableRecord)
    {
        g_HSLog_UpdateableRecord = std::move(g_HSLog_MostRecentSnapshot);
    }
    if (!g_HSLog_UpdateableRecord) return;
    if (g_HSLog_MostRecentSnapshot)
    {
        UpdateRecordOfHumanState(g_HSLog_UpdateableRecord, std::move(g_HSLog_MostRecentSnapshot));
    }
    World* world = World::GetSingleton();
    float currentTimestamp = world ? world->clockInWorldWithSlowmotion.GetCurrentTimeFloat() : 0;
    if (doCopyToClipboard)
        ImGui::LogToClipboard();
    HSLogDrawer().DrawHumanStateWithAnimation(*g_HSLog_UpdateableRecord, "r", currentTimestamp);
    if (doCopyToClipboard)
        ImGui::LogFinish();
}
bool g_DoDrawHumanStatesLog = false;
void ShowHumanStatesLogIfNeeded()
{
    if (!g_DoDrawHumanStatesLog) return;
    if (ImGuiCTX::Window _wnd{ "HumanStates", &g_DoDrawHumanStatesLog })
    {
        bool doCopyToClipboard = ImGui::Button("Dump current states tree to clipboard");
        ImGui::SameLine();
        if (ImGui::Button("Clear recorded states"))
        {
            g_EnteredHumanStates.Reset();
        }
        ImGui::Separator();
        DrawPlayerHumanStates(doCopyToClipboard);
    }
}
void DrawHumanStatesDebugControls()
{
    ImGui::Checkbox("Show the log of player's \"HumanStates\"", &g_DoDrawHumanStatesLog);
}

void WhenAboutToEnterHumanState_Log(AllRegisters* params)
{
    uint64 fnp_Enter = params->GetRAX();
    FunctorBase* stateToBeEntered = (FunctorBase*)params->rcx_;
    FunctorBase* topmostParent = stateToBeEntered->parentStack[stateToBeEntered->parentStack.size - 1];
    auto* playersHumanState = (FunctorBase*)HumanStatesHolder::GetForPlayer();
    const bool isPlayerState = topmostParent == playersHumanState;
    if (!isPlayerState)
    {
        return;
    }
    g_EnteredHumanStates.LogIfNew((uint64)fnp_Enter);
    std::lock_guard<std::mutex> _lock{ g_HSLog_mutex };
    World* world = World::GetSingleton();
    float timestampRecordedWhen = world ? world->clockInWorldWithSlowmotion.GetCurrentTimeFloat() : 0;
    g_HSLog_MostRecentSnapshot = std::make_unique<SavedHState>(*playersHumanState, timestampRecordedWhen);
}
EnterHumanStateHook::EnterHumanStateHook()
{
    uintptr_t whenNewHumanStateIsEntered = 0x1427555D2;
    PresetScript_CCodeInTheMiddle(whenNewHumanStateIsEntered, 7,
        WhenAboutToEnterHumanState_Log, RETURN_TO_RIGHT_AFTER_STOLEN_BYTES, true);
}
