#include "editor_extension.h"


#ifdef IMGUI_DEFINE_MATH_OPERATORS
#undef IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"


using namespace Engine;
using namespace ImGui;

// I dont know why he cant find this. Maybe testing it some day, but for know its ok.
static inline ImVec2 operator*(const ImVec2& lhs, const float rhs)              { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs)              { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs)                  { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs)                  { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs)            { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs)            { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs)            { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }


// ----------------------------------------------------------------------------
// SELECTION HELPERS mostly copied from imgui_internal.h and imgui_widgets.cpp



static ImVec2 InputTextCalcTextSizeW(const char* text_begin, const char* text_end, const char** remaining, ImVec2* out_offset, float wrap_width)
{
    ImGuiContext& g = *GImGui;
    ImFont* font = g.Font;
    const float line_height = g.FontSize;
    const float scale = line_height / font->FontSize;
    const char* word_wrap_eol = NULL;


    ImVec2 text_size = ImVec2(0, 0);
    float line_width = 0.0f;

    const char* s = text_begin;
    while (s < text_end)
    {
        // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
        /*if (!word_wrap_eol)
        {
            word_wrap_eol = font->CalcWordWrapPositionA(scale, s, text_end, wrap_width);
            if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
        }
        if (s >= word_wrap_eol)
        {
            word_wrap_eol = NULL;
            
            text_size.x = ImMax(text_size.x, line_width);
            text_size.y += line_height;

            // Wrapping skips upcoming blanks
            while (s < text_end)
            {
                const char c = *s;
                if (ImCharIsBlankA(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
            }
            continue;
        }*/
        unsigned int c = (unsigned int)(*s++);
        if (c == '\n')
        {
            text_size.x = ImMax(text_size.x, line_width);
            text_size.y += line_height;
            line_width = 0.0f;
            continue;
        }
        if (c == '\r')
            continue;

        const float char_width = font->GetCharAdvance((ImWchar)c) * scale;
        line_width += char_width;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (out_offset)
        *out_offset = ImVec2(line_width, text_size.y + line_height);  // offset allow for the possibility of sitting after a trailing \n

    if (line_width > 0 || text_size.y == 0.0f)                        // whereas size.y will ignore the trailing \n
        text_size.y += line_height;

    if (remaining)
        *remaining = s;

    return text_size;
}

static float   E_TEXTEDIT_GETWIDTH(ImGuiInputTextState* obj, int line_start_idx, int char_idx)  { char c = obj->TextA[line_start_idx + char_idx]; if (c == '\n') return STB_TEXTEDIT_GETWIDTH_NEWLINE; ImGuiContext& g = *GImGui; return g.Font->GetCharAdvance(c) * (g.FontSize / g.Font->FontSize); }

static void E_TEXTEDIT_LAYOUTROW(ImStb::StbTexteditRow* r, ImGuiInputTextState* obj, int lineStartIndex, float wrap_width)
{
    const char* text = obj->TextA.Data;
    const char* text_remaining = NULL;
    const ImVec2 size = InputTextCalcTextSizeW(text + lineStartIndex, text + obj->CurLenA, &text_remaining, NULL, wrap_width);
    r->x0 = 0.0f;
    r->x1 = size.x;
    r->baseline_y_delta = size.y;
    r->ymin = 0.0f;
    r->ymax = size.y;
    r->num_chars = (int)(text_remaining - (text + lineStartIndex));
}

static int e_text_to_local_cord(STB_TEXTEDIT_STRING *str, float x, float y, float wrap_width)
{
   ImStb::StbTexteditRow r;
   int n = str->CurLenA;
   float base_y = 0, prev_x;
   int i=0, k;

   r.x0 = r.x1 = 0;
   r.ymin = r.ymax = 0;
   r.num_chars = 0;

   // search rows to find one that straddles 'y'
   while (i < n) {
      E_TEXTEDIT_LAYOUTROW(&r, str, i, wrap_width);
      if (r.num_chars <= 0)
         return n;

      //if (i==0 && y < base_y + r.ymin)
      //   return 0;

      if (y < base_y + r.ymax)
         break;

      i += r.num_chars;
      base_y += r.baseline_y_delta;
   }

   // below all text, return 'after' last character
   if (i >= n)
      return n;

   // check if it's before the beginning of the line
   if (x < r.x0)
      return i;

   // check if it's before the end of the line
   if (x < r.x1) {
      // search characters in row for one that straddles 'x'
      prev_x = r.x0;
      for (k=0; k < r.num_chars; ++k) {
         float w = E_TEXTEDIT_GETWIDTH(str, i, k);
         if (x < prev_x+w) {
            if (x < prev_x+w/2)
               return k+i;
            else
               return k+i+1;
         }
         prev_x += w;
      }
      // shouldn't happen, but if it does, fall through to end-of-line case
   }

   // if the last character is a newline, return that. otherwise return 'after' the last character
   if (str->TextA[i+r.num_chars-1] == '\n')
      return i+r.num_chars-1;
   else
      return i+r.num_chars;
}


static void e_textedit_click(STB_TEXTEDIT_STRING *str, ImStb::STB_TexteditState *state, float x, float y, float wrap_width)
{
    // In single-line mode, just always make y = 0. This lets the drag keep working if the mouse
    // goes off the top or bottom of the text
    if( state->single_line )
    {
        ImStb::StbTexteditRow r;
        E_TEXTEDIT_LAYOUTROW(&r, str, 0, wrap_width);
        y = r.ymin;
    }

   state->cursor = e_text_to_local_cord(str, x, y, wrap_width);
   state->select_start = state->cursor;
   state->select_end = state->cursor;
   state->has_preferred_x = 0;
}


static void e_textedit_drag(ImGuiInputTextState* str, ImStb::STB_TexteditState* state, float x, float y, float wrap_width)
{
    int p = 0;
    // In single-line mode, just always make y = 0. This lets the drag keep working if the mouse
    // goes off the top or bottom of the text
    if( state->single_line )
    {
        ImStb::StbTexteditRow r;
        E_TEXTEDIT_LAYOUTROW(&r, str, 0, wrap_width);
        y = r.ymin;
    }

    if (state->select_start == state->select_end)
        state->select_start = state->cursor;

    p = e_text_to_local_cord(str, x, y, wrap_width);
    E_INFO("Slecting to index: " + std::to_string(p));
    state->cursor = state->select_end = p;
}

    // Implemented after ImGui::InputTextEx(...)

// reset the state to default
static void e_textedit_clear_state(ImStb::STB_TexteditState *state, int is_single_line)
{
   state->undostate.undo_point = 0;
   state->undostate.undo_char_point = 0;
   state->undostate.redo_point = STB_TEXTEDIT_UNDOSTATECOUNT;
   state->undostate.redo_char_point = STB_TEXTEDIT_UNDOCHARCOUNT;
   state->select_end = state->select_start = 0;
   state->cursor = 0;
   state->has_preferred_x = 0;
   state->preferred_x = 0;
   state->cursor_at_end_of_line = 0;
   state->initialized = 1;
   state->single_line = (unsigned char) is_single_line;
   state->insert_mode = 0;
   state->row_count_per_page = 0;
}

bool ExUI::RenderText(const char* text, const ImVec2& size, bool copy)
{
    static const char* fake_label = "SelectText";
    // Draw text to be selectable and copyablek
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    // Get Global style and io
    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;

    const bool RENDER_SELECTION_WHEN_INACTIVE = false;

    ImGui::BeginGroup();
    //Get ID
    const ImGuiID id = window->GetID(fake_label);    
    // Get item frame size
    const ImVec2 frame_size = ImGui::CalcItemSize(size, ImGui::CalcItemWidth(), g.FontSize * 8.0f);
    const ImRect frame_bb(window->DC.CursorPos, {window->DC.CursorPos + frame_size});
    ImVec2 inner_size = frame_size;



    ImGuiWindow* draw_window = window;
    ImGuiItemStatusFlags item_status_flags = g.LastItemData.StatusFlags;
    ImGuiLastItemData item_data_backup = g.LastItemData;


    ImVec2 backup_pos = window->DC.CursorPos;
    ItemSize(frame_bb, style.FramePadding.y);
    if (!ItemAdd(frame_bb, id))
    {
        EndGroup();
        return false;
    }
    window->DC.CursorPos = backup_pos;

    PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_FrameBg]);
    PushStyleVar(ImGuiStyleVar_ChildRounding, style.FrameRounding);
    PushStyleVar(ImGuiStyleVar_ChildBorderSize, style.FrameBorderSize);
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    bool child_visible = BeginChildEx(fake_label /** Dont know for what exaclty i need it. Could mess up if you show imltiple of these frames */
                                        , id, frame_bb.GetSize(), true, ImGuiWindowFlags_NoMove);

    PopStyleVar(3);
    PopStyleColor();
    if (!child_visible)
    {
        EndChild();
        EndGroup();
        return false;
    }
    draw_window = g.CurrentWindow;
    draw_window->DC.NavLayersActiveMaskNext |= (1 << draw_window->DC.NavLayerCurrent);
    draw_window->DC.CursorPos += style.FramePadding;

    inner_size.x -= draw_window->ScrollbarSizes.x;

    const bool hovered = ItemHoverable(frame_bb, id);
    if (hovered)
    {
        g.MouseCursor = ImGuiMouseCursor_TextInput;
    }

    backup_pos = window->DC.CursorPos;
    window->DC.CursorPos = backup_pos;

    ImGuiInputTextState* state = GetInputTextState(id);

    const bool input_requested_by_tabbing = (item_status_flags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
    const bool input_requested_by_nav = (g.ActiveId != id) && ((g.NavActivateInputId == id) || (g.NavActivateId == id && g.NavInputSource == ImGuiInputSource_Keyboard));


    const bool user_clicked = hovered && io.MouseClicked[0];
    const bool user_scroll_finish = state != NULL && g.ActiveId == 0 && g.ActiveIdPreviousFrame == GetWindowScrollbarID(draw_window, ImGuiAxis_Y);
    const bool user_scroll_active = state != NULL && g.ActiveId == GetWindowScrollbarID(draw_window, ImGuiAxis_Y);
    
    const bool init_changed_specs = (state != NULL && state->Stb.single_line != false);
    const bool init_make_active = (user_clicked || user_scroll_finish || input_requested_by_nav || input_requested_by_tabbing);
    const bool init_state = (init_make_active || user_scroll_active);
    if ((init_state && g.ActiveId != id) || init_changed_specs)
    {
        // Access state even if we don't own it yet.
        state = &g.InputTextState;
        state->CursorAnimReset();

        const int buf_len = (int)strlen(text);
        state->InitialTextA.resize(buf_len + 1);    // UTF-8. we use +1 to make sure that .Data is always pointing to at least an empty string.
        memcpy(state->InitialTextA.Data, text, buf_len + 1);
        state->TextAIsValid = true;
        state->TextA.resize(buf_len + 1);
        memcpy(state->TextA.Data, text, buf_len + 1);
        state->CurLenA = buf_len;      // We can't get the result from ImStrncpy() above because it is not UTF-8 aware. Here we'll cut off malformed UTF-8.

        state->ID = id;
        state->ScrollX = 0.0f;
        e_textedit_clear_state(&state->Stb, false);
    }

    
    if (IsKeyDown(ImGuiKey_Space))
    {
        E_INFO("Mouse Pos: " + std::to_string(io.MousePos.x) + " " + std::to_string(io.MousePos.y));
        E_INFO("Cursor Pos: " + std::to_string(window->DC.CursorPos.x) + " " + std::to_string(window->DC.CursorPos.y));
    }
    
    if (state)
    {
        const float mouse_x = (io.MousePos.x - frame_bb.Min.x - style.FramePadding.x) + state->ScrollX;
        const float mouse_y = io.MousePos.y - draw_window->DC.CursorPos.y;

        if (io.MouseClicked[0] && !state->SelectedAllMouseLock)
        {
            // FIXME: unselect on late click could be done release?
            if (hovered)
            {
                e_textedit_click(state, &state->Stb, mouse_x, mouse_y, size.x);
                state->CursorAnimReset();
            }
        }
        else if (io.MouseDown[0] && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f))
        {

            e_textedit_drag(state, &state->Stb, mouse_x, mouse_y, size.x);
            //E_INFO("Selecting text: " + std::to_string(state->GetSelectionStart()) + " " + std::to_string(state->GetSelectionEnd()));
            state->CursorAnimReset();
            state->CursorFollow = true;
        }


        const char* text_begin = state->TextA.Data;
        const char* text_end = text_begin + state->CurLenA;

        const char* text_selected_begin = text_begin + ImMin(state->Stb.select_start, state->Stb.select_end);
        const char* text_selected_end = text_begin + ImMax(state->Stb.select_start, state->Stb.select_end);
        const bool has_selection = state->Stb.select_start != state->Stb.select_end;
        if (has_selection)
        {
            const char* word_wrap_eol = NULL;
            const float scale = g.FontSize / g.Font->FontSize;
            i32 line_count = 0;
            i32 selection_start_line_count = -1;

            // Count lines
            for (const char* s = text_begin; *s != 0; s++)
            {
                /*if (!word_wrap_eol)
                {
                    word_wrap_eol = g.Font->CalcWordWrapPositionA(scale, s, text_begin, size.x);
                    if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                        word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
                }
                bool newLine = s >= word_wrap_eol || *s == '\n';
                if (s >= word_wrap_eol)
                {
                    word_wrap_eol = NULL;
                    // Wrapping skips upcoming blanks
                    while (s < text_end)
                    {
                        const char c = *s;
                        if (ImCharIsBlankA(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                    }
                }*/

                if (*s == '\n')
                {
                    if (s >= text_selected_begin)
                    {
                        selection_start_line_count = line_count;
                    }
                    line_count++;
                }
            }
            line_count++;

            if (selection_start_line_count == -1)
            {
                selection_start_line_count = line_count;
            }

            ImVec2 selected_start_offset;

            selected_start_offset.x = InputTextCalcTextSizeW((const char*)ImStrbolW((const ImWchar*)text_selected_begin, (const ImWchar*)text_begin), text_selected_begin, NULL, NULL, size.x).x;
            selected_start_offset.y = selection_start_line_count * g.FontSize;


            const ImVec4 clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + inner_size.x, frame_bb.Min.y + inner_size.y); // Not using frame_bb.Max because we have adjusted size
            ImVec2 draw_pos = draw_window->DC.CursorPos;
            const ImVec2 draw_scroll = ImVec2(state->ScrollX, 0.0f);



            ImVec2 rect_pos = draw_pos + selected_start_offset - draw_scroll;
            for (const char* p = text_selected_begin; p < text_selected_end;)
            {
                if (rect_pos.y > clip_rect.w + g.FontSize)
                    break;
                if (rect_pos.y < clip_rect.y)
                {
                    while (p < text_selected_end)
                        if (*p++== '\n')
                            break;
                }
                else
                {
                    ImVec2 rect_size = InputTextCalcTextSizeW(p, text_selected_end, &p, NULL, size.x);
                    if (rect_size.x <= 0.0f) rect_size.x = (float)(int)(g.Font->GetCharAdvance((unsigned short)' ') * 0.50f); // So we can see the selection
                    ImRect rect(rect_pos + ImVec2(0.0f, -g.FontSize), rect_pos + ImVec2(rect_size.x, 0.0f));
                    rect.ClipWith(clip_rect);
                    if (rect.Overlaps(clip_rect))
                        draw_window->DrawList->AddRectFilled(rect.Min, rect.Max, ImGui::GetColorU32(ImGuiCol_TextSelectedBg));
                }
                rect_pos.x = draw_pos.x - draw_scroll.x;
                rect_pos.y += g.FontSize;
            }
        }
    }    

    
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + size.x);
    TextEx(text);
    ImGui::PopTextWrapPos();


    // For focus requests to work on our multiline we need to ensure our child ItemAdd() call specifies the ImGuiItemFlags_Inputable (ref issue #4761)...
    //Dummy(ImVec2(text_size.x, text_size.y + style.FramePadding.y));
    ImGuiItemFlags backup_item_flags = g.CurrentItemFlags;
    g.CurrentItemFlags |= ImGuiItemFlags_Inputable | ImGuiItemFlags_NoTabStop;
    EndChild();
    item_data_backup.StatusFlags |= (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_HoveredWindow);
    g.CurrentItemFlags = backup_item_flags;

    // ...and then we need to undo the group overriding last item data, which gets a bit messy as EndGroup() tries to forward scrollbar being active...
    // FIXME: This quite messy/tricky, should attempt to get rid of the child window.
    EndGroup();
    if (g.LastItemData.ID == 0)
    {
        g.LastItemData.ID = id;
        g.LastItemData.InFlags = item_data_backup.InFlags;
        g.LastItemData.StatusFlags = item_data_backup.StatusFlags;
    }

    return false;
}
