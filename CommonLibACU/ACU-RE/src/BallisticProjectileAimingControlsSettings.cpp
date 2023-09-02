#include "pch.h"

#include "ACU/BallisticProjectileAimingControlsSettings.h"
#include "ACU/GamePlaySettings.h"

BallisticProjectileAimingControlsSettings& BallisticProjectileAimingControlsSettings::GetSingleton()
{
    return GamePlaySettings::GetSingleton()->ballisticProjectileAimingControlsSettings;
}
