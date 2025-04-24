#include "pch.h"

#include "ImGuiConsole.h"
#include "PluginLoaderConfig.h"

ConsoleMode g_ConsoleMode = ConsoleMode::Hidden;

void ToggleConsoleMode()
{
	switch (g_ConsoleMode)
	{
	case ConsoleMode::Hidden:
		g_ConsoleMode = ConsoleMode::ForegroundAndFocusable;
		break;
	case ConsoleMode::ForegroundAndFocusable:
		g_ConsoleMode = ConsoleMode::BackgroundSemitransparentAndUnfocusable;
		break;
	case ConsoleMode::BackgroundSemitransparentAndUnfocusable:
		g_ConsoleMode = ConsoleMode::Hidden;
		break;
	default:
		g_ConsoleMode = ConsoleMode::ForegroundAndFocusable;
		break;
	}
}

ImGuiConsole::ImGuiConsole()
{
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;

	// "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
	Commands.push_back("HELP");
	Commands.push_back("HISTORY");
	Commands.push_back("CLEAR");
	Commands.push_back("CLASSIFY");
	AutoScroll = true;
	ScrollToBottom = false;

	m_SingleLineToCopy.reserve(1024);
}
ImGuiConsole::~ImGuiConsole()
{
	ClearLog();
	for (int i = 0; i < History.Size; i++)
		free(History[i]);
}
void ImGuiConsole::ClearLog()
{
	std::lock_guard _lock{ m_Mutex };
	for (int i = 0; i < Items.Size; i++)
		free(Items[i]);
	Items.clear();
}
void ImGuiConsole::ExecCommand(const char* command_line)
{
	AddLogF("# %s\n", command_line);

	// Insert into history. First find match and delete it so it can be pushed to the back.
	// This isn't trying to be smart or optimal.
	HistoryPos = -1;
	for (int i = History.Size - 1; i >= 0; i--)
		if (Stricmp(History[i], command_line) == 0)
		{
			free(History[i]);
			History.erase(History.begin() + i);
			break;
		}
	History.push_back(Strdup(command_line));

	// Process command
	if (Stricmp(command_line, "CLEAR") == 0)
	{
		ClearLog();
	}
	else if (Stricmp(command_line, "HELP") == 0)
	{
		AddLogF("Commands:");
		for (int i = 0; i < Commands.Size; i++)
			AddLogF("- %s", Commands[i]);
	}
	else if (Stricmp(command_line, "HISTORY") == 0)
	{
		int first = History.Size - 10;
		for (int i = first > 0 ? first : 0; i < History.Size; i++)
			AddLogF("%3d: %s\n", i, History[i]);
	}
	else
	{
		AddLogF("Unknown command: '%s'\n", command_line);
	}

	// On command input, we scroll to bottom even if AutoScroll==false
	ScrollToBottom = true;
}
int ImGuiConsole::TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
	ImGuiConsole* console = (ImGuiConsole*)data->UserData;
	return console->TextEditCallback(data);
}
int ImGuiConsole::TextEditCallback(ImGuiInputTextCallbackData* data)
{
	//AddLogF("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackCompletion:
	{
		// Example of TEXT COMPLETION

		// Locate beginning of current word
		const char* word_end = data->Buf + data->CursorPos;
		const char* word_start = word_end;
		while (word_start > data->Buf)
		{
			const char c = word_start[-1];
			if (c == ' ' || c == '\t' || c == ',' || c == ';')
				break;
			word_start--;
		}

		// Build a list of candidates
		ImVector<const char*> candidates;
		for (int i = 0; i < Commands.Size; i++)
			if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
				candidates.push_back(Commands[i]);

		if (candidates.Size == 0)
		{
			// No match
			AddLogF("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
		}
		else if (candidates.Size == 1)
		{
			// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
			data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
			data->InsertChars(data->CursorPos, candidates[0]);
			data->InsertChars(data->CursorPos, " ");
		}
		else
		{
			// Multiple matches. Complete as much as we can..
			// So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
			int match_len = (int)(word_end - word_start);
			for (;;)
			{
				int c = 0;
				bool all_candidates_matches = true;
				for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
					if (i == 0)
						c = toupper(candidates[i][match_len]);
					else if (c == 0 || c != toupper(candidates[i][match_len]))
						all_candidates_matches = false;
				if (!all_candidates_matches)
					break;
				match_len++;
			}

			if (match_len > 0)
			{
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
			}

			// List matches
			AddLogF("Possible matches:\n");
			for (int i = 0; i < candidates.Size; i++)
				AddLogF("- %s\n", candidates[i]);
		}

		break;
	}
	case ImGuiInputTextFlags_CallbackHistory:
	{
		// Example of HISTORY
		const int prev_history_pos = HistoryPos;
		if (data->EventKey == ImGuiKey_UpArrow)
		{
			if (HistoryPos == -1)
				HistoryPos = History.Size - 1;
			else if (HistoryPos > 0)
				HistoryPos--;
		}
		else if (data->EventKey == ImGuiKey_DownArrow)
		{
			if (HistoryPos != -1)
				if (++HistoryPos >= History.Size)
					HistoryPos = -1;
		}

		// A better implementation would preserve the data on the current input line along with cursor position.
		if (prev_history_pos != HistoryPos)
		{
			const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
			data->DeleteChars(0, data->BufTextLen);
			data->InsertChars(0, history_str);
		}
	}
	}
	return 0;
}

void ImGuiConsole::DrawIfVisible(const char* title, ConsoleMode consoleMode)
{
	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_MenuBar;
	ImGuiWindowFlags scrolling_region_flags =
		ImGuiWindowFlags_None;

	float window_bgAlpha = 1.0f;
	bool showFooter = false;
	bool showMenuBar = true;
	switch (consoleMode)
	{
	case ConsoleMode::Hidden:
		return;
	case ConsoleMode::BackgroundSemitransparentAndUnfocusable:
	{
		window_bgAlpha = 0.35f;
		window_flags = window_flags
			| ImGuiWindowFlags_NoInputs
			| ImGuiWindowFlags_NoNav
			;
		scrolling_region_flags = scrolling_region_flags
			| ImGuiWindowFlags_NoInputs
			| ImGuiWindowFlags_NoNav
			;
		showFooter = false;
		showMenuBar = false;
		break;
	}
	case ConsoleMode::ForegroundAndFocusable:
	{
		window_bgAlpha = 1.0f;
		scrolling_region_flags = scrolling_region_flags
			| ImGuiWindowFlags_HorizontalScrollbar
			;
		showFooter = true;
		showMenuBar = true;
		break;
	}
	default:
		return;
	}

	float window_height = 450;
	float scrollingRegionHeight = 0.0f;
	ImGuiStyle& style = ImGui::GetStyle();
	const bool showFooterCommandInput = false;
	if (showFooter)
	{
		float footer_height_to_reserve = (0
			+ style.ItemSpacing.y                               // Separator
			+ ImGui::GetFontSize() + style.ItemSpacing.y        // Row of SmallButtons
			+ style.ItemSpacing.y                               // Separator
			+ ImGui::GetFrameHeightWithSpacing()                // Filter box
			);
		if (showFooterCommandInput)
			footer_height_to_reserve = (footer_height_to_reserve
				+ style.ItemSpacing.y                           // Separator
				+ ImGui::GetFrameHeightWithSpacing()            // Text input frame (for the console input)
				);
		window_height += footer_height_to_reserve;
		scrollingRegionHeight = -footer_height_to_reserve;
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, window_height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowBgAlpha(window_bgAlpha);

	static bool doSpamInConsole = false;
	static float t = 0.0f; if (doSpamInConsole && ImGui::GetTime() - t > 0.02f) { t = (float)ImGui::GetTime(); AddLogF("Spam %f", t); }
	if (!ImGui::Begin(title, nullptr, window_flags))
	{
		ImGui::End();
		return;
	}
	if (ImGui::BeginMenuBar())
	{
		if (showMenuBar)
		{
			ImGui::Checkbox("Autoscroll", &AutoScroll);
			ImGui::Separator();
			ImGui::Checkbox("Spam", &doSpamInConsole);
			ImGui::Separator();
		}
		static ImGuiTextBuffer textBuf_ToggleConsole;
		textBuf_ToggleConsole.clear();
		textBuf_ToggleConsole.appendf(
			"Press %s to toggle console"
			, enum_reflection<VirtualKeys>::GetString(g_PluginLoaderConfig.hotkey_ToggleConsole)
		);
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(textBuf_ToggleConsole.c_str()).x - style.ItemSpacing.x * 1);
		ImGui::Text(textBuf_ToggleConsole.c_str());
		ImGui::EndMenuBar();
	}

	// Options menu
	if (ImGui::BeginPopup("Options"))
	{
		ImGui::Checkbox("Auto-scroll", &AutoScroll);
		ImGui::EndPopup();
	}

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, scrollingRegionHeight), false, scrolling_region_flags);
	bool copy_to_clipboard = false;
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::Selectable("Copy line"))
		{
			ImGui::SetClipboardText(m_SingleLineToCopy.c_str());
		}
		if (ImGui::Selectable("Copy all")) copy_to_clipboard = true;
		if (ImGui::Selectable("Clear")) ClearLog();
		ImGui::EndPopup();
	}


	// Display every line as a separate entry so we can change their color or add custom widgets.
	// If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
	// NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
	// to only process visible items. The clipper will automatically measure the height of your first item and then
	// "seek" to display only items in the visible area.
	// To use the clipper we can replace your standard loop:
	//      for (int i = 0; i < Items.Size; i++)
	//   With:
	//      ImGuiListClipper clipper;
	//      clipper.Begin(Items.Size);
	//      while (clipper.Step())
	//         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
	// - That your items are evenly spaced (same height)
	// - That you have cheap random access to your elements (you can access them given their index,
	//   without processing all the ones before)
	// You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
	// We would need random-access on the post-filtered list.
	// A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
	// or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
	// and appending newly elements as they are inserted. This is left as a task to the user until we can manage
	// to improve this example code!
	// If your items are of variable height:
	// - Split them into same height items would be simpler and facilitate random-seeking into your list.
	// - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	if (copy_to_clipboard)
		ImGui::LogToClipboard();
	{
		std::lock_guard _lock{ m_Mutex };
		for (int i = 0; i < Items.Size; i++)
		{
			const char* item = Items[i];
			if (!Filter.PassFilter(item))
				continue;

			// Normally you would store more information in your item than just a string.
			// (e.g. make Items[] an array of structure, store color/type etc.)
			ImVec4 color;
			bool has_color = false;
			if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
			else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
			if (has_color)
				ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted(item);
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				m_SingleLineToCopy = item;
			}
			if (has_color)
				ImGui::PopStyleColor();
		}
	}
	if (copy_to_clipboard)
		ImGui::LogFinish();

	if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
		ImGui::SetScrollHereY(1.0f);
	ScrollToBottom = false;

	ImGui::PopStyleVar();
	ImGui::EndChild();

	if (showFooter)
	{
		ImGui::Separator();

		if (ImGui::SmallButton("Add Debug Text")) { AddLogF("%d some text", Items.Size); AddLogF("some more text"); AddLogF("display very important message here!"); }
		ImGui::SameLine();
		if (ImGui::SmallButton("Add Debug Error")) { AddLogF("[error] something went wrong"); }
		ImGui::SameLine();
		if (ImGui::SmallButton("Clear")) { ClearLog(); }
		//ImGui::SameLine();
		//bool copy_to_clipboard = ImGui::SmallButton("Copy");
		ImGui::Separator();

		// Options, Filter
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		Filter.Draw("Filter (\"incl,-excl\") (\"error\")");

		if (showFooterCommandInput)
		{
			ImGui::Separator();
			// Command-line
			bool reclaim_focus = false;
			ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
			if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
			{
				char* s = InputBuf;
				Strtrim(s);
				if (s[0])
					ExecCommand(s);
				//strcpy(s, "");
				reclaim_focus = true;
			}

			// Auto-focus on window apparition
			ImGui::SetItemDefaultFocus();
			if (reclaim_focus)
				ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
		}
	}

	ImGui::End();
}