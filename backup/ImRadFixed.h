//
// Created by yuang on 25-8-12.
//

#ifndef IMRADFIXED_H
#define IMRADFIXED_H

#include <imgui.h>
#include <imgui_internal.h>
#include <vector>

namespace ImRad {
	inline void Spacing(int n)
	{
		/*while (n--)
			ImGui::Spacing();*/
		/*ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;*/
		float sp = ImGui::GetStyle().ItemSpacing.y;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + n * sp);
	}

	inline bool Combo(const char* label, std::string* curr, const std::vector<std::string>& items, int flags = 0)
	{
		bool changed = false;
		if (ImGui::BeginCombo(label, curr->c_str(), flags))
		{
			for (const auto& item : items) {
				if (ImGui::Selectable(item.c_str(), item == *curr)) {
					*curr = item;
					changed = true;
				}
			}
			ImGui::EndCombo();
		}
		return changed;
	}

	inline bool Combo(const char* label, std::string* curr, const char* items, int flags = 0)
	{
		bool changed = false;
		if (ImGui::BeginCombo(label, curr->c_str(), flags))
		{
			const char* p = items;
			while (*p) {
				if (ImGui::Selectable(p, !curr->compare(p))) {
					*curr = p;
					changed = true;
				}
				p += strlen(p) + 1;
			}
			ImGui::EndCombo();
		}
		return changed;
	}

	inline void Dummy(const ImVec2& size)
	{
		//ImGui Dummy doesn't support negative dimensions like other controls
		ImVec2 sz = ImGui::CalcItemSize(size, 0, 0);
		return ImGui::Dummy(sz);
	}
}

#endif //IMRADFIXED_H
