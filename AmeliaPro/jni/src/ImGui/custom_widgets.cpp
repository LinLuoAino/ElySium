#define IMGUI_DEFINE_MATH_OPERATORS

#include "custom_widgets.h"
#include "imstb_textedit.h"
#include "imgui_internal.h"
using namespace ImGui;
using namespace ImStb;

extern float 上下边框和中间的间距;

void c_widgets::set_next_window_pos(const ImVec2& pos, ImGuiCond cond, const ImVec2& pivot)
{
    ImGuiContext& g = *GImGui;

    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasPos;
    g.NextWindowData.PosVal = pos;
    g.NextWindowData.PosPivotVal = ImVec2(ImClamp(pivot.x, 0.0f, 1.0f), ImClamp(pivot.y, 0.0f, 1.0f));
    g.NextWindowData.PosCond = (cond != 0 && ImIsPowerOfTwo(cond)) ? cond : ImGuiCond_Always;
}

void c_widgets::water_mark(std::string name, std::vector<std::string> function, watermark_position type, bool* visible)
{
    struct slot_anim {
        char current_text[64] = "";
        char old_text[64] = "";
        float factor = 0.0f;
    };

    struct spectator_state
    {
        float alpha;
        ImVec2 content_size, current_pos, pos;
        slot_anim anims[12];
    };

    spectator_state* state = gui->anim_container(&state, ImGui::GetID(name.c_str()));

    state->alpha = ImClamp(state->alpha + (gui->fixed_speed(12.f) * (*visible ? 1.f : -1.f)), 0.f, 1.f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, state->alpha);

    if (state->alpha >= 0.01f) {
        ImVec2 display_size = ImGui::GetIO().DisplaySize;
        float padding = 20.0f * DPI;

        switch (type)
        {
        case mark_top_left:
            state->pos = ImVec2(padding, padding);
            break;
        case mark_top_right:
            state->pos = ImVec2(display_size.x - state->content_size.x - padding, padding);
            break;
        case mark_bottom_left:
            state->pos = ImVec2(padding, display_size.y - state->content_size.y - padding);
            break;
        case mark_bottom_right:
            state->pos = ImVec2(display_size.x - state->content_size.x - padding, display_size.y - state->content_size.y - padding);
            break;
        }

        state->current_pos = ImLerp(state->current_pos, state->pos, fixed_speed(25.f));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, YuanerSet->window_rounding);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15 * DPI, 10 * DPI));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10 * DPI, 10 * DPI));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, gui->get_clr(clr->window_background));
        
        ImGui::SetNextWindowPos(state->current_pos);
        ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove);
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 cur_pos = ImGui::GetCursorScreenPos();

            if (YuanerSet->water_name)
            {
                ImFont* icon_font = YuanerSet->iconFont;
                draw_list->AddText(icon_font, icon_font->FontSize, cur_pos, gui->get_clr(clr->accent), "A");
                draw_list->AddText(icon_font, icon_font->FontSize, cur_pos, ImGui::ColorConvertFloat4ToU32({1,1,1,1}), "B");

                ImFont* bold_font = YuanerSet->inter_semibold_font;
                ImVec2 title_pos = cur_pos + ImVec2(35 * DPI, 0);
                draw_list->AddText(bold_font, bold_font->FontSize, title_pos, ImGui::ColorConvertFloat4ToU32({1,1,1,1}), "ELY");
                
                float arc_w = bold_font->CalcTextSizeA(bold_font->FontSize, FLT_MAX, 0.0f, "ELY").x;
                draw_list->AddText(bold_font, bold_font->FontSize, title_pos + ImVec2(arc_w, 0), gui->get_clr(clr->accent), "SIUM");

                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (25.0f * DPI));
            }

            ImGui::BeginGroup();
            for (size_t i = 0; i < function.size(); i++)
            {
                if (i >= 12) break;
                slot_anim& anim = state->anims[i];
                std::string target_text = function[i];

                if (target_text != anim.current_text) {
                    strcpy(anim.old_text, anim.current_text);
                    strcpy(anim.current_text, target_text.c_str());
                    anim.factor = 1.0f;
                }

                anim.factor = ImLerp(anim.factor, 0.0f, fixed_speed(15.f));
                if (anim.factor < 0.001f) anim.factor = 0.0f;

                ImVec2 text_size = ImGui::CalcTextSize(anim.current_text);
                ImVec2 p = ImGui::GetCursorScreenPos();

                ImGui::PushClipRect(p, p + ImVec2(text_size.x, text_size.y), true);
                
                float offset_y = anim.factor * text_size.y;

                if (anim.factor > 0.001f && anim.old_text[0] != '\0') {
                    draw_list->AddText(p - ImVec2(0, text_size.y - offset_y), gui->get_clr(clr->Text), anim.old_text);
                }

                draw_list->AddText(p + ImVec2(0, offset_y), gui->get_clr(clr->Text), anim.current_text);

                ImGui::PopClipRect();

                ImGui::Dummy(text_size);
                if (i < function.size() - 1) ImGui::SameLine();
            }
            ImGui::EndGroup();

            state->content_size = ImGui::GetWindowSize();
        }
        ImGui::End();

        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar(3);
    }
    ImGui::PopStyleVar();
}

bool c_widgets::begin_popup(float size)
{
    struct settings_state
    {
        bool pressed;
        float alpha_popup, content_size_y;
        ImVec2 current_pos;
    };

    ImGuiContext& g = *GImGui;
    ImGuiID id = g.LastItemData.ID;

    settings_state* state = gui->anim_container(&state, ImGui::GetCurrentWindow()->GetID(id));

    bool is_long_pressed = ImGui::IsItemActive() && ImGui::GetIO().MouseDownDuration[0] >= 0.5f;
    if (is_long_pressed && !state->pressed) {
        state->pressed = true;
    }

    if (state->pressed && ImGui::IsMouseClicked(0)) {
        ImVec2 window_pos = state->current_pos;
        ImVec2 window_size = ImVec2(size * DPI, state->content_size_y);
        
        if (!ImGui::IsMouseHoveringRect(window_pos, window_pos + window_size) && !ImGui::IsItemHovered()) {
            state->pressed = false;
        }
    }

    state->alpha_popup = ImClamp(state->alpha_popup + (gui->fixed_speed(6.f) * (state->pressed ? 1.f : -1.f)), 0.f, 1.f);

    if (state->alpha_popup <= 0.01f) return false;

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, state->alpha_popup);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f * DPI);

    ImVec2 pos = g.LastItemData.Rect.GetBL();
    pos.x = ImMin(pos.x, g.IO.DisplaySize.x - (size * DPI) - (15.0f * DPI));
    pos.y = ImMin(pos.y, g.IO.DisplaySize.y - state->content_size_y - (15.0f * DPI));
    state->current_pos = pos;

    set_next_window_pos(pos, ImGuiCond_Always, ImVec2(0, 0));

    char name[32];
    sprintf(name, "##popup_%08X", id);
    
    if (ImGui::Begin(name, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse))
    {
        YuanerSet->open_popup = state->pressed;
        
        ImGui::BeginChild("##content", ImVec2(size * DPI, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
        {
            state->content_size_y = ImGui::GetWindowHeight(); 
        }
    }

    return true; 
}

void add_rect_filled_multi_color(ImDrawList* draw, const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left, float rounding = 0.f, ImDrawFlags flags = 0)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0)
        return;

    auto fix_rect_corner_flags{ [](ImDrawFlags rflags) {    if ((rflags & ImDrawFlags_RoundCornersMask_) == 0) rflags |= ImDrawFlags_RoundCornersAll; return rflags; }};

    flags = fix_rect_corner_flags(flags);
    rounding = ImMin(rounding, ImFabs(p_max.x - p_min.x) * (((flags & ImDrawFlags_RoundCornersTop) == ImDrawFlags_RoundCornersTop) || ((flags & ImDrawFlags_RoundCornersBottom) == ImDrawFlags_RoundCornersBottom) ? 0.5f : 1.0f) - 1.0f);
    rounding = ImMin(rounding, ImFabs(p_max.y - p_min.y) * (((flags & ImDrawFlags_RoundCornersLeft) == ImDrawFlags_RoundCornersLeft) || ((flags & ImDrawFlags_RoundCornersRight) == ImDrawFlags_RoundCornersRight) ? 0.5f : 1.0f) - 1.0f);

    if (rounding > 0.0f)
    {
        const int size_before = draw->VtxBuffer.Size;
        draw->AddRectFilled(p_min, p_max, IM_COL32_WHITE, rounding, flags);
        const int size_after = draw->VtxBuffer.Size;

        for (int i = size_before; i < size_after; i++)
        {
            ImDrawVert* vert = draw->VtxBuffer.Data + i;

            ImVec4 upr_left = ImGui::ColorConvertU32ToFloat4(col_upr_left);
            ImVec4 bot_left = ImGui::ColorConvertU32ToFloat4(col_bot_left);
            ImVec4 up_right = ImGui::ColorConvertU32ToFloat4(col_upr_right);
            ImVec4 bot_right = ImGui::ColorConvertU32ToFloat4(col_bot_right);

            float X = ImClamp((vert->pos.x - p_min.x) / (p_max.x - p_min.x), 0.0f, 1.0f);

            // 4 colors - 8 deltas

            float r1 = upr_left.x + (up_right.x - upr_left.x) * X;
            float r2 = bot_left.x + (bot_right.x - bot_left.x) * X;

            float g1 = upr_left.y + (up_right.y - upr_left.y) * X;
            float g2 = bot_left.y + (bot_right.y - bot_left.y) * X;

            float b1 = upr_left.z + (up_right.z - upr_left.z) * X;
            float b2 = bot_left.z + (bot_right.z - bot_left.z) * X;

            float a1 = upr_left.w + (up_right.w - upr_left.w) * X;
            float a2 = bot_left.w + (bot_right.w - bot_left.w) * X;


            float Y = ImClamp((vert->pos.y - p_min.y) / (p_max.y - p_min.y), 0.0f, 1.0f);
            float r = r1 + (r2 - r1) * Y;
            float g = g1 + (g2 - g1) * Y;
            float b = b1 + (b2 - b1) * Y;
            float a = a1 + (a2 - a1) * Y;
            ImVec4 RGBA(r, g, b, a);

            RGBA = RGBA * ImGui::ColorConvertU32ToFloat4(vert->col);

            vert->col = ImColor(RGBA);
        }
        return;
    }

    const ImVec2 uv = draw->_Data->TexUvWhitePixel;
    draw->PrimReserve(6, 4);
    draw->PrimWriteIdx((ImDrawIdx)(draw->_VtxCurrentIdx)); draw->PrimWriteIdx((ImDrawIdx)(draw->_VtxCurrentIdx + 1)); draw->PrimWriteIdx((ImDrawIdx)(draw->_VtxCurrentIdx + 2));
    draw->PrimWriteIdx((ImDrawIdx)(draw->_VtxCurrentIdx)); draw->PrimWriteIdx((ImDrawIdx)(draw->_VtxCurrentIdx + 2)); draw->PrimWriteIdx((ImDrawIdx)(draw->_VtxCurrentIdx + 3));
    draw->PrimWriteVtx(p_min, uv, col_upr_left);
    draw->PrimWriteVtx(ImVec2(p_max.x, p_min.y), uv, col_upr_right);
    draw->PrimWriteVtx(p_max, uv, col_bot_right);
    draw->PrimWriteVtx(ImVec2(p_min.x, p_max.y), uv, col_bot_left);
}

static float calc_combo_size(int items_count, float item_size)
{
    ImGuiContext& g = *GImGui;
    if (items_count <= 0)
        return FLT_MAX;
    return item_size * items_count + g.Style.ItemSpacing.y * (items_count - 1);
}

static ImVec2 input_text_calc_text_size_w(ImGuiContext* ctx, const ImWchar* text_begin, const ImWchar* text_end, const ImWchar** remaining = NULL, ImVec2* out_offset = NULL, bool stop_on_new_line = false)
{
    ImGuiContext& g = *ctx;
    ImFont* font = g.Font;
    const float line_height = g.FontSize;
    const float scale = line_height / font->FontSize;

    ImVec2 text_size = ImVec2(0, 0);
    float line_width = 0.0f;

    const ImWchar* s = text_begin;
    while (s < text_end)
    {
        unsigned int c = (unsigned int)(*s++);
        if (c == '\n')
        {
            text_size.x = ImMax(text_size.x, line_width);
            text_size.y += line_height;
            line_width = 0.0f;
            if (stop_on_new_line)
                break;
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

static const char* items_array_getter(void* data, int idx)
{
    const char* const* items = (const char* const*)data;
    return items[idx];
}

namespace ImStb
{

    static int     STB_TEXTEDIT_STRINGLEN(const ImGuiInputTextState* obj) { return obj->CurLenW; }
    static ImWchar STB_TEXTEDIT_GETCHAR(const ImGuiInputTextState* obj, int idx) { IM_ASSERT(idx <= obj->CurLenW); return obj->TextW[idx]; }
    static float   STB_TEXTEDIT_GETWIDTH(ImGuiInputTextState* obj, int line_start_idx, int char_idx) { ImWchar c = obj->TextW[line_start_idx + char_idx]; if (c == '\n') return IMSTB_TEXTEDIT_GETWIDTH_NEWLINE; ImGuiContext& g = *obj->Ctx; return g.Font->GetCharAdvance(c) * (g.FontSize / g.Font->FontSize); }
    static int     STB_TEXTEDIT_KEYTOTEXT(int key) { return key >= 0x200000 ? 0 : key; }
    static ImWchar STB_TEXTEDIT_NEWLINE = '\n';
    static void    STB_TEXTEDIT_LAYOUTROW(StbTexteditRow* r, ImGuiInputTextState* obj, int line_start_idx)
    {
        const ImWchar* text = obj->TextW.Data;
        const ImWchar* text_remaining = NULL;
        const ImVec2 size = input_text_calc_text_size_w(obj->Ctx, text + line_start_idx, text + obj->CurLenW, &text_remaining, NULL, true);
        r->x0 = 0.0f;
        r->x1 = size.x;
        r->baseline_y_delta = size.y;
        r->ymin = 0.0f;
        r->ymax = size.y;
        r->num_chars = (int)(text_remaining - (text + line_start_idx));
    }

    static bool is_separator(unsigned int c)
    {
        return c == ',' || c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == '|' || c == '\n' || c == '\r' || c == '.' || c == '!';
    }

    static int is_word_boundary_from_right(ImGuiInputTextState* obj, int idx)
    {
        // When ImGuiInputTextFlags_Password is set, we don't want actions such as CTRL+Arrow to leak the fact that underlying data are blanks or separators.
        if ((obj->Flags & ImGuiInputTextFlags_Password) || idx <= 0)
            return 0;

        bool prev_white = ImCharIsBlankW(obj->TextW[idx - 1]);
        bool prev_separ = is_separator(obj->TextW[idx - 1]);
        bool curr_white = ImCharIsBlankW(obj->TextW[idx]);
        bool curr_separ = is_separator(obj->TextW[idx]);
        return ((prev_white || prev_separ) && !(curr_separ || curr_white)) || (curr_separ && !prev_separ);
    }
    static int is_word_boundary_from_left(ImGuiInputTextState* obj, int idx)
    {
        if ((obj->Flags & ImGuiInputTextFlags_Password) || idx <= 0)
            return 0;

        bool prev_white = ImCharIsBlankW(obj->TextW[idx]);
        bool prev_separ = is_separator(obj->TextW[idx]);
        bool curr_white = ImCharIsBlankW(obj->TextW[idx - 1]);
        bool curr_separ = is_separator(obj->TextW[idx - 1]);
        return ((prev_white) && !(curr_separ || curr_white)) || (curr_separ && !prev_separ);
    }
    static int  STB_TEXTEDIT_MOVEWORDLEFT_IMPL(ImGuiInputTextState* obj, int idx) { idx--; while (idx >= 0 && !is_word_boundary_from_right(obj, idx)) idx--; return idx < 0 ? 0 : idx; }
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_MAC(ImGuiInputTextState* obj, int idx) { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_left(obj, idx)) idx++; return idx > len ? len : idx; }
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_WIN(ImGuiInputTextState* obj, int idx) { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_right(obj, idx)) idx++; return idx > len ? len : idx; }
    static int  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL(ImGuiInputTextState* obj, int idx) { ImGuiContext& g = *obj->Ctx; if (g.IO.ConfigMacOSXBehaviors) return STB_TEXTEDIT_MOVEWORDRIGHT_MAC(obj, idx); else return STB_TEXTEDIT_MOVEWORDRIGHT_WIN(obj, idx); }
#define STB_TEXTEDIT_MOVEWORDLEFT   STB_TEXTEDIT_MOVEWORDLEFT_IMPL  // They need to be #define for stb_textedit.h
#define STB_TEXTEDIT_MOVEWORDRIGHT  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL

    static void STB_TEXTEDIT_DELETECHARS(ImGuiInputTextState* obj, int pos, int n)
    {
        ImWchar* dst = obj->TextW.Data + pos;

        // We maintain our buffer length in both UTF-8 and wchar formats
        obj->Edited = true;
        obj->CurLenA -= ImTextCountUtf8BytesFromStr(dst, dst + n);
        obj->CurLenW -= n;

        // Offset remaining text (FIXME-OPT: Use memmove)
        const ImWchar* src = obj->TextW.Data + pos + n;
        while (ImWchar c = *src++)
            *dst++ = c;
        *dst = '\0';
    }

    static bool STB_TEXTEDIT_INSERTCHARS(ImGuiInputTextState* obj, int pos, const ImWchar* new_text, int new_text_len)
    {
        const bool is_resizable = (obj->Flags & ImGuiInputTextFlags_CallbackResize) != 0;
        const int text_len = obj->CurLenW;
        IM_ASSERT(pos <= text_len);

        const int new_text_len_utf8 = ImTextCountUtf8BytesFromStr(new_text, new_text + new_text_len);
        if (!is_resizable && (new_text_len_utf8 + obj->CurLenA + 1 > obj->BufCapacityA))
            return false;

        // Grow internal buffer if needed
        if (new_text_len + text_len + 1 > obj->TextW.Size)
        {
            if (!is_resizable)
                return false;
            IM_ASSERT(text_len < obj->TextW.Size);
            obj->TextW.resize(text_len + ImClamp(new_text_len * 4, 32, ImMax(256, new_text_len)) + 1);
        }

        ImWchar* text = obj->TextW.Data;
        if (pos != text_len)
            memmove(text + pos + new_text_len, text + pos, (size_t)(text_len - pos) * sizeof(ImWchar));
        memcpy(text + pos, new_text, (size_t)new_text_len * sizeof(ImWchar));

        obj->Edited = true;
        obj->CurLenW += new_text_len;
        obj->CurLenA += new_text_len_utf8;
        obj->TextW[obj->CurLenW] = '\0';

        return true;
    }

    // We don't use an enum so we can build even with conflicting symbols (if another user of stb_textedit.h leak their STB_TEXTEDIT_K_* symbols)
#define STB_TEXTEDIT_K_LEFT         0x200000 // keyboard input to move cursor left
#define STB_TEXTEDIT_K_RIGHT        0x200001 // keyboard input to move cursor right
#define STB_TEXTEDIT_K_UP           0x200002 // keyboard input to move cursor up
#define STB_TEXTEDIT_K_DOWN         0x200003 // keyboard input to move cursor down
#define STB_TEXTEDIT_K_LINESTART    0x200004 // keyboard input to move cursor to start of line
#define STB_TEXTEDIT_K_LINEEND      0x200005 // keyboard input to move cursor to end of line
#define STB_TEXTEDIT_K_TEXTSTART    0x200006 // keyboard input to move cursor to start of text
#define STB_TEXTEDIT_K_TEXTEND      0x200007 // keyboard input to move cursor to end of text
#define STB_TEXTEDIT_K_DELETE       0x200008 // keyboard input to delete selection or character under cursor
#define STB_TEXTEDIT_K_BACKSPACE    0x200009 // keyboard input to delete selection or character left of cursor
#define STB_TEXTEDIT_K_UNDO         0x20000A // keyboard input to perform undo
#define STB_TEXTEDIT_K_REDO         0x20000B // keyboard input to perform redo
#define STB_TEXTEDIT_K_WORDLEFT     0x20000C // keyboard input to move cursor left one word
#define STB_TEXTEDIT_K_WORDRIGHT    0x20000D // keyboard input to move cursor right one word
#define STB_TEXTEDIT_K_PGUP         0x20000E // keyboard input to move cursor up a page
#define STB_TEXTEDIT_K_PGDOWN       0x20000F // keyboard input to move cursor down a page
#define STB_TEXTEDIT_K_SHIFT        0x400000

#define IMSTB_TEXTEDIT_IMPLEMENTATION
#define IMSTB_TEXTEDIT_memmove memmove
#include "imstb_textedit.h"

// stb_textedit internally allows for a single undo record to do addition and deletion, but somehow, calling
// the stb_textedit_paste() function creates two separate records, so we perform it manually. (FIXME: Report to nothings/stb?)
    static void stb_textedit_replace(ImGuiInputTextState* str, STB_TexteditState* state, const IMSTB_TEXTEDIT_CHARTYPE* text, int text_len)
    {
        stb_text_makeundo_replace(str, state, 0, str->CurLenW, text_len);
        ImStb::STB_TEXTEDIT_DELETECHARS(str, 0, str->CurLenW);
        state->cursor = state->select_start = state->select_end = 0;
        if (text_len <= 0)
            return;
        if (ImStb::STB_TEXTEDIT_INSERTCHARS(str, 0, text, text_len))
        {
            state->cursor = state->select_start = state->select_end = text_len;
            state->has_preferred_x = 0;
            return;
        }
        IM_ASSERT(0); // Failed to insert character, normally shouldn't happen because of how we currently use stb_textedit_replace()
    }

} // namespace ImStb

const char* keys[] =
{
    "None",
    "Mouse 1",
    "Mouse 2",
    "CN",
    "Mouse 3",
    "Mouse 4",
    "Mouse 5",
    "-",
    "Back",
    "Tab",
    "-",
    "-",
    "CLR",
    "Enter",
    "-",
    "-",
    "Shift",
    "CTL",
    "Menu",
    "Pause",
    "Caps",
    "KAN",
    "-",
    "JUN",
    "FIN",
    "KAN",
    "-",
    "Escape",
    "CON",
    "NCO",
    "ACC",
    "MAD",
    "Space",
    "PGU",
    "PGD",
    "End",
    "Home",
    "Left",
    "Up",
    "Right",
    "Down",
    "SEL",
    "PRI",
    "EXE",
    "PRI",
    "INS",
    "Delete",
    "HEL",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "WIN",
    "WIN",
    "APP",
    "-",
    "SLE",
    "Num 0",
    "Num 1",
    "Num 2",
    "Num 3",
    "Num 4",
    "Num 5",
    "Num 6",
    "Num 7",
    "Num 8",
    "Num 9",
    "MUL",
    "ADD",
    "SEP",
    "MIN",
    "Delete",
    "DIV",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "NUM",
    "SCR",
    "EQU",
    "MAS",
    "TOY",
    "OYA",
    "OYA",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "-",
    "Shift",
    "Shift",
    "Ctrl",
    "Ctrl",
    "Alt",
    "Alt"
};

ImU32 c_widgets::get_clr(const ImVec4& col, float alpha)
{
    ImGuiStyle& style = GImGui->Style;
    ImVec4 c = col;
    c.w *= style.Alpha * alpha;
    return ColorConvertFloat4ToU32(c);
}

void c_widgets::set_cursor_pos(const ImVec2& local_pos)
{
    ImGui::SetCursorPos(local_pos);
}

void c_widgets::begin_group()
{
    ImGui::BeginGroup();
}

void c_widgets::end_group()
{
    ImGui::EndGroup();
}

void c_widgets::push_style_var(ImGuiStyleVar idx, const ImVec2& val)
{
    ImGui::PushStyleVar(idx, val);
}

void c_widgets::push_style_var(ImGuiStyleVar idx, float val) {
    ImGui::PushStyleVar(idx, val);
}

void c_widgets::pop_style_var()
{
    ImGui::PopStyleVar();
}

void c_widgets::sameline()
{
    ImGui::SameLine(0.f, -1.f);
}

bool c_widgets::begin(std::string_view name, bool* open, ImGuiWindowFlags flags)
{
    return ImGui::Begin(name.data(), open, flags);
}

void c_widgets::end()
{
    ImGui::End();
}

bool c_widgets::selection(std::string_view label, std::string_view icon, int selection_id, int& selection_variable, int total_items, int item_index, bool round_corner)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    
    const ImGuiID id = window->GetID(label.data());
    const bool selected = (selection_id == selection_variable);

    const ImVec2 pos = window->DC.CursorPos;
    const float height = 50.f * DPI;
    
    struct selection_state {
        float alpha = 0.f;
        ImVec4 icon_color;
        ImVec4 text_color;
        float width_scale = -1.f;
    };
    
    static std::map<ImGuiID, selection_state> anim;
    auto& state = anim[id];

    if (state.width_scale < 0.f) {
        state.width_scale = 50.f * DPI;
        state.icon_color = clr->selection_icon;
        state.text_color = clr->selection_label;
    }

    if (!YuanerSet || !YuanerSet->icon || !YuanerSet->inter_semibold_font)
        return false;

    float icon_width = YuanerSet->icon->CalcTextSizeA(YuanerSet->icon->FontSize, FLT_MAX, -1.f, icon.data()).x;
    float text_width = YuanerSet->inter_semibold_font->CalcTextSizeA(YuanerSet->inter_semibold_font->FontSize, FLT_MAX, -1.f, label.data()).x;
    float content_width = icon_width + text_width + 20.f * DPI;

    state.width_scale = ImLerp(state.width_scale, selected ? content_width : 50.f * DPI, gui->fixed_speed(8.f));
    
    const ImRect rect(pos, pos + ImVec2(state.width_scale, height));

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id))
        return false;
    
    bool hovered, held;
    bool pressed = ButtonBehavior(rect, id, &hovered, &held);
    if (pressed)
        selection_variable = selection_id;
    
    state.alpha = ImLerp(state.alpha, selected ? 1.f : 0.f, gui->fixed_speed(8.f));
    state.icon_color = ImLerp(state.icon_color, selected ? clr->accent : clr->selection_icon, gui->fixed_speed(8.f));
    state.text_color = ImLerp(state.text_color, selected ? clr->accent : clr->selection_label, gui->fixed_speed(8.f));
    
    float rounding = 6.f * DPI;
    ImDrawFlags round_flags = ImDrawFlags_RoundCornersAll;
    
    if (total_items > 1) {
        if (item_index == 0) {
            round_flags = ImDrawFlags_RoundCornersLeft;
        } else if (item_index == total_items - 1) {
            round_flags = ImDrawFlags_RoundCornersRight;
        } else {
            round_flags = ImDrawFlags_RoundCornersNone;
        }
    }
    
    window->DrawList->AddRectFilled(rect.Min, rect.Max, 
        gui->get_clr(clr->selection_background, 0.3f), rounding, round_flags);
    
    if (state.alpha > 0.01f) {
        window->DrawList->AddRectFilled(rect.Min, rect.Max, 
            gui->get_clr(clr->accent, state.alpha * 0.2f), rounding, round_flags);
    }
    
    float icon_y = rect.Min.y + (height - YuanerSet->icon->FontSize) * 0.5f;
    float icon_x = rect.Min.x + 12.f * DPI;
    
    float text_y = rect.Min.y + (height - YuanerSet->inter_semibold_font->FontSize) * 0.5f;
    float text_x = icon_x + icon_width + 8.f * DPI;

    window->DrawList->PushClipRect(rect.Min, rect.Max, true);
    
    window->DrawList->AddText(YuanerSet->icon, YuanerSet->icon->FontSize, 
        ImVec2(icon_x, icon_y), gui->get_clr(state.icon_color), icon.data());
    
    window->DrawList->AddText(YuanerSet->inter_semibold_font, YuanerSet->inter_semibold_font->FontSize, 
        ImVec2(text_x, text_y), gui->get_clr(state.text_color, state.alpha), label.data());
        
    window->DrawList->PopClipRect();
    
    return pressed;
}

void c_widgets::begin_content()
{
    PushStyleVar(ImGuiStyleVar_ItemSpacing, YuanerSet->child_spacing);
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10 * DPI, 10 * DPI));
    BeginChildEx("content area", GetCurrentWindow()->GetID("content area"), GetWindowSize() - ImVec2(18 * DPI, (75 + 上下边框和中间的间距) * DPI), ImGuiChildFlags_None, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove);
}

void c_widgets::end_content()
{
    EndChild();
    ImGui::PopStyleVar(2);
}

bool c_widgets::begin_def_child(std::string_view name, const ImVec2& size_arg)
{
    ImGuiID id = GetCurrentWindow()->GetID(name.data());
    return BeginChildEx(name.data(), id, size_arg, 0, 0);
}

void c_widgets::end_def_child()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* child_window = g.CurrentWindow;

    IM_ASSERT(g.WithinEndChild == false);
    IM_ASSERT(child_window->Flags & ImGuiWindowFlags_ChildWindow);   // Mismatched BeginChild()/EndChild() calls

    g.WithinEndChild = true;
    ImVec2 child_size = child_window->Size;
    ImGui::End();
    if (child_window->BeginCount == 1)
    {
        ImGuiWindow* parent_window = g.CurrentWindow;
        ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + child_size);
        ItemSize(child_size);
        if ((child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY) && !(child_window->Flags & ImGuiWindowFlags_NavFlattened))
        {
            ItemAdd(bb, child_window->ChildId);
            RenderNavHighlight(bb, child_window->ChildId);

            // When browsing a window that has no activable items (scroll only) we keep a highlight on the child (pass g.NavId to trick into always displaying)
            if (child_window->DC.NavLayersActiveMask == 0 && child_window == g.NavWindow)
                RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_Compact);
        }
        else
        {
            // Not navigable into
            ItemAdd(bb, 0);

            // But when flattened we directly reach items, adjust active layer mask accordingly
            if (child_window->Flags & ImGuiWindowFlags_NavFlattened)
                parent_window->DC.NavLayersActiveMaskNext |= child_window->DC.NavLayersActiveMaskNext;
        }
        if (g.HoveredWindow == child_window)
            g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
    }
    g.WithinEndChild = false;
    g.LogLinePosY = -FLT_MAX; // To enforce a carriage return
}

bool begin_child_ex(const char* name, ImGuiID id, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* parent_window = g.CurrentWindow;
    IM_ASSERT(id != 0);

    // Sanity check as it is likely that some user will accidentally pass ImGuiWindowFlags into the ImGuiChildFlags argument.
    const ImGuiChildFlags ImGuiChildFlags_SupportedMask_ = ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_FrameStyle;
    IM_UNUSED(ImGuiChildFlags_SupportedMask_);
    IM_ASSERT((child_flags & ~ImGuiChildFlags_SupportedMask_) == 0 && "Illegal ImGuiChildFlags value. Did you pass ImGuiWindowFlags values instead of ImGuiChildFlags?");
    IM_ASSERT((window_flags & ImGuiWindowFlags_AlwaysAutoResize) == 0 && "Cannot specify ImGuiWindowFlags_AlwaysAutoResize for BeginChild(). Use ImGuiChildFlags_AlwaysAutoResize!");
    if (child_flags & ImGuiChildFlags_AlwaysAutoResize)
    {
        IM_ASSERT((child_flags & (ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY)) == 0 && "Cannot use ImGuiChildFlags_ResizeX or ImGuiChildFlags_ResizeY with ImGuiChildFlags_AlwaysAutoResize!");
        IM_ASSERT((child_flags & (ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) != 0 && "Must use ImGuiChildFlags_AutoResizeX or ImGuiChildFlags_AutoResizeY with ImGuiChildFlags_AlwaysAutoResize!");
    }
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    if (window_flags & ImGuiWindowFlags_AlwaysUseWindowPadding)
        child_flags |= ImGuiChildFlags_AlwaysUseWindowPadding;
#endif
    if (child_flags & ImGuiChildFlags_AutoResizeX)
        child_flags &= ~ImGuiChildFlags_ResizeX;
    if (child_flags & ImGuiChildFlags_AutoResizeY)
        child_flags &= ~ImGuiChildFlags_ResizeY;

    // Set window flags
    window_flags |= ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoTitleBar;
    window_flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove); // Inherit the NoMove flag
    if (child_flags & (ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize))
        window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    if ((child_flags & (ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY)) == 0)
        window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

    // Special framed style
    if (child_flags & ImGuiChildFlags_FrameStyle)
    {
        PushStyleColor(ImGuiCol_ChildBg, g.Style.Colors[ImGuiCol_FrameBg]);
        PushStyleVar(ImGuiStyleVar_ChildRounding, g.Style.FrameRounding);
        PushStyleVar(ImGuiStyleVar_ChildBorderSize, g.Style.FrameBorderSize);
        PushStyleVar(ImGuiStyleVar_WindowPadding, g.Style.FramePadding);
        child_flags |= ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysUseWindowPadding;
        window_flags |= ImGuiWindowFlags_NoMove;
    }

    // Forward child flags
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasChildFlags;
    g.NextWindowData.ChildFlags = child_flags;

    // Forward size
    // Important: Begin() has special processing to switch condition to ImGuiCond_FirstUseEver for a given axis when ImGuiChildFlags_ResizeXXX is set.
    // (the alternative would to store conditional flags per axis, which is possible but more code)
    
    static std::map<ImGuiID, float> anim;
    float& state = anim[id];

    ImVec2 size = size_arg;
    if (size.x <= 0)
        if (parent_window->ScrollbarY) {
            // 窗口有垂直滚
            size.x = (GetWindowWidth() - g.Style.ItemSpacing.x - g.Style.ScrollbarSize - (8 * DPI)) / 2;
        }else{
            size.x = (GetWindowWidth() - g.Style.ItemSpacing.x /*- g.Style.ScrollbarSize*/ - (8 * DPI)) / 2;
        }
    if (size.y <= 0)
        size.y = state;

    SetNextWindowSize(size);
    SetNextWindowPos(parent_window->DC.CursorPos + ImVec2(0, 30 * DPI));

    // Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
    // FIXME: 2023/11/14: commented out shorted version. We had an issue with multiple ### in child window path names, which the trailing hash helped workaround.
    // e.g. "ParentName###ParentIdentifier/ChildName###ChildIdentifier" would get hashed incorrectly by ImHashStr(), trailing _%08X somehow fixes it.
    const char* temp_window_name;
    /*if (name && parent_window->IDStack.back() == parent_window->ID)
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s", parent_window->Name, name); // May omit ID if in root of ID stack
    else*/
    if (name)
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
    else
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

    // Set style
    const float backup_border_size = g.Style.ChildBorderSize;
    if ((child_flags & ImGuiChildFlags_Border) == 0)
        g.Style.ChildBorderSize = 0.0f;

    parent_window->DrawList->AddRectFilled(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, 25 * DPI), gui->get_clr(clr->child_topbar), YuanerSet->child_rounding, ImDrawFlags_RoundCornersTop);
    parent_window->DrawList->AddRectFilled(parent_window->DC.CursorPos + ImVec2(0, 25 * DPI), parent_window->DC.CursorPos + size + ImVec2(0, 25 * DPI), gui->get_clr(clr->child_background), YuanerSet->child_rounding, ImDrawFlags_RoundCornersBottom);
    //标题文字
    parent_window->DrawList->AddText(YuanerSet->inter_semibold_min_font, YuanerSet->inter_semibold_min_font->FontSize, parent_window->DC.CursorPos + ImVec2(7 * DPI, 4 * DPI), gui->get_clr(clr->accent), name);//clr->child_label

    // Begin into window
    const bool ret = Begin(temp_window_name, NULL, window_flags);

    // Restore style
    g.Style.ChildBorderSize = backup_border_size;
    if (child_flags & ImGuiChildFlags_FrameStyle)
    {
        PopStyleVar(3);
        PopStyleColor();
    }

    ImGuiWindow* child_window = g.CurrentWindow;
    child_window->ChildId = id;
    state = child_window->ContentSize.y + (GetStyle().WindowPadding.y * 2);

    // Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
    // While this is not really documented/defined, it seems that the expected thing to do.
    if (child_window->BeginCount == 1)
        parent_window->DC.CursorPos = child_window->Pos;

    // Process navigation-in immediately so NavInit can run on first frame
    // Can enter a child if (A) it has navigable items or (B) it can be scrolled.
    const ImGuiID temp_id_for_activation = ImHashStr("##Child", 0, id);
    if (g.ActiveId == temp_id_for_activation)
        ClearActiveID();
    if (g.NavActivateId == id && !(window_flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
    {
        FocusWindow(child_window);
        NavInitWindow(child_window, false);
        SetActiveID(temp_id_for_activation, child_window); // Steal ActiveId with another arbitrary id so that key-press won't activate child item
        g.ActiveIdSource = g.NavInputSource;
    }
    return ret;
}

bool c_widgets::begin_child(std::string_view name, const ImVec2& size_arg)
{
    ImGuiID id = GetCurrentWindow()->GetID(name.data());
    PushStyleVar(ImGuiStyleVar_WindowPadding, YuanerSet->widgets_padding);
    PushStyleVar(ImGuiStyleVar_ItemSpacing, YuanerSet->widgets_spacing);
    return begin_child_ex(name.data(), id, size_arg, ImGuiChildFlags_None, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
}

void c_widgets::end_child()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* child_window = g.CurrentWindow;

    IM_ASSERT(g.WithinEndChild == false);
    IM_ASSERT(child_window->Flags & ImGuiWindowFlags_ChildWindow);   // Mismatched BeginChild()/EndChild() calls

    g.WithinEndChild = true;
    ImVec2 child_size = child_window->Size;
    ImGui::End();
    if (child_window->BeginCount == 1)
    {
        ImGuiWindow* parent_window = g.CurrentWindow;
        ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + child_size);
        ItemSize(child_size);
        if ((child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY) && !(child_window->Flags & ImGuiWindowFlags_NavFlattened))
        {
            ItemAdd(bb, child_window->ChildId);
            RenderNavHighlight(bb, child_window->ChildId);

            // When browsing a window that has no activable items (scroll only) we keep a highlight on the child (pass g.NavId to trick into always displaying)
            if (child_window->DC.NavLayersActiveMask == 0 && child_window == g.NavWindow)
                RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_Compact);
        }
        else
        {
            // Not navigable into
            ItemAdd(bb, 0);

            // But when flattened we directly reach items, adjust active layer mask accordingly
            if (child_window->Flags & ImGuiWindowFlags_NavFlattened)
                parent_window->DC.NavLayersActiveMaskNext |= child_window->DC.NavLayersActiveMaskNext;
        }
        if (g.HoveredWindow == child_window)
            g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
    }
    PopStyleVar(2);
    g.WithinEndChild = false;
    g.LogLinePosY = -FLT_MAX; // To enforce a carriage return
    SetCursorPosY(GetCursorPosY() - 10);
}

bool c_widgets::checkbox(std::string_view label, bool* callback, bool popup)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(label.data(), label.data() + label.size());

    if (!clr || !YuanerSet || !YuanerSet->inter_semibold_font || !YuanerSet->inter_semibold_min_font)
        return false;

    const float height = 30.f * DPI;
    const float width = GetWindowWidth();
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect rect(pos, pos + ImVec2(width, height));
    
    const float switch_width = 40.f * DPI;
    const float switch_height = 20.f * DPI;
    const float switch_offset_x = 15.f * DPI;
    const ImRect switch_rect(pos + ImVec2(width - switch_width - switch_offset_x, (height - switch_height) * 0.5f), 
                              pos + ImVec2(width - switch_offset_x, (height + switch_height) * 0.5f));
    
    ItemSize(rect, 0);
    if (!ItemAdd(rect, id))
        return false;
    
    bool hovered, held;
    bool pressed = ButtonBehavior(rect, id, &hovered, &held);
    if (pressed)
    {
        *callback = !(*callback);
        MarkItemEdited(id);
    }
    
    struct checkbox_state
    {
        ImVec4 label_clr;
        ImVec4 circle_clr;
        float circle_offset = 0.f;
        float alpha = -1.f;
    };

    static std::map<ImGuiID, checkbox_state> anim;
    checkbox_state& state = anim[id];
    
    if (state.alpha < 0.f) {
        state.label_clr = clr->selection_label;
        state.circle_clr = clr->selection_icon;
        state.circle_offset = *callback ? ((switch_width / 2) - ((switch_height - 4.f * DPI) / 2) - 2.f * DPI) : (-(switch_width / 2) + ((switch_height - 4.f * DPI) / 2) + 3.f * DPI);
        state.alpha = *callback ? 0.3f : 0.f;
    }
    
    float tickrate = 8.f;
    float circle_radius = (switch_height - 4.f * DPI) / 2;
    float circle_max_offset = (switch_width / 2) - circle_radius - 2.f * DPI;
    
    float target_offset = *callback ? circle_max_offset : -circle_max_offset + 3.f * DPI;
    float speed = gui->fixed_speed(tickrate);

    state.circle_offset = ImLerp(state.circle_offset, target_offset, speed);
    state.circle_clr = ImLerp(state.circle_clr, *callback ? clr->accent : clr->selection_icon, speed);
    state.label_clr = ImLerp(state.label_clr, *callback ? clr->accent : (hovered ? clr->accent : clr->selection_label), speed);
    state.alpha = ImLerp(state.alpha, *callback ? 0.3f : 0.f, speed);
    
    float rounding = 10.f * DPI;
    window->DrawList->AddRectFilled(switch_rect.Min, switch_rect.Max, 
        gui->get_clr(clr->selection_background, 0.5f), rounding);
    
    if (state.alpha > 0.01f) {
        window->DrawList->AddRectFilled(switch_rect.Min, switch_rect.Max, 
            gui->get_clr(clr->accent, state.alpha), rounding);
    }
    
    ImVec2 circle_center = switch_rect.GetCenter() + ImVec2(state.circle_offset, 0);
    window->DrawList->AddCircleFilled(circle_center, circle_radius, 
        gui->get_clr(state.circle_clr), 32);
    
    ImFont* font = popup ? YuanerSet->inter_semibold_font : YuanerSet->inter_semibold_min_font;
    float text_y = rect.Min.y + (height - font->FontSize) * 0.5f;
    window->DrawList->AddText(font, font->FontSize, ImVec2(rect.Min.x, text_y), 
        gui->get_clr(state.label_clr), label.data(), label.data() + label.size());
    
    return pressed;
}

bool c_widgets::PopUpCheckbox(std::string_view Icon, std::string_view label, bool* callback, bool popup)
{
    struct checkbox_state
    {
        ImVec4 label_clr = clr->checkbox_label;
        float alpha = 0.f;
        float offset = 3.f;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const ImVec2 rect_size = ImVec2(24 * DPI, 24 * DPI);
    const float width = GetWindowWidth() - rect_size.x;
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect rect(pos, pos + ImVec2(width, rect_size.y));
    ItemSize(rect, style.FramePadding.y);
    if (!ItemAdd(rect, id))
    {
        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*callback ? ImGuiItemStatusFlags_Checked : 0));
        return false;
    }

    // 限制交互范围到 checkbox 本身
    ImRect checkbox_rect(rect.Max - rect_size, rect.Max);
    bool hovered, held;
    bool pressed = ButtonBehavior(checkbox_rect, id, &hovered, &held);
    if (pressed)
    {
        *callback = !(*callback);
        MarkItemEdited(id);
    }

    static std::map<ImGuiID, checkbox_state> anim;
    checkbox_state& state = anim[id];

    state.offset = ImLerp(state.offset, *callback ? (0.f) : (3.f * DPI), gui->fixed_speed(8.f));
    state.alpha = ImClamp(state.alpha + (gui->fixed_speed(4.f) * (*callback ? 1.f : -1.f)), 0.f, 1.f);

    state.label_clr = ImLerp(state.label_clr, *callback ? clr->checkbox_label_active : clr->checkbox_label, gui->fixed_speed(4.f));


    PushFont(popup ? YuanerSet->inter_semibold_font : YuanerSet->inter_semibold_min_font);
    window->DrawList->AddText(rect.Min + ImVec2(0, popup ? 4 * DPI : 1 * DPI), gui->get_clr(state.label_clr), label.data());
    PopFont();

    window->DrawList->AddRectFilled(checkbox_rect.Min, checkbox_rect.Max, gui->get_clr(clr->checkbox_background), YuanerSet->widgets_rounding);
    window->DrawList->AddRectFilled(checkbox_rect.Min - ImVec2(state.offset, state.offset), checkbox_rect.Max + ImVec2(state.offset, state.offset), gui->get_clr(clr->accent, state.alpha), YuanerSet->widgets_rounding);
    RenderCheckMark(window->DrawList, checkbox_rect.Min + ImVec2(4 * DPI, 4 * DPI), gui->get_clr(clr->checkbox_checkmark, state.alpha), 16.f * DPI);

    PushFont(popup ? YuanerSet->icon : YuanerSet->icon);
    window->DrawList->AddText(rect.Min + ImVec2(checkbox_rect.Min.x - YuanerSet->icon->FontSize, popup ? 4 * DPI : 1 * DPI), gui->get_clr(state.label_clr), Icon.data());
    PopFont();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*callback ? ImGuiItemStatusFlags_Checked : 0));
    return pressed;
}

bool popup_selection(std::string_view label, std::string_view icon, bool active)
{
    struct popup_selection_state
    {
        float background_alpha = 0.f;
        ImVec4 icon_clr = ImVec4(1.f, 1.f, 1.f, 1.f);
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, pos + ImVec2(161 * DPI, 30 * DPI));

    ItemSize(rect, 0);
    if (!ItemAdd(rect, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(rect, id, &hovered, &held);
    if (pressed)
        MarkItemEdited(id);

    static std::map<ImGuiID, popup_selection_state> anim;
    popup_selection_state& state = anim[id];

    state.background_alpha = ImClamp(state.background_alpha + (gui->fixed_speed(4.f) * (active ? 1.f : -1.f)), 0.f, 1.f);
    state.icon_clr = ImLerp(state.icon_clr, active ? clr->accent : ImVec4(1.f, 1.f, 1.f, 1.f), gui->fixed_speed(8.f));

    window->DrawList->AddRectFilled(rect.Min, rect.Max, gui->get_clr(clr->popup_selection_background, state.background_alpha), YuanerSet->widgets_rounding);

    window->DrawList->AddText(YuanerSet->inter_semibold_font, YuanerSet->inter_semibold_font->FontSize, rect.Min + ImVec2(25 * DPI, 7 * DPI), gui->get_clr(clr->popup_selection_label), label.data());
    //前面的图标
    window->DrawList->AddText(YuanerSet->icon, YuanerSet->icon->FontSize, rect.Min + ImVec2(3 * DPI, 8 * DPI), gui->get_clr(state.icon_clr), icon.data());
    //小箭头
    if (icon.data() != "C") window->DrawList->AddText(YuanerSet->icon, YuanerSet->icon->FontSize, rect.Max - ImVec2(20 * DPI, 22 * DPI), gui->get_clr(state.icon_clr), "\uEC55");


    return pressed;
}

bool popup_keybind(const char* label, int* key)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;

    const ImGuiID id = window->GetID(label);

    const ImVec2 pos = window->DC.CursorPos;
    const float width = GetWindowWidth() - g.Style.WindowPadding.x * 2;
    const ImRect rect(pos, pos + ImVec2(width, 25 * DPI));
    ImGui::ItemSize(rect, style.FramePadding.y);
    if (!ImGui::ItemAdd(rect, id, &rect))
        return false;

    char buf_display[64] = "...";

    bool value_changed = false;
    int k = *key;

    std::string active_key = "";
    active_key += keys[*key];

    if (*key != 0 && g.ActiveId != id) {
        strcpy(buf_display, active_key.c_str());
    }
    else if (g.ActiveId == id) {
        strcpy(buf_display, "...");
    }

    bool hovered = ItemHoverable(rect, id, 0);

    if (hovered && io.MouseClicked[0])
    {
        if (g.ActiveId != id) {
            // Start edition
            memset(io.MouseDown, 0, sizeof(io.MouseDown));
            memset(io.KeysDown, 0, sizeof(io.KeysDown));
            *key = 0;
        }
        ImGui::SetActiveID(id, window);
        ImGui::FocusWindow(window);
    }
    else if (io.MouseClicked[0]) {
        // Release focus when we click outside
        if (g.ActiveId == id)
            ImGui::ClearActiveID();
    }

    if (g.ActiveId == id) {
        for (auto i = 0; i < 5; i++) {
            if (io.MouseDown[i]) {
                switch (i) {
                case 0:
                    k = 0x01;
                    break;
                case 1:
                    k = 0x02;
                    break;
                case 2:
                    k = 0x04;
                    break;
                case 3:
                    k = 0x05;
                    break;
                case 4:
                    k = 0x06;
                    break;
                }
                value_changed = true;
                ImGui::ClearActiveID();
            }
        }
        if (!value_changed) {
            for (auto i = 0x08; i <= 0xA5; i++) {
                if (io.KeysDown[i]) {
                    k = i;
                    value_changed = true;
                    ImGui::ClearActiveID();
                }
            }
        }

        if (io.KeysDown[ImGuiKey_Escape]) { *key = 0; ImGui::ClearActiveID(); }
        else {
            *key = k;
        }
    }

    window->DrawList->AddRectFilled(rect.Min, rect.Max, gui->get_clr(clr->popup_key_background), YuanerSet->widgets_rounding);
    //三个小点
    window->DrawList->AddText(YuanerSet->inter_semibold_min_font, YuanerSet->inter_semibold_min_font->FontSize, rect.Min + ImVec2(7 * DPI, 4 * DPI), gui->get_clr(clr->accent), buf_display);

    return value_changed;
}

bool popup_selectable(const char* label, bool active)
{
    struct selectable_state
    {
        float alpha = 0.f;
        float offset = 0.f;
        ImVec4 color = clr->selectable_inactive;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 pos = window->DC.CursorPos;
    const ImRect rect(pos, pos + ImVec2(122 * DPI, 21 * DPI));
    ItemSize(rect, style.FramePadding.y);
    if (!ItemAdd(rect, id))
        return false;

    bool hovered = IsItemHovered();
    bool pressed = hovered && g.IO.MouseClicked[0];
    if (pressed)
        MarkItemEdited(id);

    static std::map<ImGuiID, selectable_state> anim;
    selectable_state& state = anim[id];

    state.alpha = ImClamp(state.alpha + (gui->fixed_speed(6.f) * (active ? 1.f : -1.f)), 0.f, 1.f);
    state.color = ImLerp(state.color, active ? clr->selectable_active : clr->selectable_inactive, gui->fixed_speed(12.f));
    state.offset = ImLerp(state.offset, active ? (16.f * DPI) : 0.f, gui->fixed_speed(12.f));

    window->DrawList->AddCircleFilled(ImVec2(rect.Min.x + (3 * DPI), rect.GetCenter().y), 3.f * DPI, gui->get_clr(clr->accent, state.alpha));
    window->DrawList->AddText(YuanerSet->inter_semibold_min_font, YuanerSet->inter_semibold_min_font->FontSize, rect.Min + ImVec2(-1 + std::round(state.offset), 1 * DPI), gui->get_clr(state.color), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

bool combo_popup(ImGuiID popup_id, const ImRect& bb, ImGuiComboFlags flags)
{
    ImGuiContext& g = *GImGui;
    if (!IsPopupOpen(popup_id, ImGuiPopupFlags_None))
    {
        g.NextWindowData.ClearFlags();
        return false;
    }

    // Set popup size
    float w = bb.GetWidth();
    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint)
    {
        g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
    }
    else
    {
        if ((flags & ImGuiComboFlags_HeightMask_) == 0)
            flags |= ImGuiComboFlags_HeightRegular;
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_)); // Only one
        int popup_max_height_in_items = -1;
        if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
        else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
        else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
        ImVec2 constraint_min(0.0f, 0.0f), constraint_max(FLT_MAX, FLT_MAX);
        if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize) == 0 || g.NextWindowData.SizeVal.x <= 0.0f) // Don't apply constraints if user specified a size
            constraint_min.x = w;
        if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize) == 0 || g.NextWindowData.SizeVal.y <= 0.0f)
            constraint_max.y = calc_combo_size(popup_max_height_in_items, 25.f * DPI);
        SetNextWindowSizeConstraints(constraint_min, constraint_max);
    }

    // This is essentially a specialized version of BeginPopupEx()
    char name[16];
    ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginComboDepth); // Recycle windows based on depth

    // Set position given a custom constraint (peak into expected window size so we can position it)
    // FIXME: This might be easier to express with an hypothetical SetNextWindowPosConstraints() function?
    // FIXME: This might be moved to Begin() or at least around the same spot where Tooltips and other Popups are calling FindBestWindowPosForPopupEx()?
    if (ImGuiWindow* popup_window = FindWindowByName(name))
        if (popup_window->WasActive)
        {
            // Always override 'AutoPosLastDirection' to not leave a chance for a past value to affect us.
            ImVec2 size_expected = CalcWindowNextAutoFitSize(popup_window);
            popup_window->AutoPosLastDirection = (flags & ImGuiComboFlags_PopupAlignLeft) ? ImGuiDir_Left : ImGuiDir_Down; // Left = "Below, Toward Left", Down = "Below, Toward Right (default)"
            ImRect r_outer = GetPopupAllowedExtentRect(popup_window);
            ImVec2 pos = FindBestWindowPosForPopupEx(bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, bb, ImGuiPopupPositionPolicy_ComboBox);
            SetNextWindowPos(pos + ImVec2(0, 10 * DPI));
        }

    // We don't use BeginPopupEx() solely because we have a custom name string, which we could make an argument to BeginPopupEx()
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding;
    bool ret = Begin(name, NULL, window_flags);
    if (!ret)
    {
        EndPopup();
        IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
        return false;
    }
    g.BeginComboDepth++;
    return true;
}

bool begin_popup_combo(const char* label, const char* preview_value, int val, ImGuiComboFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
    g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together
    if (flags & ImGuiComboFlags_WidthFitPreview)
        IM_ASSERT((flags & (ImGuiComboFlags_NoPreview | (ImGuiComboFlags)ImGuiComboFlags_CustomPreview)) == 0);

    const ImVec2 pos = window->DC.CursorPos;
    const float width = GetWindowWidth() - g.Style.WindowPadding.x * 2;
    const ImRect rect(pos, pos + ImVec2(width, 25 * DPI));
    ItemSize(rect, style.FramePadding.y);
    if (!ItemAdd(rect, id, &rect))
        return false;

    // Open on click
    bool hovered, held;
    bool pressed = ButtonBehavior(rect, id, &hovered, &held);
    const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
    bool popup_open = IsPopupOpen(popup_id, ImGuiPopupFlags_None);
    if (pressed && !popup_open)
    {
        OpenPopupEx(popup_id, ImGuiPopupFlags_None);
        popup_open = true;
    }

    window->DrawList->AddRectFilled(rect.Min, rect.Max, gui->get_clr(clr->combo_background), YuanerSet->widgets_rounding);

    PushFont(YuanerSet->inter_semibold_min_font);
    PushStyleColor(ImGuiCol_Text, gui->get_clr(clr->combo_value));
    RenderTextClipped(rect.Min + ImVec2(8 * DPI, -1), rect.Max - ImVec2(0, 1), preview_value, NULL, NULL, ImVec2(0.0f, 0.5f));
    PopStyleColor();
    PopFont();
//箭头
    window->DrawList->AddText(YuanerSet->icon, YuanerSet->icon->FontSize, rect.Max - ImVec2(17 * DPI, 20 * DPI), gui->get_clr(clr->accent), "\uEC53");

    if (!popup_open)
        return false;

    g.NextWindowData.Flags = backup_next_window_data_flags;

    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
    PushStyleVar(ImGuiStyleVar_PopupRounding, YuanerSet->widgets_rounding);
    PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.f);
    PushStyleColor(ImGuiCol_PopupBg, gui->get_clr(clr->combo_background));

    return combo_popup(popup_id, rect, flags);
}

void end_popup_combo()
{
    ImGuiContext& g = *GImGui;
    EndPopup();
    PopStyleVar(4);
    PopStyleColor();
    g.BeginComboDepth--;
}

bool popup_combo(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items)
{
    ImGuiContext& g = *GImGui;

    // Call the getter to obtain the preview string which is a parameter to BeginCombo()
    const char* preview_value = NULL;
    if (*current_item >= 0 && *current_item < items_count)
        preview_value = getter(user_data, *current_item);

    // The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
    if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
        SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, calc_combo_size(popup_max_height_in_items, 31.f * DPI)));

    if (!begin_popup_combo(label, preview_value, items_count, ImGuiComboFlags_None))
        return false;

    // Display items
    // FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
    bool value_changed = false;
    for (int i = 0; i < items_count; i++)
    {
        const char* item_text = getter(user_data, i);
        if (item_text == NULL)
            item_text = "*Unknown item*";

        PushID(i);
        const bool item_selected = (i == *current_item);
        if (popup_selectable(item_text, item_selected) && *current_item != i)
        {
            value_changed = true;
            *current_item = i;
        }
        if (item_selected)
            SetItemDefaultFocus();
        PopID();
    }

    end_popup_combo();

    if (value_changed)
        MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

bool popup_dropdown(std::string_view label, int* current_item, const char* const items[], int items_count)
{
    const bool value_changed = popup_combo(label.data(), current_item, items_array_getter, (void*)items, items_count, -1);
    return value_changed;
}

bool popup_button(const char* label)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    const float width = GetWindowWidth() - g.Style.WindowPadding.x * 2;
    const ImRect rect(pos, pos + ImVec2(width, 25 * DPI));

    ItemSize(rect, style.FramePadding.y);
    if (!ItemAdd(rect, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(rect, id, &hovered, &held, 0);

    // Render
    //Button 文字
    window->DrawList->AddText(YuanerSet->inter_semibold_min_font, YuanerSet->inter_semibold_min_font->FontSize, rect.Min + ImVec2(25 * DPI, 7 * DPI), gui->get_clr(ImVec4(1.f, 0.6f, 0.6f, 1.f)), label);
    //Button 前面的小图标
    window->DrawList->AddText(YuanerSet->icon, YuanerSet->icon->FontSize, rect.Min + ImVec2(3, 7), gui->get_clr(ImVec4(1.f, 0.6f, 0.6f, 1.f)), "\uEB59");

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

void c_widgets::checkbox_with_popup(std::string_view label, bool* callback, bool keybind, bool palette, std::string_view hint, int* key, int* mode, float color[4], ImGuiColorEditFlags flags)
{
    struct popup_state
    {
        bool opened = false;
        bool hovered[4];
        ImVec2 stored_pos = ImVec2(0, 0);
        bool show = false;

        int page_selector = -1;
    };

    ImGuiWindow* window = GetCurrentWindow();

    ImGuiContext& g = *GImGui;

    gui->PopUpCheckbox("\uEC6A", label, callback);

    const char* mode_types[3] = { "Hold", "Toggle", "Always" };

    PushFont(YuanerSet->inter_semibold_font);
    const ImVec2 hint_size = CalcTextSize(hint.data());
    PopFont();

    static std::map<ImGuiID, popup_state> anim;
    popup_state& state = anim[GetID(label.data())];

    if (g.IO.MouseDown[0] && g.IO.MouseDownDuration[0] > 0.5f) // 长按 0.5 秒
        state.stored_pos = GetMousePos();

    // 检测鼠标拖动（向右滑动）
    if (IsMouseHoveringRect(g.LastItemData.Rect.Min, g.LastItemData.Rect.Max) &&
        g.IO.MouseDownDuration[0] > 0.0f && g.IO.MouseDelta.x > 10.0f)
    {
        state.opened = true;
    }
    else if (state.opened && !IsMouseHoveringRect(g.LastItemData.Rect.Min, g.LastItemData.Rect.Max) &&
             g.IO.MouseDownDuration[0] > 0.0f && g.IO.MouseDelta.x > 10.0f)
    {
        state.opened = false;
    }

    if (state.opened)
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

        PushStyleColor(ImGuiCol_WindowBg, gui->get_clr(clr->popup_background));
        PushStyleVar(ImGuiStyleVar_WindowRounding, YuanerSet->widgets_rounding);
        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
        SetNextWindowPos(state.stored_pos);
        SetNextWindowSize(ImVec2(161 * DPI, calc_combo_size(keybind + palette + (!hint.empty() ? 1 : 0) + 1, 30.f * DPI) + g.Style.WindowPadding.x * 2));
        Begin((std::stringstream{} << label.data() << "window").str().c_str(), NULL, window_flags);
        {
            state.hovered[0] = IsWindowHovered();
            if (keybind)
                if (popup_selection("创建绑定", "\uECD2", state.page_selector == 0))//
                    state.page_selector = 0;
            if (palette)
                if (popup_selection("调色板", "\uED17", state.page_selector == 1))//
                    state.page_selector = 1;
            if (!hint.empty())
                if (popup_selection("描述", "\uECA3", state.page_selector == 2))//
                    state.page_selector = 2;
            PushID((std::stringstream{} << label.data() << "delete").str().c_str());
            popup_button("Reset");
            PopID();

            if (state.page_selector == 0)
            {
                SetNextWindowPos(state.stored_pos + ImVec2(171 * DPI, 20 * DPI));
                SetNextWindowSize(ImVec2(162 * DPI, 98 * DPI));
                Begin((std::stringstream{} << label.data() << "bind").str().c_str(), NULL, window_flags);
                {
                    state.hovered[1] = IsWindowHovered();
                    popup_keybind((std::stringstream{} << label.data() << "key").str().c_str(), key);
                    popup_dropdown((std::stringstream{} << label.data() << "mode").str().c_str(), mode, mode_types, 3);
                    PushID((std::stringstream{} << label.data() << "show").str().c_str());
                    gui->checkbox("绑定", &state.show, true);
                    PopID();
                }
                ImGui::End();
            }
            else if (state.page_selector == 1)
            {
                SetNextWindowPos(state.stored_pos + ImVec2(171 * DPI, !keybind ? (20 * DPI) : (45 * DPI)));
                SetNextWindowSize(ImVec2(162 * DPI, 28 * DPI));
                Begin((std::stringstream{} << label.data() << "palette").str().c_str(), NULL, window_flags);
                {
                    state.hovered[2] = IsWindowHovered();
                    PushID((std::stringstream{} << label.data() << "color").str().c_str());
                    gui->color_edit("Color", color, flags, true);
                    PopID();
                }
                ImGui::End();
            }
            else if (state.page_selector == 2)
            {
                SetNextWindowPos(state.stored_pos + ImVec2(171 * DPI, (!keybind && !palette) ? (20 * DPI) : (!keybind  && palette) || (keybind && !palette) ? (45 * DPI) : (70 * DPI)));
                SetNextWindowSize(hint_size + ImVec2(20 * DPI, 20 * DPI));
                Begin((std::stringstream{} << label.data() << "hint").str().c_str(), NULL, window_flags);
                {
                    state.hovered[3] = IsWindowHovered();
                    PushFont(YuanerSet->inter_semibold_font);
                    Text("%s",hint.data());
                    PopFont();
                }
                ImGui::End();
            }
        }
        ImGui::End();

        PopStyleColor();
        PopStyleVar(3);
    }
}




template<typename TYPE, typename SIGNEDTYPE, typename FLOATTYPE>
bool slider_behavior_t(const ImRect& bb, ImGuiID id, ImGuiDataType data_type, TYPE* v, const TYPE v_min, const TYPE v_max, const char* format, ImGuiSliderFlags flags, ImRect* out_grab_bb)
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImGuiAxis axis = (flags & ImGuiSliderFlags_Vertical) ? ImGuiAxis_Y : ImGuiAxis_X;
    const bool is_logarithmic = (flags & ImGuiSliderFlags_Logarithmic) != 0;
    const bool is_floating_point = (data_type == ImGuiDataType_Float) || (data_type == ImGuiDataType_Double);
    const float v_range_f = (float)(v_min < v_max ? v_max - v_min : v_min - v_max); // We don't need high precision for what we do with it.

    // Calculate bounds
    const float grab_padding = 2.0f; // FIXME: Should be part of style.
    const float slider_sz = (bb.Max[axis] - bb.Min[axis]) - grab_padding * 2.0f;
    float grab_sz = 13.f;
    const float slider_usable_sz = slider_sz - grab_sz;
    const float slider_usable_pos_min = bb.Min[axis] + grab_padding + grab_sz * 0.5f;
    const float slider_usable_pos_max = bb.Max[axis] - grab_padding - grab_sz * 0.5f;

    float logarithmic_zero_epsilon = 0.0f; // Only valid when is_logarithmic is true
    float zero_deadzone_halfsize = 0.0f; // Only valid when is_logarithmic is true
    if (is_logarithmic)
    {
        // When using logarithmic sliders, we need to clamp to avoid hitting zero, but our choice of clamp value greatly affects slider precision. We attempt to use the specified precision to estimate a good lower bound.
        const int decimal_precision = is_floating_point ? ImParseFormatPrecision(format, 3) : 1;
        logarithmic_zero_epsilon = ImPow(0.1f, (float)decimal_precision);
        zero_deadzone_halfsize = (style.LogSliderDeadzone * 0.5f) / ImMax(slider_usable_sz, 1.0f);
    }

    // Process interacting with the slider
    bool value_changed = false;
    if (g.ActiveId == id)
    {
        bool set_new_value = false;
        float clicked_t = 0.0f;
        if (g.ActiveIdSource == ImGuiInputSource_Mouse)
        {
            if (!g.IO.MouseDown[0])
            {
                ClearActiveID();
            }
            else
            {
                const float mouse_abs_pos = g.IO.MousePos[axis];
                if (g.ActiveIdIsJustActivated)
                {
                    float grab_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);
                    if (axis == ImGuiAxis_Y)
                        grab_t = 1.0f - grab_t;
                    const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
                    const bool clicked_around_grab = (mouse_abs_pos >= grab_pos - grab_sz * 0.5f - 1.0f) && (mouse_abs_pos <= grab_pos + grab_sz * 0.5f + 1.0f); // No harm being extra generous here.
                    g.SliderGrabClickOffset = (clicked_around_grab && is_floating_point) ? mouse_abs_pos - grab_pos : 0.0f;
                }
                if (slider_usable_sz > 0.0f)
                    clicked_t = ImSaturate((mouse_abs_pos - g.SliderGrabClickOffset - slider_usable_pos_min) / slider_usable_sz);
                if (axis == ImGuiAxis_Y)
                    clicked_t = 1.0f - clicked_t;
                set_new_value = true;
            }
        }
        else if (g.ActiveIdSource == ImGuiInputSource_Keyboard || g.ActiveIdSource == ImGuiInputSource_Gamepad)
        {
            if (g.ActiveIdIsJustActivated)
            {
                g.SliderCurrentAccum = 0.0f; // Reset any stored nav delta upon activation
                g.SliderCurrentAccumDirty = false;
            }

            float input_delta = (axis == ImGuiAxis_X) ? GetNavTweakPressedAmount(axis) : -GetNavTweakPressedAmount(axis);
            if (input_delta != 0.0f)
            {
                const bool tweak_slow = IsKeyDown((g.NavInputSource == ImGuiInputSource_Gamepad) ? ImGuiKey_NavGamepadTweakSlow : ImGuiKey_NavKeyboardTweakSlow);
                const bool tweak_fast = IsKeyDown((g.NavInputSource == ImGuiInputSource_Gamepad) ? ImGuiKey_NavGamepadTweakFast : ImGuiKey_NavKeyboardTweakFast);
                const int decimal_precision = is_floating_point ? ImParseFormatPrecision(format, 3) : 0;
                if (decimal_precision > 0)
                {
                    input_delta /= 100.0f;    // Gamepad/keyboard tweak speeds in % of slider bounds
                    if (tweak_slow)
                        input_delta /= 10.0f;
                }
                else
                {
                    if ((v_range_f >= -100.0f && v_range_f <= 100.0f && v_range_f != 0.0f) || tweak_slow)
                        input_delta = ((input_delta < 0.0f) ? -1.0f : +1.0f) / v_range_f; // Gamepad/keyboard tweak speeds in integer steps
                    else
                        input_delta /= 100.0f;
                }
                if (tweak_fast)
                    input_delta *= 10.0f;

                g.SliderCurrentAccum += input_delta;
                g.SliderCurrentAccumDirty = true;
            }

            float delta = g.SliderCurrentAccum;
            if (g.NavActivatePressedId == id && !g.ActiveIdIsJustActivated)
            {
                ClearActiveID();
            }
            else if (g.SliderCurrentAccumDirty)
            {
                clicked_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);

                if ((clicked_t >= 1.0f && delta > 0.0f) || (clicked_t <= 0.0f && delta < 0.0f)) // This is to avoid applying the saturation when already past the limits
                {
                    set_new_value = false;
                    g.SliderCurrentAccum = 0.0f; // If pushing up against the limits, don't continue to accumulate
                }
                else
                {
                    set_new_value = true;
                    float old_clicked_t = clicked_t;
                    clicked_t = ImSaturate(clicked_t + delta);

                    // Calculate what our "new" clicked_t will be, and thus how far we actually moved the slider, and subtract this from the accumulator
                    TYPE v_new = ScaleValueFromRatioT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, clicked_t, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);
                    if (is_floating_point && !(flags & ImGuiSliderFlags_NoRoundToFormat))
                        v_new = RoundScalarWithFormatT<TYPE>(format, data_type, v_new);
                    float new_clicked_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, v_new, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);

                    if (delta > 0)
                        g.SliderCurrentAccum -= ImMin(new_clicked_t - old_clicked_t, delta);
                    else
                        g.SliderCurrentAccum -= ImMax(new_clicked_t - old_clicked_t, delta);
                }

                g.SliderCurrentAccumDirty = false;
            }
        }

        if (set_new_value)
            if ((g.LastItemData.InFlags & ImGuiItemFlags_ReadOnly) || (flags & ImGuiSliderFlags_ReadOnly))
                set_new_value = false;

        if (set_new_value)
        {
            TYPE v_new = ScaleValueFromRatioT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, clicked_t, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);

            // Round to user desired precision based on format string
            if (is_floating_point && !(flags & ImGuiSliderFlags_NoRoundToFormat))
                v_new = RoundScalarWithFormatT<TYPE>(format, data_type, v_new);

            // Apply result
            if (*v != v_new)
            {
                *v = v_new;
                value_changed = true;
            }
        }
    }

    if (slider_sz < 1.0f)
    {
        *out_grab_bb = ImRect(bb.Min, bb.Min);
    }
    else
    {
        // Output grab position so it can be displayed by the caller
        float grab_t = ScaleRatioFromValueT<TYPE, SIGNEDTYPE, FLOATTYPE>(data_type, *v, v_min, v_max, is_logarithmic, logarithmic_zero_epsilon, zero_deadzone_halfsize);
        if (axis == ImGuiAxis_Y)
            grab_t = 1.0f - grab_t;
        const float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
        if (axis == ImGuiAxis_X)
            *out_grab_bb = ImRect(grab_pos - grab_sz * 0.5f, bb.Min.y + grab_padding, grab_pos + grab_sz * 0.5f, bb.Max.y - grab_padding);
        else
            *out_grab_bb = ImRect(bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f, bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f);
    }

    return value_changed;}

// For 32-bit and larger types, slider bounds are limited to half the natural type range.
// So e.g. an integer Slider between INT_MAX-10 and INT_MAX will fail, but an integer Slider between INT_MAX/2-10 and INT_MAX/2 will be ok.
// It would be possible to lift that limitation with some work but it doesn't seem to be worth it for sliders.
bool slider_behavior(const ImRect& bb, ImGuiID id, ImGuiDataType data_type, void* p_v, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags, ImRect* out_grab_bb)
{
    // Read imgui.cpp "API BREAKING CHANGES" section for 1.78 if you hit this assert.
    IM_ASSERT((flags == 1 || (flags & ImGuiSliderFlags_InvalidMask_) == 0) && "Invalid ImGuiSliderFlags flag!  Has the 'float power' argument been mistakenly cast to flags? Call function with ImGuiSliderFlags_Logarithmic flags instead.");

    switch (data_type)
    {
    case ImGuiDataType_S8:  { ImS32 v32 = (ImS32)*(ImS8*)p_v;  bool r = slider_behavior_t<ImS32, ImS32, float>(bb, id, ImGuiDataType_S32, &v32, *(const ImS8*)p_min,  *(const ImS8*)p_max,  format, flags, out_grab_bb); if (r) *(ImS8*)p_v  = (ImS8)v32;  return r; }
    case ImGuiDataType_U8:  { ImU32 v32 = (ImU32)*(ImU8*)p_v;  bool r = slider_behavior_t<ImU32, ImS32, float>(bb, id, ImGuiDataType_U32, &v32, *(const ImU8*)p_min,  *(const ImU8*)p_max,  format, flags, out_grab_bb); if (r) *(ImU8*)p_v  = (ImU8)v32;  return r; }
    case ImGuiDataType_S16: { ImS32 v32 = (ImS32)*(ImS16*)p_v; bool r = slider_behavior_t<ImS32, ImS32, float>(bb, id, ImGuiDataType_S32, &v32, *(const ImS16*)p_min, *(const ImS16*)p_max, format, flags, out_grab_bb); if (r) *(ImS16*)p_v = (ImS16)v32; return r; }
    case ImGuiDataType_U16: { ImU32 v32 = (ImU32)*(ImU16*)p_v; bool r = slider_behavior_t<ImU32, ImS32, float>(bb, id, ImGuiDataType_U32, &v32, *(const ImU16*)p_min, *(const ImU16*)p_max, format, flags, out_grab_bb); if (r) *(ImU16*)p_v = (ImU16)v32; return r; }
    case ImGuiDataType_S32:
        IM_ASSERT(*(const ImS32*)p_min >= INT_MIN / 2 && *(const ImS32*)p_max <= INT_MAX / 2);
        return slider_behavior_t<ImS32, ImS32, float >(bb, id, data_type, (ImS32*)p_v,  *(const ImS32*)p_min,  *(const ImS32*)p_max,  format, flags, out_grab_bb);
    case ImGuiDataType_U32:
        IM_ASSERT(*(const ImU32*)p_max <= UINT_MAX / 2);
        return slider_behavior_t<ImU32, ImS32, float >(bb, id, data_type, (ImU32*)p_v,  *(const ImU32*)p_min,  *(const ImU32*)p_max,  format, flags, out_grab_bb);
    case ImGuiDataType_S64:
        IM_ASSERT(*(const ImS64*)p_min >= LLONG_MIN / 2 && *(const ImS64*)p_max <= LLONG_MAX / 2);
        return slider_behavior_t<ImS64, ImS64, double>(bb, id, data_type, (ImS64*)p_v,  *(const ImS64*)p_min,  *(const ImS64*)p_max,  format, flags, out_grab_bb);
    case ImGuiDataType_U64:
        IM_ASSERT(*(const ImU64*)p_max <= ULLONG_MAX / 2);
        return slider_behavior_t<ImU64, ImS64, double>(bb, id, data_type, (ImU64*)p_v,  *(const ImU64*)p_min,  *(const ImU64*)p_max,  format, flags, out_grab_bb);
    case ImGuiDataType_Float:
        IM_ASSERT(*(const float*)p_min >= -FLT_MAX / 2.0f && *(const float*)p_max <= FLT_MAX / 2.0f);
        return slider_behavior_t<float, float, float >(bb, id, data_type, (float*)p_v,  *(const float*)p_min,  *(const float*)p_max,  format, flags, out_grab_bb);
    case ImGuiDataType_Double:
        IM_ASSERT(*(const double*)p_min >= -DBL_MAX / 2.0f && *(const double*)p_max <= DBL_MAX / 2.0f);
        return slider_behavior_t<double, double, double>(bb, id, data_type, (double*)p_v, *(const double*)p_min, *(const double*)p_max, format, flags, out_grab_bb);
    case ImGuiDataType_COUNT: break;
    }
    IM_ASSERT(0);
    return false;
}

bool slider_scalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    struct slider_state
    {
        float grab_slide = 0.f;
        float alpha = 0.f;
        float slow_value = 0.f;
    };
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const float width = GetWindowWidth() - g.Style.WindowPadding.x * 2;
    const float slider_height = 8.f * DPI;
    const float padding = 15.f * DPI;
    const ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, pos + ImVec2(width, 65.f * DPI));
    const ImRect slider_rect(pos + ImVec2(0, rect.GetHeight() - slider_height - padding), 
                              pos + ImVec2(width, rect.GetHeight() - padding));

    const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
    ItemSize(rect, style.FramePadding.y);
    if (!ItemAdd(rect, id, &slider_rect, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
        return false;

    // Default format string when passing NULL
    if (format == NULL)
        format = DataTypeGetInfo(data_type)->PrintFmt;

    const bool hovered = ItemHoverable(slider_rect, id, g.LastItemData.InFlags);
    bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
    if (!temp_input_is_active)
    {
        // Tabbing or CTRL-clicking on Slider turns it into an input box
        const bool clicked = hovered && IsMouseClicked(0, id);
        const bool make_active = (clicked || g.NavActivateId == id);
        if (make_active && clicked)
            SetKeyOwner(ImGuiKey_MouseLeft, id);
        if (make_active && temp_input_allowed)
            if ((clicked && g.IO.KeyCtrl) || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
                temp_input_is_active = true;

        if (make_active && !temp_input_is_active)
        {
            SetActiveID(id, window);
            SetFocusID(id, window);
            FocusWindow(window);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
        }
    }

    // Slider behavior
    ImRect grab_bb;
    const bool value_changed = slider_behavior(ImRect(slider_rect.Min, slider_rect.Max), id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
    if (value_changed)
        MarkItemEdited(id);

    // Display value using user-provided display format
    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

    static std::map<ImGuiID, slider_state> anim;
    slider_state& state = anim[id];

    state.grab_slide = ImLerp(state.grab_slide, grab_bb.Min.x - slider_rect.Min.x, gui->fixed_speed(12.f));
    state.alpha = ImLerp(state.alpha, hovered ? 1.f : 0.f, gui->fixed_speed(12.f));

    // 获取当前值用于显示动画 (只处理 float 类型)
    if (data_type == ImGuiDataType_Float) {
        float* p_val = (float*)p_data;
        float target = *p_val;
        float step = (target - state.slow_value) * gui->fixed_speed(12.f);
        if (step == 0 && state.slow_value != target) step = (target > state.slow_value) ? 1 : -1;
        state.slow_value += step;
    } else {
        // 对于其他类型，直接使用当前值
        if (data_type == ImGuiDataType_S32 || data_type == ImGuiDataType_U32 || data_type == ImGuiDataType_S64 || data_type == ImGuiDataType_U64) {
            int* p_val = (int*)p_data;
            int target = *p_val;
            float step = (target - state.slow_value) * gui->fixed_speed(12.f);
            if (step == 0 && state.slow_value != target) step = (target > state.slow_value) ? 1 : -1;
            state.slow_value += step;
        } else {
            state.slow_value = *(float*)p_data;
        }
    }

    if (std::fabs(state.slow_value) < std::numeric_limits<float>::epsilon()) {
        state.slow_value = 0.f;
    }

    // 重新格式化显示值
    char slow_buf[64];
    if (data_type == ImGuiDataType_Float) {
        snprintf(slow_buf, sizeof(slow_buf), format, state.slow_value);
    } else {
        snprintf(slow_buf, sizeof(slow_buf), format, (int)state.slow_value);
    }

    // 绘制滑动条背景
    float rounding = 4.f * DPI;
    window->DrawList->AddRectFilled(slider_rect.Min, slider_rect.Max, 
        gui->get_clr(clr->slider_background), rounding);

    // 绘制渐变填充（从半透明到实色）
    float filled_width = state.grab_slide + 13.f * DPI;
    ImRect filled_rect(slider_rect.Min, ImVec2(slider_rect.Min.x + filled_width, slider_rect.Max.y));
    
    // 修复 AddRectFilledMultiColor 参数（不需要 rounding 参数）
    window->DrawList->AddRectFilledMultiColor(filled_rect.Min, filled_rect.Max,
        gui->get_clr(clr->accent, 0.5f),
        gui->get_clr(clr->accent),
        gui->get_clr(clr->accent),
        gui->get_clr(clr->accent, 0.5f));

    // 绘制滑块手柄
    float handle_width = 14.f * DPI;
    float handle_height = 8.f * DPI;
    ImRect handle_rect(ImVec2(slider_rect.Min.x + state.grab_slide, slider_rect.GetCenter().y - handle_height / 2),
                       ImVec2(slider_rect.Min.x + state.grab_slide + handle_width, slider_rect.GetCenter().y + handle_height / 2));
    window->DrawList->AddRectFilled(handle_rect.Min, handle_rect.Max, 
        gui->get_clr(clr->slider_circle), rounding);

    // 绘制标签和值
    window->DrawList->AddText(YuanerSet->inter_semibold_min_font, YuanerSet->inter_semibold_min_font->FontSize, 
        rect.Min + ImVec2(0, padding), gui->get_clr(clr->slider_label), label);
    
    // 显示缓慢变化的值
    float text_width = YuanerSet->inter_semibold_min_font->CalcTextSizeA(YuanerSet->inter_semibold_min_font->FontSize, FLT_MAX, -1.f, slow_buf).x;
    window->DrawList->AddText(YuanerSet->inter_semibold_min_font, YuanerSet->inter_semibold_min_font->FontSize, 
        rect.Min + ImVec2(rect.GetWidth() - text_width, padding), 
        gui->get_clr(clr->accent), slow_buf);

    return value_changed;
}

bool c_widgets::slider_float(std::string_view label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return slider_scalar(label.data(), ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}

bool c_widgets::slider_int(std::string_view label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return slider_scalar(label.data(), ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}

bool selectable_ex(const char* label, bool active)
{
    struct selectable_state
    {
        float alpha = 0.f;
        float offset = 0.f;
        ImVec4 color = clr->selectable_inactive;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const float width = GetContentRegionMax().x - 2 * YuanerSet->combo_spacing;
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect rect(pos, pos + ImVec2(width, 31 * DPI));
    ItemSize(rect, style.FramePadding.y);
    if (!ItemAdd(rect, id))
        return false;

    bool hovered = IsItemHovered();
    bool pressed = hovered && g.IO.MouseClicked[0];
    if (pressed)
        MarkItemEdited(id);

    static std::map<ImGuiID, selectable_state> anim;
    selectable_state& state = anim[id];

    state.alpha = ImClamp(state.alpha + (gui->fixed_speed(6.f) * (active ? 1.f : -1.f)), 0.f, 1.f);
    state.color = ImLerp(state.color, active ? clr->selectable_active : clr->selectable_inactive, gui->fixed_speed(6.f));
    state.offset = ImLerp(state.offset, active ? 16.f : 0.f, gui->fixed_speed(6.f));


    window->DrawList->AddCircleFilled(ImVec2(rect.Min.x + 3, rect.GetCenter().y), 3.f * DPI, gui->get_clr(clr->accent, state.alpha));
    window->DrawList->AddText(YuanerSet->inter_semibold_min_font, YuanerSet->inter_semibold_min_font->FontSize, rect.Min + ImVec2(-1 + std::round(state.offset), 7 * DPI), gui->get_clr(state.color), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}

bool selectable(const char* label, bool* p_selected)
{
    if (selectable_ex(label, *p_selected))
    {
        *p_selected = !*p_selected;
        return true;
    }
    return false;
}

bool begin_combo(const char* label, const char* preview_value, int val, ImGuiComboFlags flags, bool multi)
{
    struct combo_state
    {
        bool combo_opened = false;
        bool hovered = false;
        float alpha = 0.f;
        float offset = 0.f;
    };

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
    g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together
    if (flags & ImGuiComboFlags_WidthFitPreview)
        IM_ASSERT((flags & (ImGuiComboFlags_NoPreview | ImGuiComboFlags_CustomPreview)) == 0);

    if (val > 5) val = 5;
    const ImVec2 pos = window->DC.CursorPos;
    const float width = GetWindowWidth()- g.Style.WindowPadding.x * 2;;
    //菜单大小
    const ImRect rect(pos, pos + ImVec2(width, 53 * DPI));
    const ImRect clickable(pos + ImVec2(0, 23 * DPI), pos + ImVec2(width, 53 * DPI));
    ItemSize(rect, style.FramePadding.y);
    if (!ItemAdd(rect, id))
        return false;

    // Open on click
    bool hovered, held;
    bool pressed = ButtonBehavior(clickable, id, &hovered, &held);

    static std::map<ImGuiID, combo_state> anim;
    combo_state& state = anim[id];

    if (hovered && g.IO.MouseClicked[0] || state.combo_opened && g.IO.MouseClicked[0] && !state.hovered)
        state.combo_opened = !state.combo_opened;

    state.alpha = ImClamp(state.alpha + (gui->fixed_speed(8.f) * (state.combo_opened ? 1.f : -1.f)), 0.f, 1.f);
    state.offset = ImLerp(state.offset, state.combo_opened ? 10.f : 0.f, gui->fixed_speed(12.f));

    if (!IsRectVisible(rect.Min, rect.Max + ImVec2(0, 2)))
    {
        state.combo_opened = false;
        state.alpha = 0.f;
    }
    //复选框标题大小
    window->DrawList->AddText(YuanerSet->inter_semibold_min_font, YuanerSet->inter_semibold_min_font->FontSize, rect.Min - ImVec2(1, 3), gui->get_clr(clr->combo_label), label);

    window->DrawList->AddRectFilled(clickable.Min, clickable.Max, gui->get_clr(clr->combo_background), YuanerSet->widgets_rounding);

    PushFont(YuanerSet->inter_semibold_min_font);
    PushStyleColor(ImGuiCol_Text, gui->get_clr(clr->combo_value));
    RenderTextClipped(clickable.Min + ImVec2(14, -1), clickable.Max - ImVec2(44, 1), preview_value, NULL, NULL, ImVec2(0.0f, 0.5f));
    PopStyleColor();
    PopFont();

    window->DrawList->AddRectFilledMultiColor(ImVec2(clickable.Max.x - 64, clickable.Min.y + 10), clickable.Max - ImVec2(44, 10), gui->get_clr(clr->combo_background, 0.f), gui->get_clr(clr->combo_background), gui->get_clr(clr->combo_background), gui->get_clr(clr->combo_background, 0.f));
    //箭头
    window->DrawList->AddText(YuanerSet->icon, YuanerSet->icon->FontSize, clickable.Max - ImVec2(25 * DPI, 24 * DPI), gui->get_clr(clr->accent), "\uEC53");

    if (!state.combo_opened && state.alpha < 0.1f)
        return false;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing;

    PushStyleVar(ImGuiStyleVar_Alpha, state.alpha);
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
    PushStyleVar(ImGuiStyleVar_WindowRounding, YuanerSet->widgets_rounding);
    PushStyleColor(ImGuiCol_WindowBg, gui->get_clr(clr->combo_background));
    SetNextWindowPos(clickable.GetBL() + ImVec2(0, std::round(state.offset)));
    SetNextWindowSize(ImVec2(clickable.GetWidth(), calc_combo_size(val, 31.f * DPI)));
    Begin(label, NULL, window_flags);
    {
        SetCursorPosX(GetCursorPosX() + YuanerSet->combo_spacing);
        BeginGroup();
        state.hovered = IsWindowHovered();
        const ImVec2 pos = ImGui::GetWindowPos();
        const ImVec2 size = ImGui::GetWindowSize();
        ImDrawList* draw = ImGui::GetWindowDrawList();

        if (!multi)
            if (IsWindowHovered() && g.IO.MouseClicked[0])
                state.combo_opened = false;
    }
    return true;
}

void end_combo()
{
    EndGroup();
    PopStyleColor();
    PopStyleVar(4);
    ImGui::End();
}

bool combo(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items)
{
    ImGuiContext& g = *GImGui;

    // Call the getter to obtain the preview string which is a parameter to BeginCombo()
    const char* preview_value = NULL;
    if (*current_item >= 0 && *current_item < items_count)
        preview_value = getter(user_data, *current_item);

    // The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
    if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
        SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, calc_combo_size(popup_max_height_in_items, 31.f)));

    if (!begin_combo(label, preview_value, items_count, ImGuiComboFlags_None, false))
        return false;

    // Display items
    // FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
    bool value_changed = false;
    for (int i = 0; i < items_count; i++)
    {
        const char* item_text = getter(user_data, i);
        if (item_text == NULL)
            item_text = "*Unknown item*";

        PushID(i);
        const bool item_selected = (i == *current_item);
        if (selectable_ex(item_text, item_selected) && *current_item != i)
        {
            value_changed = true;
            *current_item = i;
        }
        if (item_selected)
            SetItemDefaultFocus();
        PopID();
    }

    end_combo();

    if (value_changed)
        MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

bool c_widgets::dropdown(std::string_view label, int* current_item, const char* const items[], int items_count)
{
    const bool value_changed = combo(label.data(), current_item, items_array_getter, (void*)items, items_count, -1);
    return value_changed;
}

bool c_widgets::dropdown(std::string_view label, int* current_item, const std::vector<std::string>& items)
{
    std::vector<const char*> c_strs;
    c_strs.reserve(items.size());
    for (const auto& item : items) {
        c_strs.push_back(item.c_str());
    }

    return dropdown(label, current_item, c_strs.data(), static_cast<int>(c_strs.size()));
}


void c_widgets::multi_dropdown(std::string_view label, bool variable[], const char* labels[], int count)
{
    ImGuiContext& g = *GImGui;

    std::string preview = "Select";

    for (auto i = 0, j = 0; i < count; i++)
    {
        if (variable[i])
        {
            if (j)
                preview += (", ") + (std::string)labels[i];
            else
                preview = labels[i];

            j++;
        }
    }

    if (begin_combo(label.data(), preview.c_str(), count, 0, true))
    {
        for (auto i = 0; i < count; i++)
            selectable(labels[i], &variable[i]);
        end_combo();
    }

    preview = ("Select");
}

static bool input_text_filter_character(ImGuiContext* ctx, unsigned int* p_char, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data, ImGuiInputSource input_source)
{
    IM_ASSERT(input_source == ImGuiInputSource_Keyboard || input_source == ImGuiInputSource_Clipboard);
    unsigned int c = *p_char;

    // Filter non-printable (NB: isprint is unreliable! see #2467)
    bool apply_named_filters = true;
    if (c < 0x20)
    {
        bool pass = false;
        pass |= (c == '\n') && (flags & ImGuiInputTextFlags_Multiline) != 0; // Note that an Enter KEY will emit \r and be ignored (we poll for KEY in InputText() code)
        pass |= (c == '\t') && (flags & ImGuiInputTextFlags_AllowTabInput) != 0;
        if (!pass)
            return false;
        apply_named_filters = false; // Override named filters below so newline and tabs can still be inserted.
    }

    if (input_source != ImGuiInputSource_Clipboard)
    {
        // We ignore Ascii representation of delete (emitted from Backspace on OSX, see #2578, #2817)
        if (c == 127)
            return false;

        // Filter private Unicode range. GLFW on OSX seems to send private characters for special keys like arrow keys (FIXME)
        if (c >= 0xE000 && c <= 0xF8FF)
            return false;
    }

    // Filter Unicode ranges we are not handling in this build
    if (c > IM_UNICODE_CODEPOINT_MAX)
        return false;

    // Generic named filters
    if (apply_named_filters && (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsScientific)))
    {
        // The libc allows overriding locale, with e.g. 'setlocale(LC_NUMERIC, "de_DE.UTF-8");' which affect the output/input of printf/scanf to use e.g. ',' instead of '.'.
        // The standard mandate that programs starts in the "C" locale where the decimal point is '.'.
        // We don't really intend to provide widespread support for it, but out of empathy for people stuck with using odd API, we support the bare minimum aka overriding the decimal point.
        // Change the default decimal_point with:
        //   ImGui::GetIO()->PlatformLocaleDecimalPoint = *localeconv()->decimal_point;
        // Users of non-default decimal point (in particular ',') may be affected by word-selection logic (is_word_boundary_from_right/is_word_boundary_from_left) functions.
        ImGuiContext& g = *ctx;
        const unsigned c_decimal_point = (unsigned int)g.IO.PlatformLocaleDecimalPoint;
        if (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsScientific))
            if (c == '.' || c == ',')
                c = c_decimal_point;

        // Full-width -> half-width conversion for numeric fields (https://en.wikipedia.org/wiki/Halfwidth_and_Fullwidth_Forms_(Unicode_block)
        // While this is mostly convenient, this has the side-effect for uninformed users accidentally inputting full-width characters that they may
        // scratch their head as to why it works in numerical fields vs in generic text fields it would require support in the font.
        if (flags & (ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsScientific | ImGuiInputTextFlags_CharsHexadecimal))
            if (c >= 0xFF01 && c <= 0xFF5E)
                c = c - 0xFF01 + 0x21;

        // Allow 0-9 . - + * /
        if (flags & ImGuiInputTextFlags_CharsDecimal)
            if (!(c >= '0' && c <= '9') && (c != c_decimal_point) && (c != '-') && (c != '+') && (c != '*') && (c != '/'))
                return false;

        // Allow 0-9 . - + * / e E
        if (flags & ImGuiInputTextFlags_CharsScientific)
            if (!(c >= '0' && c <= '9') && (c != c_decimal_point) && (c != '-') && (c != '+') && (c != '*') && (c != '/') && (c != 'e') && (c != 'E'))
                return false;

        // Allow 0-9 a-F A-F
        if (flags & ImGuiInputTextFlags_CharsHexadecimal)
            if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F'))
                return false;

        // Turn a-z into A-Z
        if (flags & ImGuiInputTextFlags_CharsUppercase)
            if (c >= 'a' && c <= 'z')
                c += (unsigned int)('A' - 'a');

        if (flags & ImGuiInputTextFlags_CharsNoBlank)
            if (ImCharIsBlankW(c))
                return false;

        *p_char = c;
    }

    // Custom callback filter
    if (flags & ImGuiInputTextFlags_CallbackCharFilter)
    {
        ImGuiContext& g = *GImGui;
        ImGuiInputTextCallbackData callback_data;
        callback_data.Ctx = &g;
        callback_data.EventFlag = ImGuiInputTextFlags_CallbackCharFilter;
        callback_data.EventChar = (ImWchar)c;
        callback_data.Flags = flags;
        callback_data.UserData = user_data;
        if (callback(&callback_data) != 0)
            return false;
        *p_char = callback_data.EventChar;
        if (!callback_data.EventChar)
            return false;
    }

    return true;
}

static int input_text_calc_text_len_and_line_count(const char* text_begin, const char** out_text_end)
{
    int line_count = 0;
    const char* s = text_begin;
    while (char c = *s++) // We are only matching for \n so we can ignore UTF-8 decoding
        if (c == '\n')
            line_count++;
    s--;
    if (s[0] != '\n' && s[0] != '\r')
        line_count++;
    *out_text_end = s;
    return line_count;
}

static void input_text_reconcile_undo_state_after_user_callback(ImGuiInputTextState* state, const char* new_buf_a, int new_length_a)
{
    ImGuiContext& g = *GImGui;
    const ImWchar* old_buf = state->TextW.Data;
    const int old_length = state->CurLenW;
    const int new_length = ImTextCountCharsFromUtf8(new_buf_a, new_buf_a + new_length_a);
    g.TempBuffer.reserve_discard((new_length + 1) * sizeof(ImWchar));
    ImWchar* new_buf = (ImWchar*)(void*)g.TempBuffer.Data;
    ImTextStrFromUtf8(new_buf, new_length + 1, new_buf_a, new_buf_a + new_length_a);

    const int shorter_length = ImMin(old_length, new_length);
    int first_diff;
    for (first_diff = 0; first_diff < shorter_length; first_diff++)
        if (old_buf[first_diff] != new_buf[first_diff])
            break;
    if (first_diff == old_length && first_diff == new_length)
        return;

    int old_last_diff = old_length - 1;
    int new_last_diff = new_length - 1;
    for (; old_last_diff >= first_diff && new_last_diff >= first_diff; old_last_diff--, new_last_diff--)
        if (old_buf[old_last_diff] != new_buf[new_last_diff])
            break;

    const int insert_len = new_last_diff - first_diff + 1;
    const int delete_len = old_last_diff - first_diff + 1;
    if (insert_len > 0 || delete_len > 0)
        if (IMSTB_TEXTEDIT_CHARTYPE* p = stb_text_createundo(&state->Stb.undostate, first_diff, delete_len, insert_len))
            for (int i = 0; i < delete_len; i++)
                p[i] = ImStb::STB_TEXTEDIT_GETCHAR(state, first_diff + i);
}

bool input_text_ex(const char* label, const char* hint, char* buf, int buf_size, const ImVec2& size_arg, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* callback_user_data)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    IM_ASSERT(buf != NULL && buf_size >= 0);
    IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackHistory) && (flags & ImGuiInputTextFlags_Multiline)));        // Can't use both together (they both use up/down keys)
    IM_ASSERT(!((flags & ImGuiInputTextFlags_CallbackCompletion) && (flags & ImGuiInputTextFlags_AllowTabInput))); // Can't use both together (they both use tab key)

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;

    const bool RENDER_SELECTION_WHEN_INACTIVE = false;
    const bool is_multiline = (flags & ImGuiInputTextFlags_Multiline) != 0;

    if (is_multiline) // Open group before calling GetID() because groups tracks id created within their scope (including the scrollbar)
        BeginGroup();
    const ImGuiID id = window->GetID(label);

    const ImVec2 pos = window->DC.CursorPos;
    const float width = GetWindowWidth() - g.Style.WindowPadding.x * 2;;
    const ImRect rect(pos, pos + ImVec2(width, 72));
    const ImRect clickable(pos + ImVec2(0, 27), pos + ImVec2(width, 72));

    ImGuiWindow* draw_window = window;
    ImVec2 inner_size = clickable.GetSize();
    ImGuiLastItemData item_data_backup;
    if (is_multiline)
    {
        ImVec2 backup_pos = window->DC.CursorPos;
        ItemSize(rect, style.FramePadding.y);
        if (!ItemAdd(rect, id, &clickable, ImGuiItemFlags_Inputable))
        {
            EndGroup();
            return false;
        }
        item_data_backup = g.LastItemData;
        window->DC.CursorPos = backup_pos;

        // Prevent NavActivation from Tabbing when our widget accepts Tab inputs: this allows cycling through widgets without stopping.
        if (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_FromTabbing) && (flags & ImGuiInputTextFlags_AllowTabInput))
            g.NavActivateId = 0;

        // Prevent NavActivate reactivating in BeginChild() when we are already active.
        const ImGuiID backup_activate_id = g.NavActivateId;
        if (g.ActiveId == id) // Prevent reactivation
            g.NavActivateId = 0;

        // We reproduce the contents of BeginChildFrame() in order to provide 'label' so our window internal data are easier to read/debug.
        PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_FrameBg]);
        PushStyleVar(ImGuiStyleVar_ChildRounding, style.FrameRounding);
        PushStyleVar(ImGuiStyleVar_ChildBorderSize, style.FrameBorderSize);
        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Ensure no clip rect so mouse hover can reach FramePadding edges
        bool child_visible = BeginChildEx(label, id, clickable.GetSize(), true, ImGuiWindowFlags_NoMove);
        g.NavActivateId = backup_activate_id;
        PopStyleVar(3);
        PopStyleColor();
        if (!child_visible)
        {
            EndChild();
            EndGroup();
            return false;
        }
        draw_window = g.CurrentWindow; // Child window
        draw_window->DC.NavLayersActiveMaskNext |= (1 << draw_window->DC.NavLayerCurrent); // This is to ensure that EndChild() will display a navigation highlight so we can "enter" into it.
        draw_window->DC.CursorPos += style.FramePadding;
        inner_size.x -= draw_window->ScrollbarSizes.x;
    }
    else
    {
        // Support for internal ImGuiInputTextFlags_MergedItem flag, which could be redesigned as an ItemFlags if needed (with test performed in ItemAdd)
        ItemSize(rect, style.FramePadding.y);
        if (!(flags & ImGuiInputTextFlags_MergedItem))
            if (!ItemAdd(rect, id, &clickable, ImGuiItemFlags_Inputable))
                return false;
    }
    const bool hovered = ItemHoverable(clickable, id, g.LastItemData.InFlags);

    PushFont(YuanerSet->inter_semibold_min_font);
    // We are only allowed to access the state if we are already the active widget.
    ImGuiInputTextState* state = GetInputTextState(id);

    if (g.LastItemData.InFlags & ImGuiItemFlags_ReadOnly)
        flags |= ImGuiInputTextFlags_ReadOnly;
    const bool is_readonly = (flags & ImGuiInputTextFlags_ReadOnly) != 0;
    const bool is_password = (flags & ImGuiInputTextFlags_Password) != 0;
    const bool is_undoable = (flags & ImGuiInputTextFlags_NoUndoRedo) == 0;
    const bool is_resizable = (flags & ImGuiInputTextFlags_CallbackResize) != 0;
    if (is_resizable)
        IM_ASSERT(callback != NULL); // Must provide a callback if you set the ImGuiInputTextFlags_CallbackResize flag!

    const bool input_requested_by_nav = (g.ActiveId != id) && ((g.NavActivateId == id) && ((g.NavActivateFlags & ImGuiActivateFlags_PreferInput) || (g.NavInputSource == ImGuiInputSource_Keyboard)));

    const bool user_clicked = hovered && io.MouseClicked[0];
    const bool user_scroll_finish = is_multiline && state != NULL && g.ActiveId == 0 && g.ActiveIdPreviousFrame == GetWindowScrollbarID(draw_window, ImGuiAxis_Y);
    const bool user_scroll_active = is_multiline && state != NULL && g.ActiveId == GetWindowScrollbarID(draw_window, ImGuiAxis_Y);
    bool clear_active_id = false;
    bool select_all = false;

    float scroll_y = is_multiline ? draw_window->Scroll.y : FLT_MAX;

    const bool init_reload_from_user_buf = (state != NULL && state->ReloadUserBuf);
    const bool init_changed_specs = (state != NULL && state->Stb.single_line != !is_multiline); // state != NULL means its our state.
    const bool init_make_active = (user_clicked || user_scroll_finish || input_requested_by_nav);
    const bool init_state = (init_make_active || user_scroll_active);
    if ((init_state && g.ActiveId != id) || init_changed_specs || init_reload_from_user_buf)
    {
        // Access state even if we don't own it yet.
        state = &g.InputTextState;
        state->CursorAnimReset();
        state->ReloadUserBuf = false;

        // Backup state of deactivating item so they'll have a chance to do a write to output buffer on the same frame they report IsItemDeactivatedAfterEdit (#4714)
        InputTextDeactivateHook(state->ID);

        // From the moment we focused we are normally ignoring the content of 'buf' (unless we are in read-only mode)
        const int buf_len = (int)strlen(buf);
        if (!init_reload_from_user_buf)
        {
            // Take a copy of the initial buffer value.
            state->InitialTextA.resize(buf_len + 1);    // UTF-8. we use +1 to make sure that .Data is always pointing to at least an empty string.
            memcpy(state->InitialTextA.Data, buf, buf_len + 1);
        }

        // Preserve cursor position and undo/redo stack if we come back to same widget
        // FIXME: Since we reworked this on 2022/06, may want to differentiate recycle_cursor vs recycle_undostate?
        bool recycle_state = (state->ID == id && !init_changed_specs && !init_reload_from_user_buf);
        if (recycle_state && (state->CurLenA != buf_len || (state->TextAIsValid && strncmp(state->TextA.Data, buf, buf_len) != 0)))
            recycle_state = false;

        // Start edition
        const char* buf_end = NULL;
        state->ID = id;
        state->TextW.resize(buf_size + 1);          // wchar count <= UTF-8 count. we use +1 to make sure that .Data is always pointing to at least an empty string.
        state->TextA.resize(0);
        state->TextAIsValid = false;                // TextA is not valid yet (we will display buf until then)
        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, buf_size, buf, NULL, &buf_end);
        state->CurLenA = (int)(buf_end - buf);      // We can't get the result from ImStrncpy() above because it is not UTF-8 aware. Here we'll cut off malformed UTF-8.

        if (recycle_state)
        {
            // Recycle existing cursor/selection/undo stack but clamp position
            // Note a single mouse click will override the cursor/position immediately by calling stb_textedit_click handler.
            state->CursorClamp();
        }
        else
        {
            state->ScrollX = 0.0f;
            stb_textedit_initialize_state(&state->Stb, !is_multiline);
        }

        if (init_reload_from_user_buf)
        {
            state->Stb.select_start = state->ReloadSelectionStart;
            state->Stb.cursor = state->Stb.select_end = state->ReloadSelectionEnd;
            state->CursorClamp();
        }
        else if (!is_multiline)
        {
            if (flags & ImGuiInputTextFlags_AutoSelectAll)
                select_all = true;
            if (input_requested_by_nav && (!recycle_state || !(g.NavActivateFlags & ImGuiActivateFlags_TryToPreserveState)))
                select_all = true;
            if (user_clicked && io.KeyCtrl)
                select_all = true;
        }

        if (flags & ImGuiInputTextFlags_AlwaysOverwrite)
            state->Stb.insert_mode = 1; // stb field name is indeed incorrect (see #2863)
    }

    const bool is_osx = io.ConfigMacOSXBehaviors;
    if (g.ActiveId != id && init_make_active)
    {
        IM_ASSERT(state && state->ID == id);
        SetActiveID(id, window);
        SetFocusID(id, window);
        FocusWindow(window);
    }
    if (g.ActiveId == id)
    {
        // Declare some inputs, the other are registered and polled via Shortcut() routing system.
        if (user_clicked)
            SetKeyOwner(ImGuiKey_MouseLeft, id);
        g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
        if (is_multiline || (flags & ImGuiInputTextFlags_CallbackHistory))
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Up) | (1 << ImGuiDir_Down);
        SetKeyOwner(ImGuiKey_Enter, id);
        SetKeyOwner(ImGuiKey_KeypadEnter, id);
        SetKeyOwner(ImGuiKey_Home, id);
        SetKeyOwner(ImGuiKey_End, id);
        if (is_multiline)
        {
            SetKeyOwner(ImGuiKey_PageUp, id);
            SetKeyOwner(ImGuiKey_PageDown, id);
        }
        if (is_osx)
            SetKeyOwner(ImGuiMod_Alt, id);
    }

    // We have an edge case if ActiveId was set through another widget (e.g. widget being swapped), clear id immediately (don't wait until the end of the function)
    if (g.ActiveId == id && state == NULL)
        ClearActiveID();

    // Release focus when we click outside
    if (g.ActiveId == id && io.MouseClicked[0] && !init_state && !init_make_active) //-V560
        clear_active_id = true;

    // Lock the decision of whether we are going to take the path displaying the cursor or selection
    bool render_cursor = (g.ActiveId == id) || (state && user_scroll_active);
    bool render_selection = state && (state->HasSelection() || select_all) && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
    bool value_changed = false;
    bool validated = false;

    // When read-only we always use the live data passed to the function
    // FIXME-OPT: Because our selection/cursor code currently needs the wide text we need to convert it when active, which is not ideal :(
    if (is_readonly && state != NULL && (render_cursor || render_selection))
    {
        const char* buf_end = NULL;
        state->TextW.resize(buf_size + 1);
        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, buf, NULL, &buf_end);
        state->CurLenA = (int)(buf_end - buf);
        state->CursorClamp();
        render_selection &= state->HasSelection();
    }

    // Select the buffer to render.
    const bool buf_display_from_state = (render_cursor || render_selection || g.ActiveId == id) && !is_readonly && state && state->TextAIsValid;
    const bool is_displaying_hint = (hint != NULL && (buf_display_from_state ? state->TextA.Data : buf)[0] == 0);

    // Password pushes a temporary font with only a fallback glyph
    if (is_password && !is_displaying_hint)
    {
        const ImFontGlyph* glyph = g.Font->FindGlyph('*');
        ImFont* password_font = &g.InputTextPasswordFont;
        password_font->FontSize = g.Font->FontSize;
        password_font->Scale = g.Font->Scale;
        password_font->Ascent = g.Font->Ascent;
        password_font->Descent = g.Font->Descent;
        password_font->ContainerAtlas = g.Font->ContainerAtlas;
        password_font->FallbackGlyph = glyph;
        password_font->FallbackAdvanceX = glyph->AdvanceX;
        IM_ASSERT(password_font->Glyphs.empty() && password_font->IndexAdvanceX.empty() && password_font->IndexLookup.empty());
        PushFont(password_font);
    }

    // Process mouse inputs and character inputs
    int backup_current_text_length = 0;
    if (g.ActiveId == id)
    {
        IM_ASSERT(state != NULL);
        backup_current_text_length = state->CurLenA;
        state->Edited = false;
        state->BufCapacityA = buf_size;
        state->Flags = flags;

        // Although we are active we don't prevent mouse from hovering other elements unless we are interacting right now with the widget.
        // Down the line we should have a cleaner library-wide concept of Selected vs Active.
        g.ActiveIdAllowOverlap = !io.MouseDown[0];

        // Edit in progress
        const float mouse_x = (io.MousePos.x - clickable.Min.x - style.FramePadding.x) + state->ScrollX;
        const float mouse_y = (is_multiline ? (io.MousePos.y - draw_window->DC.CursorPos.y) : (g.FontSize * 0.5f));

        if (select_all)
        {
            state->SelectAll();
            state->SelectedAllMouseLock = true;
        }
        else if (hovered && io.MouseClickedCount[0] >= 2 && !io.KeyShift)
        {
            stb_textedit_click(state, &state->Stb, mouse_x, mouse_y);
            const int multiclick_count = (io.MouseClickedCount[0] - 2);
            if ((multiclick_count % 2) == 0)
            {
                // Double-click: Select word
                // We always use the "Mac" word advance for double-click select vs CTRL+Right which use the platform dependent variant:
                // FIXME: There are likely many ways to improve this behavior, but there's no "right" behavior (depends on use-case, software, OS)
                const bool is_bol = (state->Stb.cursor == 0) || ImStb::STB_TEXTEDIT_GETCHAR(state, state->Stb.cursor - 1) == '\n';
                if (STB_TEXT_HAS_SELECTION(&state->Stb) || !is_bol)
                    state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT);
                //state->OnKeyPressed(STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT);
                if (!STB_TEXT_HAS_SELECTION(&state->Stb))
                    ImStb::stb_textedit_prep_selection_at_cursor(&state->Stb);
                state->Stb.cursor = ImStb::STB_TEXTEDIT_MOVEWORDRIGHT_MAC(state, state->Stb.cursor);
                state->Stb.select_end = state->Stb.cursor;
                ImStb::stb_textedit_clamp(state, &state->Stb);
            }
            else
            {
                // Triple-click: Select line
                const bool is_eol = ImStb::STB_TEXTEDIT_GETCHAR(state, state->Stb.cursor) == '\n';
                state->OnKeyPressed(STB_TEXTEDIT_K_LINESTART);
                state->OnKeyPressed(STB_TEXTEDIT_K_LINEEND | STB_TEXTEDIT_K_SHIFT);
                state->OnKeyPressed(STB_TEXTEDIT_K_RIGHT | STB_TEXTEDIT_K_SHIFT);
                if (!is_eol && is_multiline)
                {
                    ImSwap(state->Stb.select_start, state->Stb.select_end);
                    state->Stb.cursor = state->Stb.select_end;
                }
                state->CursorFollow = false;
            }
            state->CursorAnimReset();
        }
        else if (io.MouseClicked[0] && !state->SelectedAllMouseLock)
        {
            if (hovered)
            {
                if (io.KeyShift)
                    stb_textedit_drag(state, &state->Stb, mouse_x, mouse_y);
                else
                    stb_textedit_click(state, &state->Stb, mouse_x, mouse_y);
                state->CursorAnimReset();
            }
        }
        else if (io.MouseDown[0] && !state->SelectedAllMouseLock && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f))
        {
            stb_textedit_drag(state, &state->Stb, mouse_x, mouse_y);
            state->CursorAnimReset();
            state->CursorFollow = true;
        }
        if (state->SelectedAllMouseLock && !io.MouseDown[0])
            state->SelectedAllMouseLock = false;

        // We expect backends to emit a Tab key but some also emit a Tab character which we ignore (#2467, #1336)
        // (For Tab and Enter: Win32/SFML/Allegro are sending both keys and chars, GLFW and SDL are only sending keys. For Space they all send all threes)
        if ((flags & ImGuiInputTextFlags_AllowTabInput) && !is_readonly)
        {
            if (Shortcut(ImGuiKey_Tab, id, ImGuiInputFlags_Repeat))
            {
                unsigned int c = '\t'; // Insert TAB
                if (input_text_filter_character(&g, &c, flags, callback, callback_user_data, ImGuiInputSource_Keyboard))
                    state->OnKeyPressed((int)c);
            }
            // FIXME: Implement Shift+Tab
            /*
            if (Shortcut(ImGuiKey_Tab | ImGuiMod_Shift, id, ImGuiInputFlags_Repeat))
            {
            }
            */
        }

        // Process regular text input (before we check for Return because using some IME will effectively send a Return?)
        // We ignore CTRL inputs, but need to allow ALT+CTRL as some keyboards (e.g. German) use AltGR (which _is_ Alt+Ctrl) to input certain characters.
        const bool ignore_char_inputs = (io.KeyCtrl && !io.KeyAlt) || (is_osx && io.KeySuper);
        if (io.InputQueueCharacters.Size > 0)
        {
            if (!ignore_char_inputs && !is_readonly && !input_requested_by_nav)
                for (int n = 0; n < io.InputQueueCharacters.Size; n++)
                {
                    // Insert character if they pass filtering
                    unsigned int c = (unsigned int)io.InputQueueCharacters[n];
                    if (c == '\t') // Skip Tab, see above.
                        continue;
                    if (input_text_filter_character(&g, &c, flags, callback, callback_user_data, ImGuiInputSource_Keyboard))
                        state->OnKeyPressed((int)c);
                }

            // Consume characters
            io.InputQueueCharacters.resize(0);
        }
    }

    // Process other shortcuts/key-presses
    bool revert_edit = false;
    if (g.ActiveId == id && !g.ActiveIdIsJustActivated && !clear_active_id)
    {
        IM_ASSERT(state != NULL);

        const int row_count_per_page = ImMax((int)((inner_size.y - style.FramePadding.y) / g.FontSize), 1);
        state->Stb.row_count_per_page = row_count_per_page;

        const int k_mask = (io.KeyShift ? STB_TEXTEDIT_K_SHIFT : 0);
        const bool is_wordmove_key_down = is_osx ? io.KeyAlt : io.KeyCtrl;                     // OS X style: Text editing cursor movement using Alt instead of Ctrl
        const bool is_startend_key_down = is_osx && io.KeySuper && !io.KeyCtrl && !io.KeyAlt;  // OS X style: Line/Text Start and End using Cmd+Arrows instead of Home/End

        // Using Shortcut() with ImGuiInputFlags_RouteFocused (default policy) to allow routing operations for other code (e.g. calling window trying to use CTRL+A and CTRL+B: formet would be handled by InputText)
        // Otherwise we could simply assume that we own the keys as we are active.
        const ImGuiInputFlags f_repeat = ImGuiInputFlags_Repeat;
        const bool is_cut = (Shortcut(ImGuiMod_Shortcut | ImGuiKey_X, id, f_repeat) || Shortcut(ImGuiMod_Shift | ImGuiKey_Delete, id, f_repeat)) && !is_readonly && !is_password && (!is_multiline || state->HasSelection());
        const bool is_copy = (Shortcut(ImGuiMod_Shortcut | ImGuiKey_C, id) || Shortcut(ImGuiMod_Ctrl | ImGuiKey_Insert, id)) && !is_password && (!is_multiline || state->HasSelection());
        const bool is_paste = (Shortcut(ImGuiMod_Shortcut | ImGuiKey_V, id, f_repeat) || Shortcut(ImGuiMod_Shift | ImGuiKey_Insert, id, f_repeat)) && !is_readonly;
        const bool is_undo = (Shortcut(ImGuiMod_Shortcut | ImGuiKey_Z, id, f_repeat)) && !is_readonly && is_undoable;
        const bool is_redo = (Shortcut(ImGuiMod_Shortcut | ImGuiKey_Y, id, f_repeat) || (is_osx && Shortcut(ImGuiMod_Shortcut | ImGuiMod_Shift | ImGuiKey_Z, id, f_repeat))) && !is_readonly && is_undoable;
        const bool is_select_all = Shortcut(ImGuiMod_Shortcut | ImGuiKey_A, id);

        // We allow validate/cancel with Nav source (gamepad) to makes it easier to undo an accidental NavInput press with no keyboard wired, but otherwise it isn't very useful.
        const bool nav_gamepad_active = (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) != 0 && (io.BackendFlags & ImGuiBackendFlags_HasGamepad) != 0;
        const bool is_enter_pressed = IsKeyPressed(ImGuiKey_Enter, true) || IsKeyPressed(ImGuiKey_KeypadEnter, true);
        const bool is_gamepad_validate = nav_gamepad_active && (IsKeyPressed(ImGuiKey_NavGamepadActivate, false) || IsKeyPressed(ImGuiKey_NavGamepadInput, false));
        const bool is_cancel = Shortcut(ImGuiKey_Escape, id, f_repeat) || (nav_gamepad_active && Shortcut(ImGuiKey_NavGamepadCancel, id, f_repeat));

        // FIXME: Should use more Shortcut() and reduce IsKeyPressed()+SetKeyOwner(), but requires modifiers combination to be taken account of.
        if (IsKeyPressed(ImGuiKey_LeftArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINESTART : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDLEFT : STB_TEXTEDIT_K_LEFT) | k_mask); }
        else if (IsKeyPressed(ImGuiKey_RightArrow)) { state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_LINEEND : is_wordmove_key_down ? STB_TEXTEDIT_K_WORDRIGHT : STB_TEXTEDIT_K_RIGHT) | k_mask); }
        else if (IsKeyPressed(ImGuiKey_UpArrow) && is_multiline) { if (io.KeyCtrl) SetScrollY(draw_window, ImMax(draw_window->Scroll.y - g.FontSize, 0.0f)); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTSTART : STB_TEXTEDIT_K_UP) | k_mask); }
        else if (IsKeyPressed(ImGuiKey_DownArrow) && is_multiline) { if (io.KeyCtrl) SetScrollY(draw_window, ImMin(draw_window->Scroll.y + g.FontSize, GetScrollMaxY())); else state->OnKeyPressed((is_startend_key_down ? STB_TEXTEDIT_K_TEXTEND : STB_TEXTEDIT_K_DOWN) | k_mask); }
        else if (IsKeyPressed(ImGuiKey_PageUp) && is_multiline) { state->OnKeyPressed(STB_TEXTEDIT_K_PGUP | k_mask); scroll_y -= row_count_per_page * g.FontSize; }
        else if (IsKeyPressed(ImGuiKey_PageDown) && is_multiline) { state->OnKeyPressed(STB_TEXTEDIT_K_PGDOWN | k_mask); scroll_y += row_count_per_page * g.FontSize; }
        else if (IsKeyPressed(ImGuiKey_Home)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTSTART | k_mask : STB_TEXTEDIT_K_LINESTART | k_mask); }
        else if (IsKeyPressed(ImGuiKey_End)) { state->OnKeyPressed(io.KeyCtrl ? STB_TEXTEDIT_K_TEXTEND | k_mask : STB_TEXTEDIT_K_LINEEND | k_mask); }
        else if (IsKeyPressed(ImGuiKey_Delete) && !is_readonly && !is_cut)
        {
            if (!state->HasSelection())
            {
                // OSX doesn't seem to have Super+Delete to delete until end-of-line, so we don't emulate that (as opposed to Super+Backspace)
                if (is_wordmove_key_down)
                    state->OnKeyPressed(STB_TEXTEDIT_K_WORDRIGHT | STB_TEXTEDIT_K_SHIFT);
            }
            state->OnKeyPressed(STB_TEXTEDIT_K_DELETE | k_mask);
        }
        else if (IsKeyPressed(ImGuiKey_Backspace) && !is_readonly)
        {
            if (!state->HasSelection())
            {
                if (is_wordmove_key_down)
                    state->OnKeyPressed(STB_TEXTEDIT_K_WORDLEFT | STB_TEXTEDIT_K_SHIFT);
                else if (is_osx && io.KeySuper && !io.KeyAlt && !io.KeyCtrl)
                    state->OnKeyPressed(STB_TEXTEDIT_K_LINESTART | STB_TEXTEDIT_K_SHIFT);
            }
            state->OnKeyPressed(STB_TEXTEDIT_K_BACKSPACE | k_mask);
        }
        else if (is_enter_pressed || is_gamepad_validate)
        {
            // Determine if we turn Enter into a \n character
            bool ctrl_enter_for_new_line = (flags & ImGuiInputTextFlags_CtrlEnterForNewLine) != 0;
            if (!is_multiline || is_gamepad_validate || (ctrl_enter_for_new_line && !io.KeyCtrl) || (!ctrl_enter_for_new_line && io.KeyCtrl))
            {
                validated = true;
                if (io.ConfigInputTextEnterKeepActive && !is_multiline)
                    state->SelectAll(); // No need to scroll
                else
                    clear_active_id = true;
            }
            else if (!is_readonly)
            {
                unsigned int c = '\n'; // Insert new line
                if (input_text_filter_character(&g, &c, flags, callback, callback_user_data, ImGuiInputSource_Keyboard))
                    state->OnKeyPressed((int)c);
            }
        }
        else if (is_cancel)
        {
            if (flags & ImGuiInputTextFlags_EscapeClearsAll)
            {
                if (buf[0] != 0)
                {
                    revert_edit = true;
                }
                else
                {
                    render_cursor = render_selection = false;
                    clear_active_id = true;
                }
            }
            else
            {
                clear_active_id = revert_edit = true;
                render_cursor = render_selection = false;
            }
        }
        else if (is_undo || is_redo)
        {
            state->OnKeyPressed(is_undo ? STB_TEXTEDIT_K_UNDO : STB_TEXTEDIT_K_REDO);
            state->ClearSelection();
        }
        else if (is_select_all)
        {
            state->SelectAll();
            state->CursorFollow = true;
        }
        else if (is_cut || is_copy)
        {
            // Cut, Copy
            if (io.SetClipboardTextFn)
            {
                const int ib = state->HasSelection() ? ImMin(state->Stb.select_start, state->Stb.select_end) : 0;
                const int ie = state->HasSelection() ? ImMax(state->Stb.select_start, state->Stb.select_end) : state->CurLenW;
                const int clipboard_data_len = ImTextCountUtf8BytesFromStr(state->TextW.Data + ib, state->TextW.Data + ie) + 1;
                char* clipboard_data = (char*)IM_ALLOC(clipboard_data_len * sizeof(char));
                ImTextStrToUtf8(clipboard_data, clipboard_data_len, state->TextW.Data + ib, state->TextW.Data + ie);
                SetClipboardText(clipboard_data);
                MemFree(clipboard_data);
            }
            if (is_cut)
            {
                if (!state->HasSelection())
                    state->SelectAll();
                state->CursorFollow = true;
                stb_textedit_cut(state, &state->Stb);
            }
        }
        else if (is_paste)
        {
            if (const char* clipboard = GetClipboardText())
            {
                // Filter pasted buffer
                const int clipboard_len = (int)strlen(clipboard);
                ImWchar* clipboard_filtered = (ImWchar*)IM_ALLOC((clipboard_len + 1) * sizeof(ImWchar));
                int clipboard_filtered_len = 0;
                for (const char* s = clipboard; *s != 0; )
                {
                    unsigned int c;
                    s += ImTextCharFromUtf8(&c, s, NULL);
                    if (!input_text_filter_character(&g, &c, flags, callback, callback_user_data, ImGuiInputSource_Clipboard))
                        continue;
                    clipboard_filtered[clipboard_filtered_len++] = (ImWchar)c;
                }
                clipboard_filtered[clipboard_filtered_len] = 0;
                if (clipboard_filtered_len > 0) // If everything was filtered, ignore the pasting operation
                {
                    stb_textedit_paste(state, &state->Stb, clipboard_filtered, clipboard_filtered_len);
                    state->CursorFollow = true;
                }
                MemFree(clipboard_filtered);
            }
        }

        // Update render selection flag after events have been handled, so selection highlight can be displayed during the same frame.
        render_selection |= state->HasSelection() && (RENDER_SELECTION_WHEN_INACTIVE || render_cursor);
    }

    // Process callbacks and apply result back to user's buffer.
    const char* apply_new_text = NULL;
    int apply_new_text_length = 0;
    if (g.ActiveId == id)
    {
        IM_ASSERT(state != NULL);
        if (revert_edit && !is_readonly)
        {
            if (flags & ImGuiInputTextFlags_EscapeClearsAll)
            {
                // Clear input
                IM_ASSERT(buf[0] != 0);
                apply_new_text = "";
                apply_new_text_length = 0;
                value_changed = true;
                IMSTB_TEXTEDIT_CHARTYPE empty_string;
                stb_textedit_replace(state, &state->Stb, &empty_string, 0);
            }
            else if (strcmp(buf, state->InitialTextA.Data) != 0)
            {
                // Restore initial value. Only return true if restoring to the initial value changes the current buffer contents.
                // Push records into the undo stack so we can CTRL+Z the revert operation itself
                apply_new_text = state->InitialTextA.Data;
                apply_new_text_length = state->InitialTextA.Size - 1;
                value_changed = true;
                ImVector<ImWchar> w_text;
                if (apply_new_text_length > 0)
                {
                    w_text.resize(ImTextCountCharsFromUtf8(apply_new_text, apply_new_text + apply_new_text_length) + 1);
                    ImTextStrFromUtf8(w_text.Data, w_text.Size, apply_new_text, apply_new_text + apply_new_text_length);
                }
                stb_textedit_replace(state, &state->Stb, w_text.Data, (apply_new_text_length > 0) ? (w_text.Size - 1) : 0);
            }
        }

        // Apply ASCII value
        if (!is_readonly)
        {
            state->TextAIsValid = true;
            state->TextA.resize(state->TextW.Size * 4 + 1);
            ImTextStrToUtf8(state->TextA.Data, state->TextA.Size, state->TextW.Data, NULL);
        }

        // When using 'ImGuiInputTextFlags_EnterReturnsTrue' as a special case we reapply the live buffer back to the input buffer
        // before clearing ActiveId, even though strictly speaking it wasn't modified on this frame.
        // If we didn't do that, code like InputInt() with ImGuiInputTextFlags_EnterReturnsTrue would fail.
        // This also allows the user to use InputText() with ImGuiInputTextFlags_EnterReturnsTrue without maintaining any user-side storage
        // (please note that if you use this property along ImGuiInputTextFlags_CallbackResize you can end up with your temporary string object
        // unnecessarily allocating once a frame, either store your string data, either if you don't then don't use ImGuiInputTextFlags_CallbackResize).
        const bool apply_edit_back_to_user_buffer = !revert_edit || (validated && (flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0);
        if (apply_edit_back_to_user_buffer)
        {
            // Apply new value immediately - copy modified buffer back
            // Note that as soon as the input box is active, the in-widget value gets priority over any underlying modification of the input buffer
            // FIXME: We actually always render 'buf' when calling DrawList->AddText, making the comment above incorrect.
            // FIXME-OPT: CPU waste to do this every time the widget is active, should mark dirty state from the stb_textedit callbacks.

            // User callback
            if ((flags & (ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackEdit | ImGuiInputTextFlags_CallbackAlways)) != 0)
            {
                IM_ASSERT(callback != NULL);

                // The reason we specify the usage semantic (Completion/History) is that Completion needs to disable keyboard TABBING at the moment.
                ImGuiInputTextFlags event_flag = 0;
                ImGuiKey event_key = ImGuiKey_None;
                if ((flags & ImGuiInputTextFlags_CallbackCompletion) != 0 && Shortcut(ImGuiKey_Tab, id))
                {
                    event_flag = ImGuiInputTextFlags_CallbackCompletion;
                    event_key = ImGuiKey_Tab;
                }
                else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressed(ImGuiKey_UpArrow))
                {
                    event_flag = ImGuiInputTextFlags_CallbackHistory;
                    event_key = ImGuiKey_UpArrow;
                }
                else if ((flags & ImGuiInputTextFlags_CallbackHistory) != 0 && IsKeyPressed(ImGuiKey_DownArrow))
                {
                    event_flag = ImGuiInputTextFlags_CallbackHistory;
                    event_key = ImGuiKey_DownArrow;
                }
                else if ((flags & ImGuiInputTextFlags_CallbackEdit) && state->Edited)
                {
                    event_flag = ImGuiInputTextFlags_CallbackEdit;
                }
                else if (flags & ImGuiInputTextFlags_CallbackAlways)
                {
                    event_flag = ImGuiInputTextFlags_CallbackAlways;
                }

                if (event_flag)
                {
                    ImGuiInputTextCallbackData callback_data;
                    callback_data.Ctx = &g;
                    callback_data.EventFlag = event_flag;
                    callback_data.Flags = flags;
                    callback_data.UserData = callback_user_data;

                    char* callback_buf = is_readonly ? buf : state->TextA.Data;
                    callback_data.EventKey = event_key;
                    callback_data.Buf = callback_buf;
                    callback_data.BufTextLen = state->CurLenA;
                    callback_data.BufSize = state->BufCapacityA;
                    callback_data.BufDirty = false;

                    // We have to convert from wchar-positions to UTF-8-positions, which can be pretty slow (an incentive to ditch the ImWchar buffer, see https://github.com/nothings/stb/issues/188)
                    ImWchar* text = state->TextW.Data;
                    const int utf8_cursor_pos = callback_data.CursorPos = ImTextCountUtf8BytesFromStr(text, text + state->Stb.cursor);
                    const int utf8_selection_start = callback_data.SelectionStart = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_start);
                    const int utf8_selection_end = callback_data.SelectionEnd = ImTextCountUtf8BytesFromStr(text, text + state->Stb.select_end);

                    // Call user code
                    callback(&callback_data);

                    // Read back what user may have modified
                    callback_buf = is_readonly ? buf : state->TextA.Data; // Pointer may have been invalidated by a resize callback
                    IM_ASSERT(callback_data.Buf == callback_buf);         // Invalid to modify those fields
                    IM_ASSERT(callback_data.BufSize == state->BufCapacityA);
                    IM_ASSERT(callback_data.Flags == flags);
                    const bool buf_dirty = callback_data.BufDirty;
                    if (callback_data.CursorPos != utf8_cursor_pos || buf_dirty) { state->Stb.cursor = ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.CursorPos); state->CursorFollow = true; }
                    if (callback_data.SelectionStart != utf8_selection_start || buf_dirty) { state->Stb.select_start = (callback_data.SelectionStart == callback_data.CursorPos) ? state->Stb.cursor : ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionStart); }
                    if (callback_data.SelectionEnd != utf8_selection_end || buf_dirty) { state->Stb.select_end = (callback_data.SelectionEnd == callback_data.SelectionStart) ? state->Stb.select_start : ImTextCountCharsFromUtf8(callback_data.Buf, callback_data.Buf + callback_data.SelectionEnd); }
                    if (buf_dirty)
                    {
                        IM_ASSERT(!is_readonly);
                        IM_ASSERT(callback_data.BufTextLen == (int)strlen(callback_data.Buf)); // You need to maintain BufTextLen if you change the text!
                        input_text_reconcile_undo_state_after_user_callback(state, callback_data.Buf, callback_data.BufTextLen); // FIXME: Move the rest of this block inside function and rename to InputTextReconcileStateAfterUserCallback() ?
                        if (callback_data.BufTextLen > backup_current_text_length && is_resizable)
                            state->TextW.resize(state->TextW.Size + (callback_data.BufTextLen - backup_current_text_length)); // Worse case scenario resize
                        state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, callback_data.Buf, NULL);
                        state->CurLenA = callback_data.BufTextLen;  // Assume correct length and valid UTF-8 from user, saves us an extra strlen()
                        state->CursorAnimReset();
                    }
                }
            }

            // Will copy result string if modified
            if (!is_readonly && strcmp(state->TextA.Data, buf) != 0)
            {
                apply_new_text = state->TextA.Data;
                apply_new_text_length = state->CurLenA;
                value_changed = true;
            }
        }
    }

    // Handle reapplying final data on deactivation (see InputTextDeactivateHook() for details)
    if (g.InputTextDeactivatedState.ID == id)
    {
        if (g.ActiveId != id && IsItemDeactivatedAfterEdit() && !is_readonly && strcmp(g.InputTextDeactivatedState.TextA.Data, buf) != 0)
        {
            apply_new_text = g.InputTextDeactivatedState.TextA.Data;
            apply_new_text_length = g.InputTextDeactivatedState.TextA.Size - 1;
            value_changed = true;
            //IMGUI_DEBUG_LOG("InputText(): apply Deactivated data for 0x%08X: \"%.*s\".\n", id, apply_new_text_length, apply_new_text);
        }
        g.InputTextDeactivatedState.ID = 0;
    }

    // Copy result to user buffer. This can currently only happen when (g.ActiveId == id)
    if (apply_new_text != NULL)
    {
        // We cannot test for 'backup_current_text_length != apply_new_text_length' here because we have no guarantee that the size
        // of our owned buffer matches the size of the string object held by the user, and by design we allow InputText() to be used
        // without any storage on user's side.
        IM_ASSERT(apply_new_text_length >= 0);
        if (is_resizable)
        {
            ImGuiInputTextCallbackData callback_data;
            callback_data.Ctx = &g;
            callback_data.EventFlag = ImGuiInputTextFlags_CallbackResize;
            callback_data.Flags = flags;
            callback_data.Buf = buf;
            callback_data.BufTextLen = apply_new_text_length;
            callback_data.BufSize = ImMax(buf_size, apply_new_text_length + 1);
            callback_data.UserData = callback_user_data;
            callback(&callback_data);
            buf = callback_data.Buf;
            buf_size = callback_data.BufSize;
            apply_new_text_length = ImMin(callback_data.BufTextLen, buf_size - 1);
            IM_ASSERT(apply_new_text_length <= buf_size);
        }
        //IMGUI_DEBUG_PRINT("InputText(\"%s\"): apply_new_text length %d\n", label, apply_new_text_length);

        // If the underlying buffer resize was denied or not carried to the next frame, apply_new_text_length+1 may be >= buf_size.
        ImStrncpy(buf, apply_new_text, ImMin(apply_new_text_length + 1, buf_size));
    }

    // Release active ID at the end of the function (so e.g. pressing Return still does a final application of the value)
    // Otherwise request text input ahead for next frame.
    if (g.ActiveId == id && clear_active_id)
        ClearActiveID();
    else if (g.ActiveId == id)
        g.WantTextInputNextFrame = 1;

    static std::map<ImGuiID, float> anim;
    float& it_anim = anim[id];

    // Render frame
    if (!is_multiline)
    {
        window->DrawList->AddText(YuanerSet->inter_semibold_min_font, YuanerSet->inter_semibold_min_font->FontSize, rect.Min - ImVec2(1, 3), gui->get_clr(clr->input_label), label);
        window->DrawList->AddRectFilled(clickable.Min, clickable.Max, gui->get_clr(clr->input_background), YuanerSet->widgets_rounding);
    }

    const ImVec4 clip_rect(clickable.Min.x, clickable.Min.y, clickable.Min.x + inner_size.x, clickable.Min.y + inner_size.y); // Not using frame_bb.Max because we have adjusted size
    ImVec2 draw_pos = is_multiline ? draw_window->DC.CursorPos : clickable.Min + style.FramePadding + ImVec2(10, 11);
    ImVec2 text_size(0.0f, 0.0f);

    // Set upper limit of single-line InputTextEx() at 2 million characters strings. The current pathological worst case is a long line
    // without any carriage return, which would makes ImFont::RenderText() reserve too many vertices and probably crash. Avoid it altogether.
    // Note that we only use this limit on single-line InputText(), so a pathologically large line on a InputTextMultiline() would still crash.
    const int buf_display_max_length = 2 * 1024 * 1024;
    const char* buf_display = buf_display_from_state ? state->TextA.Data : buf; //-V595
    const char* buf_display_end = NULL; // We have specialized paths below for setting the length
    if (is_displaying_hint)
    {
        buf_display = hint;
        buf_display_end = hint + strlen(hint);
    }

    // Render text. We currently only render selection when the widget is active or while scrolling.
    // FIXME: We could remove the '&& render_cursor' to keep rendering selection when inactive.
    if (render_cursor || render_selection)
    {
        IM_ASSERT(state != NULL);
        if (!is_displaying_hint)
            buf_display_end = buf_display + state->CurLenA;

        // Render text (with cursor and selection)
        // This is going to be messy. We need to:
        // - Display the text (this alone can be more easily clipped)
        // - Handle scrolling, highlight selection, display cursor (those all requires some form of 1d->2d cursor position calculation)
        // - Measure text height (for scrollbar)
        // We are attempting to do most of that in **one main pass** to minimize the computation cost (non-negligible for large amount of text) + 2nd pass for selection rendering (we could merge them by an extra refactoring effort)
        // FIXME: This should occur on buf_display but we'd need to maintain cursor/select_start/select_end for UTF-8.
        const ImWchar* text_begin = state->TextW.Data;
        ImVec2 cursor_offset, select_start_offset;

        {
            // Find lines numbers straddling 'cursor' (slot 0) and 'select_start' (slot 1) positions.
            const ImWchar* searches_input_ptr[2] = { NULL, NULL };
            int searches_result_line_no[2] = { -1000, -1000 };
            int searches_remaining = 0;
            if (render_cursor)
            {
                searches_input_ptr[0] = text_begin + state->Stb.cursor;
                searches_result_line_no[0] = -1;
                searches_remaining++;
            }
            if (render_selection)
            {
                searches_input_ptr[1] = text_begin + ImMin(state->Stb.select_start, state->Stb.select_end);
                searches_result_line_no[1] = -1;
                searches_remaining++;
            }

            // Iterate all lines to find our line numbers
            // In multi-line mode, we never exit the loop until all lines are counted, so add one extra to the searches_remaining counter.
            searches_remaining += is_multiline ? 1 : 0;
            int line_count = 0;
            //for (const ImWchar* s = text_begin; (s = (const ImWchar*)wcschr((const wchar_t*)s, (wchar_t)'\n')) != NULL; s++)  // FIXME-OPT: Could use this when wchar_t are 16-bit
            for (const ImWchar* s = text_begin; *s != 0; s++)
                if (*s == '\n')
                {
                    line_count++;
                    if (searches_result_line_no[0] == -1 && s >= searches_input_ptr[0]) { searches_result_line_no[0] = line_count; if (--searches_remaining <= 0) break; }
                    if (searches_result_line_no[1] == -1 && s >= searches_input_ptr[1]) { searches_result_line_no[1] = line_count; if (--searches_remaining <= 0) break; }
                }
            line_count++;
            if (searches_result_line_no[0] == -1)
                searches_result_line_no[0] = line_count;
            if (searches_result_line_no[1] == -1)
                searches_result_line_no[1] = line_count;

            // Calculate 2d position by finding the beginning of the line and measuring distance
            cursor_offset.x = input_text_calc_text_size_w(&g, ImStrbolW(searches_input_ptr[0], text_begin), searches_input_ptr[0]).x;
            cursor_offset.y = searches_result_line_no[0] * g.FontSize;
            if (searches_result_line_no[1] >= 0)
            {
                select_start_offset.x = input_text_calc_text_size_w(&g, ImStrbolW(searches_input_ptr[1], text_begin), searches_input_ptr[1]).x;
                select_start_offset.y = searches_result_line_no[1] * g.FontSize;
            }

            // Store text height (note that we haven't calculated text width at all, see GitHub issues #383, #1224)
            if (is_multiline)
                text_size = ImVec2(inner_size.x, line_count * g.FontSize);
        }

        // Scroll
        if (render_cursor && state->CursorFollow)
        {
            // Horizontal scroll in chunks of quarter width
            if (!(flags & ImGuiInputTextFlags_NoHorizontalScroll))
            {
                const float scroll_increment_x = inner_size.x * 0.25f;
                const float visible_width = inner_size.x - style.FramePadding.x;
                if (cursor_offset.x < state->ScrollX)
                    state->ScrollX = IM_TRUNC(ImMax(0.0f, cursor_offset.x - scroll_increment_x));
                else if (cursor_offset.x - visible_width >= state->ScrollX)
                    state->ScrollX = IM_TRUNC(cursor_offset.x - visible_width + scroll_increment_x);
            }
            else
            {
                state->ScrollX = 0.0f;
            }

            // Vertical scroll
            if (is_multiline)
            {
                // Test if cursor is vertically visible
                if (cursor_offset.y - g.FontSize < scroll_y)
                    scroll_y = ImMax(0.0f, cursor_offset.y - g.FontSize);
                else if (cursor_offset.y - (inner_size.y - style.FramePadding.y * 2.0f) >= scroll_y)
                    scroll_y = cursor_offset.y - inner_size.y + style.FramePadding.y * 2.0f;
                const float scroll_max_y = ImMax((text_size.y + style.FramePadding.y * 2.0f) - inner_size.y, 0.0f);
                scroll_y = ImClamp(scroll_y, 0.0f, scroll_max_y);
                draw_pos.y += (draw_window->Scroll.y - scroll_y);   // Manipulate cursor pos immediately avoid a frame of lag
                draw_window->Scroll.y = scroll_y;
            }

            state->CursorFollow = false;
        }

        // Draw selection
        const ImVec2 draw_scroll = ImVec2(state->ScrollX, 0.0f);
        if (render_selection)
        {
            const ImWchar* text_selected_begin = text_begin + ImMin(state->Stb.select_start, state->Stb.select_end);
            const ImWchar* text_selected_end = text_begin + ImMax(state->Stb.select_start, state->Stb.select_end);

            ImU32 bg_color = GetColorU32(ImGuiCol_TextSelectedBg, render_cursor ? 1.0f : 0.6f); // FIXME: current code flow mandate that render_cursor is always true here, we are leaving the transparent one for tests.
            float bg_offy_up = is_multiline ? 0.0f : -1.0f;    // FIXME: those offsets should be part of the style? they don't play so well with multi-line selection.
            float bg_offy_dn = is_multiline ? 0.0f : 2.0f;
            ImVec2 rect_pos = draw_pos + select_start_offset - draw_scroll;
            for (const ImWchar* p = text_selected_begin; p < text_selected_end; )
            {
                if (rect_pos.y > clip_rect.w + g.FontSize)
                    break;
                if (rect_pos.y < clip_rect.y)
                {
                    //p = (const ImWchar*)wmemchr((const wchar_t*)p, '\n', text_selected_end - p);  // FIXME-OPT: Could use this when wchar_t are 16-bit
                    //p = p ? p + 1 : text_selected_end;
                    while (p < text_selected_end)
                        if (*p++ == '\n')
                            break;
                }
                else
                {
                    ImVec2 rect_size = input_text_calc_text_size_w(&g, p, text_selected_end, &p, NULL, true);
                    if (rect_size.x <= 0.0f) rect_size.x = IM_TRUNC(g.Font->GetCharAdvance((ImWchar)' ') * 0.50f); // So we can see selected empty lines
                    ImRect rect(rect_pos + ImVec2(0.0f, bg_offy_up - g.FontSize), rect_pos + ImVec2(rect_size.x, bg_offy_dn));
                    rect.ClipWith(clip_rect);
                    if (rect.Overlaps(clip_rect))
                        draw_window->DrawList->AddRectFilled(rect.Min, rect.Max + ImVec2(0, 2), gui->get_clr(clr->accent, 0.3f), 2.f);
                }
                rect_pos.x = draw_pos.x - draw_scroll.x;
                rect_pos.y += g.FontSize;
            }
        }

        // We test for 'buf_display_max_length' as a way to avoid some pathological cases (e.g. single-line 1 MB string) which would make ImDrawList crash.
        if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
            draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos - draw_scroll, gui->get_clr(clr->input_value), buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);

        // Draw blinking cursor
        if (render_cursor)
        {
            state->CursorAnim += io.DeltaTime;
            bool cursor_is_visible = (!g.IO.ConfigInputTextCursorBlink) || (state->CursorAnim <= 0.0f) || ImFmod(state->CursorAnim, 1.20f) <= 0.80f;
            ImVec2 cursor_screen_pos = ImTrunc(draw_pos + cursor_offset - draw_scroll);
            ImRect cursor_screen_rect(cursor_screen_pos.x, cursor_screen_pos.y - g.FontSize + 0.5f, cursor_screen_pos.x + 1.0f, cursor_screen_pos.y - 1.5f);
            it_anim = ImLerp(it_anim, cursor_screen_rect.Min.x - clickable.Min.x, gui->fixed_speed(20.f));
            if (cursor_is_visible && cursor_screen_rect.Overlaps(clip_rect))
                draw_window->DrawList->AddLine(clickable.Min + ImVec2(it_anim, 12), ImVec2(clickable.Min.x + it_anim, clickable.Max.y - 12), GetColorU32(ImGuiCol_Text));

            // Notify OS of text input position for advanced IME (-1 x offset so that Windows IME can cover our cursor. Bit of an extra nicety.)
            if (!is_readonly)
            {
                g.PlatformImeData.WantVisible = true;
                g.PlatformImeData.InputPos = ImVec2(cursor_screen_pos.x - 1.0f, cursor_screen_pos.y - g.FontSize);
                g.PlatformImeData.InputLineHeight = g.FontSize;
            }
        }
    }
    else
    {
        // Render text only (no selection, no cursor)
        if (is_multiline)
            text_size = ImVec2(inner_size.x, input_text_calc_text_len_and_line_count(buf_display, &buf_display_end) * g.FontSize); // We don't need width
        else if (!is_displaying_hint && g.ActiveId == id)
            buf_display_end = buf_display + state->CurLenA;
        else if (!is_displaying_hint)
            buf_display_end = buf_display + strlen(buf_display);

        if (is_multiline || (buf_display_end - buf_display) < buf_display_max_length)
            draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos, gui->get_clr(clr->input_value), buf_display, buf_display_end, 0.0f, is_multiline ? NULL : &clip_rect);
    }

    if (is_password && !is_displaying_hint)
        PopFont();

    if (is_multiline)
    {
        // For focus requests to work on our multiline we need to ensure our child ItemAdd() call specifies the ImGuiItemFlags_Inputable (ref issue #4761)...
        Dummy(ImVec2(text_size.x, text_size.y + style.FramePadding.y));
        g.NextItemData.ItemFlags |= ImGuiItemFlags_Inputable | ImGuiItemFlags_NoTabStop;
        EndChild();
        item_data_backup.StatusFlags |= (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_HoveredWindow);

        // ...and then we need to undo the group overriding last item data, which gets a bit messy as EndGroup() tries to forward scrollbar being active...
        // FIXME: This quite messy/tricky, should attempt to get rid of the child window.
        EndGroup();
        if (g.LastItemData.ID == 0)
        {
            g.LastItemData.ID = id;
            g.LastItemData.InFlags = item_data_backup.InFlags;
            g.LastItemData.StatusFlags = item_data_backup.StatusFlags;
        }
    }

    // Log as text
    if (g.LogEnabled && (!is_password || is_displaying_hint))
    {
        LogSetNextTextDecoration("{", "}");
        LogRenderedText(&draw_pos, buf_display, buf_display_end);
    }

    if (value_changed && !(flags & ImGuiInputTextFlags_NoMarkEdited))
        MarkItemEdited(id);

    PopFont();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Inputable);
    if ((flags & ImGuiInputTextFlags_EnterReturnsTrue) != 0)
        return validated;
    else
        return value_changed;
}

bool c_widgets::text_field(std::string_view label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline)); // call InputTextMultiline()
    return input_text_ex(label.data(), NULL, buf, (int)buf_size, ImVec2(0, 0), flags, callback, user_data);
}

static void color_edit_restore_hs(const float* col, float* H, float* S, float* V)
{
    ImGuiContext& g = *GImGui;

    if (*S == 0.0f || (*H == 0.0f && g.ColorEditSavedHue == 1))
        *H = g.ColorEditSavedHue;

    if (*V == 0.0f) *S = g.ColorEditSavedSat;
}

bool color_picker(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col)
{
    struct picker_state
    {
        ImVec2 sv;
        float hue, alpha;
    };

    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImDrawList* draw_list = window->DrawList;

    ImGuiStyle& style = g.Style;
    ImGuiIO& io = g.IO;

    const float width = CalcItemWidth();
    g.NextItemData.ClearFlags();

    PushID(label);
    BeginGroup();
    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 5));

    if (!(flags & ImGuiColorEditFlags_NoSidePreview)) flags |= ImGuiColorEditFlags_NoSmallPreview;

    if (!(flags & ImGuiColorEditFlags_NoOptions)) ColorPickerOptionsPopup(col, flags);

    if (!(flags & ImGuiColorEditFlags_PickerMask_))
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_PickerMask_;
    if (!(flags & ImGuiColorEditFlags_InputMask_))
        flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_InputMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_InputMask_;
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_));
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));
    if (!(flags & ImGuiColorEditFlags_NoOptions)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

    int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
    bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
    ImVec2 picker_pos = window->DC.CursorPos + ImVec2(0, 0);
    ImVec2 bar_pos = window->DC.CursorPos + ImVec2(0, 135 * DPI);

    float picker_size = 160.f * DPI;
    float bar_width = 160.f * DPI;
    float bar_height = 4.f * DPI;

    float backup_initial_col[4];
    memcpy(backup_initial_col, col, components * sizeof(float));

    float H = col[0], S = col[1], V = col[2];
    float R = col[0], G = col[1], B = col[2];
    if (flags & ImGuiColorEditFlags_InputRGB)
    {
        ColorConvertRGBtoHSV(R, G, B, H, S, V);
        color_edit_restore_hs(col, &H, &S, &V);
    }
    else if (flags & ImGuiColorEditFlags_InputHSV)
    {
        ColorConvertHSVtoRGB(H, S, V, R, G, B);
    }

    bool value_changed = false, value_changed_h = false, value_changed_sv = false;

    PushItemFlag(ImGuiItemFlags_NoNav, true);

    SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y));
    InvisibleButton("sv", ImVec2(picker_size, picker_size));
    if (IsItemActive())
    {
        S = ImSaturate((io.MousePos.x - picker_pos.x) / (picker_size - 1));
        V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (picker_size - 1));

        if (g.ColorEditSavedColor == ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0))) H = g.ColorEditSavedHue;
        value_changed = value_changed_sv = true;
    }

    //未开启透明通道后 高度
    SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y + 164.f * DPI));
    InvisibleButton("hue", ImVec2(bar_width, bar_height + 8 * DPI));
    if (IsItemActive())
    {
        H = 1.f - ImSaturate((io.MousePos.x - picker_pos.x) / (bar_width - 1));
        value_changed = value_changed_h = true;
    }

    if (alpha_bar)
    {
        //开启透明通道后 高度
        SetCursorScreenPos(ImVec2(picker_pos.x, picker_pos.y + 177.f * DPI));
        InvisibleButton("alpha", ImVec2(bar_width, bar_height + 8 * DPI));
        if (IsItemActive())
        {
            col[3] = ImSaturate((io.MousePos.x - picker_pos.x) / (bar_width - 1));
            value_changed = true;
        }
    }
    PopItemFlag();
    if (value_changed_h || value_changed_sv)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            ColorConvertHSVtoRGB(H, S, V, col[0], col[1], col[2]);
            g.ColorEditSavedHue = H;
            g.ColorEditSavedSat = S;
            g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0));
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            col[0] = H;
            col[1] = S;
            col[2] = V;
        }
    }

    bool value_changed_fix_hue_wrap = false;
    ImGuiColorEditFlags sub_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
    ImGuiColorEditFlags sub_flags = (flags & sub_flags_to_forward) | ImGuiColorEditFlags_NoPicker;

    if (flags & ImGuiColorEditFlags_DisplayRGB || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
        if (gui->color_edit("##rgb", col, sub_flags | ImGuiColorEditFlags_DisplayRGB))
        {
            value_changed_fix_hue_wrap = (g.ActiveId != 0 && !g.ActiveIdAllowOverlap);
            value_changed = true;
        }
    if (flags & ImGuiColorEditFlags_DisplayHSV || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
        value_changed |= gui->color_edit("##hsv", col, sub_flags | ImGuiColorEditFlags_DisplayHSV);
    if (flags & ImGuiColorEditFlags_DisplayHex || (flags & ImGuiColorEditFlags_DisplayMask_) == 0)
        value_changed |= gui->color_edit("##hex", col, sub_flags | ImGuiColorEditFlags_DisplayHex);

    if (value_changed_fix_hue_wrap && (flags & ImGuiColorEditFlags_InputRGB))
    {
        float new_H, new_S, new_V;
        ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);
        if (new_H <= 0 && H > 0)
        {
            if (new_V <= 0 && V != new_V)
                ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
            else if (new_S <= 0)
                ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
        }
    }

    if (value_changed)
    {
        if (flags & ImGuiColorEditFlags_InputRGB)
        {
            R = col[0];
            G = col[1];
            B = col[2];
            ColorConvertRGBtoHSV(R, G, B, H, S, V);
            color_edit_restore_hs(col, &H, &S, &V);
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            H = col[0];
            S = col[1];
            V = col[2];
            ColorConvertHSVtoRGB(H, S, V, R, G, B);
        }
    }

    const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
    const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
    const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
    const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
    const ImU32 col_hues[7] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(0,0,255,style_alpha8),IM_COL32(0,255,255,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

    ImVec4 hue_color_f(1, 1, 1, style.Alpha); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
    ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);
    ImU32 user_col32_striped_of_alpha = ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha));

    ImVec2 sv_cursor_pos;
    const int vtx_idx_0 = draw_list->VtxBuffer.Size;
    draw_list->AddRectFilled(picker_pos, picker_pos + ImVec2(picker_size, picker_size - 2), col_white, 3.0f);
    const int vtx_idx_1 = draw_list->VtxBuffer.Size;
    ShadeVertsLinearColorGradientKeepAlpha(draw_list, vtx_idx_0, vtx_idx_1, picker_pos, picker_pos + ImVec2(picker_size, 0.0f), col_white, hue_color32);

    add_rect_filled_multi_color(draw_list, picker_pos, picker_pos + ImVec2(picker_size, picker_size), 0, 0, col_black, col_black, 3.f);

    sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * picker_size), picker_pos.x + 2, picker_pos.x + picker_size - 2);
    sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * (picker_size)), picker_pos.y + 2, picker_pos.y + picker_size - 2);

    static std::map<ImGuiID, picker_state> anim;
    picker_state& state = anim[GetID(label)];


    //未开启透明通道
    for (int i = 0; i < 6; ++i)
        add_rect_filled_multi_color(draw_list, ImVec2(picker_pos.x + i * (bar_width / 6) - (i == 5 ? 1 : 0), picker_pos.y + (168 * DPI)), ImVec2(picker_pos.x + (i + 1) * (bar_width / 6) + (i == 0 ? 1 : 0), picker_pos.y + (168 * DPI) + bar_height), col_hues[i], col_hues[i + 1], col_hues[i + 1], col_hues[i], 2.f, i == 0 ? ImDrawFlags_RoundCornersLeft : i == 5 ? ImDrawFlags_RoundCornersRight : ImDrawFlags_RoundCornersNone);
    float bar0_line_x = IM_ROUND(picker_pos.x + (1.f - H) * bar_width);
    bar0_line_x = ImClamp(bar0_line_x, picker_pos.x + 3.f, picker_pos.x + 156.f * DPI);

    state.hue = ImLerp(state.hue, bar0_line_x, g.IO.DeltaTime * 20.f);
    draw_list->AddCircleFilled(ImVec2(state.hue, picker_pos.y + 170 * DPI), 5 * DPI, GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)));

    state.sv = ImLerp(state.sv, sv_cursor_pos, g.IO.DeltaTime * 10.f);

    draw_list->AddShadowCircle(state.sv, 5.f * DPI, GetColorU32(ImVec4(0.f, 0.f, 0.f, 0.78f)), 20.f, ImVec2(0, 2));
    draw_list->AddCircle(state.sv, 5.f * DPI, col_white, 12, 4.f);
    draw_list->AddCircleFilled(state.sv, 5.f * DPI, user_col32_striped_of_alpha, 12);


    //开启透明通道 单独的调色盘
    if (alpha_bar)
    {
        float alpha = ImSaturate(col[3]);
        add_rect_filled_multi_color(draw_list, picker_pos + ImVec2(0, 191 * DPI), picker_pos + ImVec2(bar_width, (191 * DPI)  + bar_height), col_black, user_col32_striped_of_alpha, user_col32_striped_of_alpha, col_black, 2.f);
        float bar1_line_x = IM_ROUND(picker_pos.x + alpha * bar_width);
        bar1_line_x = ImClamp(bar1_line_x, picker_pos.x + 3.f, picker_pos.x + 156.f * DPI);

        state.alpha = ImLerp(state.alpha, bar1_line_x, g.IO.DeltaTime * 20.f);
        draw_list->AddCircleFilled(ImVec2(state.alpha, picker_pos.y + 193 * DPI), 5 * DPI, GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)));
    }

    PopStyleVar();
    EndGroup();

    if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0) value_changed = false;
    if (value_changed && g.LastItemData.ID != 0)  MarkItemEdited(g.LastItemData.ID);

    PopID();

    return value_changed;
    }

bool color_button(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags, const ImVec2& size_arg, bool popup = false)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(desc_id);
    const float default_size = GetFrameHeight();

    const float width = GetWindowWidth() - g.Style.WindowPadding.x * 2;
    const ImVec2 rect_size = ImVec2(33 * DPI, 20 * DPI);
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect rect(pos, pos + ImVec2(width, 20 * DPI));

    ItemSize(ImRect(rect.Min, rect.Max - ImVec2(0, 0)));
    if (!ItemAdd((flags & ImGuiColorEditFlags_NoLabel) ? rect : rect, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(rect, id, &hovered, &held);

    if (flags & ImGuiColorEditFlags_NoAlpha) flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

    ImVec4 col_rgb = col;
    if (flags & ImGuiColorEditFlags_InputHSV) ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

    window->DrawList->AddText(rect.Min + ImVec2(0, popup ? 3 : 1), gui->get_clr(clr->color_label), desc_id);

    RenderColorRectWithAlphaCheckerboard(window->DrawList, ImVec2(rect.Max.x - rect_size.x, rect.Min.y), rect.Max, gui->get_clr(col_rgb), ImMin(17 * DPI, 17 * DPI) / 2.99f, ImVec2(0.f, 0.f), YuanerSet->widgets_rounding);

    return pressed;
}

bool c_widgets::color_edit(std::string_view label, float col[4], ImGuiColorEditFlags flags, bool popup)
{
    struct color_state
    {
        bool active = false;
        bool hovered = false;
        float alpha = 0.f;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float square_sz = GetFrameHeight();
    const float w_full = CalcItemWidth();
    const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
    const float w_inputs = w_full - w_button;
    const char* label_display_end = FindRenderedTextEnd(label.data());
    g.NextItemData.ClearFlags();

    /*
    POINT cursorPos;
    COLORREF color;
*/

    BeginGroup();
    PushID(label.data());

    const ImGuiColorEditFlags flags_untouched = flags;
    if (flags & ImGuiColorEditFlags_NoInputs) flags = (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

    if (!(flags & ImGuiColorEditFlags_NoOptions)) ColorEditOptionsPopup(col, flags);

    if (!(flags & ImGuiColorEditFlags_DisplayMask_)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DisplayMask_);
    if (!(flags & ImGuiColorEditFlags_DataTypeMask_)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DataTypeMask_);
    if (!(flags & ImGuiColorEditFlags_PickerMask_)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_);
    if (!(flags & ImGuiColorEditFlags_InputMask_)) flags |= (g.ColorEditOptions & ImGuiColorEditFlags_InputMask_);

    flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_));

    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_DisplayMask_));
    IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));

    const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
    const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
    const int components = alpha ? 4 : 3;

    float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
    if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
        ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
    else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
    {
        ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
        color_edit_restore_hs(col, &f[0], &f[1], &f[2]);
    }
    int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

    bool value_changed = false;
    bool value_changed_as_float = false;

    const ImVec2 pos = window->DC.CursorPos;
    const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;

    ImGuiWindow* picker_active_window = NULL;
    if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
    {
        const float button_offset_x = ((flags & ImGuiColorEditFlags_NoInputs) || (style.ColorButtonPosition == ImGuiDir_Left)) ? 0.0f : w_inputs + style.ItemInnerSpacing.x;
        const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);

        PushFont(popup ? YuanerSet->inter_semibold_font : YuanerSet->inter_semibold_min_font);
        //按钮大小
        if (color_button(label.data(), col_v4, flags, ImVec2(0, 0), popup))
        {
            if (!(flags & ImGuiColorEditFlags_NoPicker))
            {
                // Store current color and open a picker
                g.ColorPickerRef = col_v4;
                OpenPopup("picker");
                SetNextWindowPos(g.LastItemData.Rect.GetBL() + ImVec2(0.0f, 0.f));
            }
        }
        PopFont();

        static std::map<ImGuiID, color_state> anim;
        color_state& state = anim[GetID(label.data())];

        PushStyleColor(ImGuiCol_PopupBg, gui->get_clr(clr->color_background));
        PushStyleVar(ImGuiStyleVar_PopupRounding, YuanerSet->widgets_rounding);
        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 5));

        if (BeginPopup("picker"))
        {
            if (g.CurrentWindow->BeginCount == 1)
            {
                picker_active_window = g.CurrentWindow;

                ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
                ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
                SetNextItemWidth(square_sz * (12.0f * DPI));

                //颜色选择器
                value_changed |= color_picker("##picker", col, picker_flags, &g.ColorPickerRef.x);

                static std::vector<float> color_x, color_y, color_z, color_a;

                static bool add_status = true;


                if (IsItemHovered() && GetIO().MouseClicked[1]) add_status = !add_status;

                for (int i = 0; i < color_x.size(); i++)
                {
                    std::string number_x = std::to_string(color_x[i]),
                        number_y = std::to_string(color_y[i]),
                        number_z = std::to_string(color_z[i]),
                        number_a = std::to_string(color_a[i]);

                    std::string name_box = std::to_string(i);


                    if ((i + 1) % 7 != 0) ImGui::SameLine(0, 5);

                }

            }
            EndPopup();
        }
        PopStyleVar(3);
        PopStyleColor();
    }

    if (value_changed && picker_active_window == NULL)
    {
        if (!value_changed_as_float)
            for (int n = 0; n < 4; n++)
                f[n] = i[n] / 255.0f;
        if ((flags & ImGuiColorEditFlags_DisplayHSV) && (flags & ImGuiColorEditFlags_InputRGB))
        {
            g.ColorEditSavedHue = f[0];
            g.ColorEditSavedSat = f[1];
            ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
            g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(f[0], f[1], f[2], 0));
        }
        if ((flags & ImGuiColorEditFlags_DisplayRGB) && (flags & ImGuiColorEditFlags_InputHSV))
            ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

        col[0] = f[0];
        col[1] = f[1];
        col[2] = f[2];
        if (alpha)
            col[3] = f[3];
    }

    PopID();
    EndGroup();

    if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
        g.LastItemData.ID = g.ActiveId;

    if (value_changed && g.LastItemData.ID != 0)
        MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

bool c_widgets::button(std::string_view label, const ImVec2& size_arg)
{

    struct button_state
    {
        ImVec4 label_clr = clr->button_label;
        float bg_alpha = 0.f;
        bool clicked = false;
    };

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());

    const float width = GetWindowWidth() - g.Style.WindowPadding.x * 2;
    ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, pos + ImVec2(width, 30 * DPI));
    ItemSize(rect, style.FramePadding.y);
    if (!ItemAdd(rect, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(rect, id, &hovered, &held, 0);

    // Render

    static std::map<ImGuiID, button_state> anim;
    button_state& state = anim[id];

    if (pressed)
        state.clicked = true;

    state.bg_alpha = ImClamp(state.bg_alpha + (gui->fixed_speed(6.f) * (state.clicked ? 1.f : -1.f)), 0.f, 1.f);
    state.label_clr = ImLerp(state.label_clr, state.clicked ? clr->button_label_active : clr->button_label, gui->fixed_speed(12.f));

    if (state.bg_alpha >= 0.9f)
        state.clicked = false;

    window->DrawList->AddRectFilled(rect.Min, rect.Max, gui->get_clr(clr->button_background), YuanerSet->widgets_rounding);
    window->DrawList->AddRectFilled(rect.Min, rect.Max, gui->get_clr(clr->accent, 0.1f * state.bg_alpha), YuanerSet->widgets_rounding);
    window->DrawList->AddRect(rect.Min, rect.Max, gui->get_clr(clr->accent, state.bg_alpha), YuanerSet->widgets_rounding);

    PushFont(YuanerSet->inter_semibold_min_font);
    PushStyleColor(ImGuiCol_Text, gui->get_clr(state.label_clr));
    RenderTextClipped(rect.Min - ImVec2(0, 1), rect.Max - ImVec2(0, 1), label.data(), NULL, NULL, ImVec2(0.5f, 0.5f));
    PopStyleColor();
    PopFont();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}
struct button_state
{
    ImVec4 background, text;
    float alpha;
};
bool c_widgets::IconButton(const char* icon, const ImVec2& size_arg, ImGuiButtonFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();

    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(icon);
    const ImVec2 label_size = CalcTextSize(icon, NULL, true), pos = window->DC.CursorPos;

    ImVec2 size = CalcItemSize(size_arg, label_size.x, label_size.y);

    const ImRect bb(pos, pos + size);

    ItemSize(size, 0.f);
    if (!ItemAdd(bb, id)) return false;

    bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    static std::map<ImGuiID, button_state> anim;
    button_state& state = anim[id];

    state.text = ImLerp(state.text, IsItemActive() || hovered ? ImColor(255, 255, 255) : ImColor(92, 107, 161, 125), gui->fixed_speed(6.f));

    PushFont(YuanerSet->icon);
    window->DrawList->AddText(ImVec2(bb.Min.x + (size.x - CalcTextSize(icon).x) / 2, bb.Max.y - CalcTextSize(icon).y - (size.y - CalcTextSize(icon).y) / 2), GetColorU32(state.text), icon);
    PopFont();

    return pressed;
}

inline float alphaColor = 0.f;
//旋转图像
void c_widgets::AddImageRotated(ImTextureID pic, float posX, float posY, float sizeX, float sizeY)
{
    auto style = ImGui::GetStyle();
    style.Alpha = alphaColor;
    SetCursorPos({ posX, posY });
    ImageRotation(pic, {sizeX, sizeY}, ImVec2(1, 1), ImVec2(0, 0), ImGui::GetColorU32(clr->accent), 0.1f);
}



