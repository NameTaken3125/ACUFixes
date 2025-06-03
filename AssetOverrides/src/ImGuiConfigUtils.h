#pragma once

namespace ImGui {
template<typename EnumType>
bool DrawEnumPicker(const char* label, EnumType& currentValueInOut, ImGuiComboFlags flags)
{
    bool isNewSelection = false;
    auto itemsStrings = enum_reflection<EnumType>::GetAllStrings();
    auto itemsValues = enum_reflection<EnumType>::GetAllValues();
    auto it = std::find(itemsValues.begin(), itemsValues.end(), currentValueInOut);
    int item_current_idx = (int)(it - itemsValues.begin());                    // Here our selection data is an index.
    if (ImGui::BeginCombo(label, itemsStrings[item_current_idx], flags))
    {
        for (int n = 0; n < itemsStrings.size(); n++)
        {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(itemsStrings[n], is_selected)) {
                item_current_idx = n;
                currentValueInOut = itemsValues[n];
                isNewSelection = true;
            }
        }
        ImGui::EndCombo();
    }
    return isNewSelection;
}
template<class Hack>
void DrawCheckboxForHack(Hack& hack, const std::string_view& text)
{
    if (auto* instance = &hack)
    {
        bool isActive = instance->IsActive();
        if (ImGui::Checkbox(text.data(), &isActive))
        {
            instance->Toggle();
        }
    }
}
} // namespace ImGui
