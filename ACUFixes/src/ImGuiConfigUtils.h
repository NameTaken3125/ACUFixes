#pragma once

namespace ImGui {
template<typename EnumType>
bool DrawEnumPicker(const char* label, EnumType& currentValueInOut, ImGuiComboFlags flags)
{
    bool isNewSelection = false;
    auto itemsStrings = enum_reflection<EnumType>::GetAllStrings();
    auto itemsValues = enum_reflection<EnumType>::GetAllValues();
    auto it = std::find(itemsValues.begin(), itemsValues.end(), currentValueInOut);
    const bool isInvalidValue = it == itemsValues.end();

    const char* strCurrentValue = nullptr;
    int item_current_idx{ -1 };
    std::string strInvalidValue;
    if (!isInvalidValue)
    {
        item_current_idx = (int)(it - itemsValues.begin());                    // Here our selection data is an index.
    }
    else
    {
        std::stringstream ss;
        ss << "INVALID VALUE: 0x" << std::hex << (unsigned long long)currentValueInOut;
        strInvalidValue = ss.str();
    }
    const char* preview_value = isInvalidValue ? strInvalidValue.c_str() : itemsStrings[item_current_idx];

    if (ImGui::BeginCombo(label, preview_value, flags))
    {
        if (isInvalidValue)
            ImGui::Selectable(strInvalidValue.c_str(), true);
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
// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
} // namespace ImGui
