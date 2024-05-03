#include "pch.h"

#include "Animation/AnimationGraphEvaluationPatches.h"
#include "Input/Hack_GameRawInputHook.h"

class VariousHooksContainer
{
public:
    AutoAssembleWrapper<GameRawInputHook> m_GameRawInputHook;
    AutoAssembleWrapper<AnimationGraphEvaluationPatches> m_AnimationGraphEvaluationPatches;
};
std::optional<VariousHooksContainer> g_VariousHooks;
void PluginLoader_VariousHooks_Start()
{
    g_VariousHooks.emplace();
    g_VariousHooks->m_GameRawInputHook.Activate();
    g_VariousHooks->m_AnimationGraphEvaluationPatches.Activate();
}
