#include "editor.hpp"
#include <algorithm>
#include <cstdio>
#include <wx/dcbuffer.h>
#include <wx/clipbrd.h>

EditorCtrl::EditorCtrl(wxWindow *parent) : wxWindow(parent, wxID_ANY)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    editorFont = wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "JetBrains Mono");

    Bind(wxEVT_PAINT, &EditorCtrl::OnPaint, this);
    Bind(wxEVT_CHAR, &EditorCtrl::OnChar, this);
    Bind(wxEVT_KEY_DOWN, &EditorCtrl::OnKeyDown, this);
    Bind(wxEVT_LEFT_DOWN, &EditorCtrl::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &EditorCtrl::OnLeftUp, this);
    Bind(wxEVT_MOTION, &EditorCtrl::OnMouseMove, this);

    buffer = TextBuffer(L"\n");
    dragging = false;
}

// ==================================================
// Events
// ==================================================

void EditorCtrl::OnPaint(wxPaintEvent &event)
{
    wxAutoBufferedPaintDC dc(this);
    dc.SetFont(editorFont);
    charSize = dc.GetTextExtent("a");

    dc.SetBackground(wxBrush(wxColour(33, 33, 33)));
    dc.Clear();

    dc.SetTextForeground(wxColour(210, 210, 210));
    size_t i = 0;
    for (i = 0; i < buffer.GetLineCount(); i++) {
        RenderSelection(dc, i);
        RenderText(dc, 0, i, buffer.GetLine(i));
    }

    dc.SetTextForeground(wxColour(128, 128, 128));
    int gridHeight = GetGridSize().y;
    for (; i < gridHeight; i++) {
        RenderText(dc, 0, i, L"~");
    }

    RenderCaret(dc);
}

void EditorCtrl::OnChar(wxKeyEvent &event)
{
    wxChar ch = event.GetUnicodeKey();
    if (ch != WXK_NONE) {
        if (ch >= 32) {
            if (HasSelection()) {
                DeleteSelection();
                NormalizeCaret();
            }
            buffer.InsertChar(caretPos.line, caretPos.column, ch);
            MoveCaret(caretPos.line, caretPos.column + 1);
            InvalidateSelection();
        }
    } else {
        bool shift = event.ShiftDown();
        switch (event.GetKeyCode()) {
        case WXK_LEFT:
            MoveLeft(shift);
            break;
        case WXK_RIGHT:
            MoveRight(shift);
            break;
        case WXK_UP:
            MoveUp(shift);
            break;
        case WXK_DOWN:
            MoveDown(shift);
            break;
        }
    }
}

void EditorCtrl::OnKeyDown(wxKeyEvent &event)
{
    bool shift = event.ShiftDown();
    bool ctrl = event.ControlDown();

    switch (event.GetKeyCode()) {
    case 'A':
        if (ctrl) {
            SelectAll();
        } else {
            event.Skip();
        }
        break;
    case 'C':
        if (ctrl) {
            CopySelection();
        } else {
            event.Skip();
        }
        break;
    case 'X':
        if (ctrl) {
            CopySelection();
            DeleteSelection();
        } else {
            event.Skip();
        }
        break;
    case 'V':
        if (ctrl) {
            DeleteSelection();
            Paste();
        } else {
            event.Skip();
        }
        break;
    case 'Z':
        if (ctrl) {
            if (shift) {
                //
            } else {
                //
            }
            InvalidateSelection();
            Refresh();
        } else {
            event.Skip();
        }
        break;
    case 'Y':
        if (ctrl) {
            //
            InvalidateSelection();
            Refresh();
        } else {
            event.Skip();
        }
        break;
    case WXK_RETURN:
        InsertNewline();
        break;
    case WXK_DELETE:
        DeleteForward();
        break;
    case WXK_BACK:
        DeleteBackward();
        break;
    case WXK_HOME:
        MoveHome(shift, ctrl);
        break;
    case WXK_END:
        MoveEnd(shift, ctrl);
        break;
    default:
        event.Skip();
    }
}

void EditorCtrl::OnLeftDown(wxMouseEvent &event)
{
    SetFocus();
    int x = event.GetX() / charSize.GetWidth();
    int y = event.GetY() / charSize.GetHeight();
    MoveCaret(y, x);
    InvalidateSelection();
    dragging = true;
    CaptureMouse();
}

void EditorCtrl::OnLeftUp(wxMouseEvent &event)
{
    dragging = false;
    if (HasCapture())
        ReleaseMouse();
}

void EditorCtrl::OnMouseMove(wxMouseEvent &event)
{
    if (!dragging)
        return;

    MoveCaret(event.GetY() / charSize.GetHeight(), event.GetX() / charSize.GetWidth());
}

// ==================================================
// Element rendering
// ==================================================

void EditorCtrl::RenderText(wxDC &dc, int x, int y, std::wstring string)
{
    int pixelX = charSize.GetWidth() * x;
    int pixelY = charSize.GetHeight() * y;
    dc.DrawText(wxString(string), wxPoint(pixelX, pixelY));
}

void EditorCtrl::RenderCaret(wxDC &dc)
{
    int x = caretPos.column * charSize.GetWidth();
    int y = (caretPos.line + 1) * charSize.GetHeight() - 4;
    dc.SetBrush(wxBrush(wxColour(255, 255, 255)));
    dc.DrawRectangle(x, y, charSize.GetWidth(), 4);
}

void EditorCtrl::RenderSelection(wxDC &dc, int line)
{
    CaretPositionRange range = GetSelectionRange();

    if (!HasSelection() || line < range.start.line || line > range.end.line) {
        return;
    }

    size_t lineLength = buffer.GetLineLength(line);
    dc.SetBrush(wxBrush(wxColour(26, 97, 130)));

    int height = charSize.GetHeight();
    int y = line * height;

    if (range.start.line == range.end.line) {
        int x1 = range.start.column * charSize.GetWidth();
        int x2 = range.end.column * charSize.GetWidth();

        dc.DrawRectangle(x1, y, x2 - x1, height);
    } else if (line == range.start.line) {
        dc.DrawRectangle(range.start.column * charSize.GetWidth(), y, (lineLength - range.start.column) * charSize.GetWidth(), height);
    } else if (line == range.end.line) {
        dc.DrawRectangle(0, y, range.end.column * charSize.GetWidth(), height);
    } else {
        dc.DrawRectangle(0, y, lineLength * charSize.GetWidth(), height);
    }
}

// ==================================================
// Caret
// ==================================================

void EditorCtrl::MoveCaret(int line, int column)
{
    if (line < 0) {
        caretPos.line = 0;
    } else if (line >= buffer.GetLineCount()) {
        caretPos.line = buffer.GetLineCount() - 1;
    } else {
        caretPos.line = line;
    }

    int lineLength = buffer.GetLineLength(caretPos.line);

    if (column < 0) {
        caretPos.goalColumn = 0;
        caretPos.column = 0;
    } else {
        caretPos.goalColumn = column;
        if (caretPos.goalColumn > lineLength) {
            caretPos.column = lineLength;
        } else {
            caretPos.column = caretPos.goalColumn;
        }
    }

    Refresh();
}

void EditorCtrl::NormalizeCaret()
{
    MoveCaret(caretPos.line, caretPos.column);
}

// ==================================================
// Selection
// ==================================================

bool EditorCtrl::HasSelection()
{
    return selectionAnchor.line != -1 && caretPos != selectionAnchor;
}

CaretPositionRange EditorCtrl::GetSelectionRange()
{
    return CaretPositionRange(std::min(caretPos, selectionAnchor), std::max(caretPos, selectionAnchor));
}

void EditorCtrl::InvalidateSelection()
{
    selectionAnchor = caretPos;
}

void EditorCtrl::DeleteSelection()
{
    if (!HasSelection())
        return;

    CaretPositionRange range = GetSelectionRange();
    buffer.RemoveRange(range);
    MoveCaret(range.start.line, range.start.column);
    InvalidateSelection();
}

void EditorCtrl::SelectAll()
{
    MoveHome(false, true);
    MoveEnd(true, true);
}

std::wstring EditorCtrl::GetSelectionText()
{
    CaretPositionRange range = GetSelectionRange();
    return buffer.GetRange(range.start.line, range.start.column, range.end.line, range.end.column);
}

void EditorCtrl::CopySelection()
{
    if (wxTheClipboard->Open()) {
        wxTheClipboard->SetData(new wxTextDataObject(GetSelectionText()));
        wxTheClipboard->Close();
    }
}

void EditorCtrl::Paste()
{
    if (wxTheClipboard->Open()) {
        if (wxTheClipboard->IsSupported(wxDF_TEXT)) {
            wxTextDataObject data;
            wxTheClipboard->GetData(data);
            int lastLine, lastColumn;
            buffer.InsertString(caretPos.line, caretPos.column, data.GetText().ToStdWstring(), &lastLine, &lastColumn);
            MoveCaret(lastLine, lastColumn);
            InvalidateSelection();
        }
        wxTheClipboard->Close();
    }
}

// ==================================================
// Helpers
// ==================================================

void EditorCtrl::InsertNewline()
{
    if (HasSelection()) {
        DeleteSelection();
    }

    if (caretPos.column == 0) {
        buffer.InsertLine(caretPos.line);
        MoveCaret(caretPos.line + 1, caretPos.column);
        InvalidateSelection();
    } else if (caretPos.column == buffer.GetLineLength(caretPos.line)) {
        buffer.InsertLine(caretPos.line + 1);
        MoveCaret(caretPos.line + 1, caretPos.column);
        InvalidateSelection();
    } else {
        std::wstring line = buffer.GetLine(caretPos.line);
        std::wstring a = line.substr(0, caretPos.column);
        std::wstring b = line.substr(caretPos.column);

        buffer.SetLine(caretPos.line, a);
        buffer.InsertLine(caretPos.line + 1);
        buffer.SetLine(caretPos.line + 1, b);
        MoveCaret(caretPos.line + 1, 0);
        InvalidateSelection();
    }
}

void EditorCtrl::DeleteForward()
{
    if (HasSelection()) {
        DeleteSelection();
        return;
    }

    if (caretPos.column == buffer.GetLineLength(caretPos.line) && caretPos.line != buffer.GetLineCount() - 1) {
        buffer.MergeNext(caretPos.line);
    } else {
        buffer.RemoveChar(caretPos.line, caretPos.column);
    }
    NormalizeCaret();
}

void EditorCtrl::DeleteBackward()
{
    if (HasSelection()) {
        DeleteSelection();
        return;
    }

    if (caretPos.column == 0 && caretPos.line != 0) {
        int newColumn = buffer.GetLineLength(caretPos.line - 1);
        buffer.MergeNext(caretPos.line - 1);
        MoveCaret(caretPos.line - 1, newColumn);
        InvalidateSelection();
    } else {
        buffer.RemoveChar(caretPos.line, caretPos.column - 1);
        MoveCaret(caretPos.line, caretPos.column - 1);
        InvalidateSelection();
    }
}

// ==================================================
// Caret movement
// ==================================================

void EditorCtrl::MoveLeft(bool extendSelection)
{
    MoveCaret(caretPos.line, caretPos.column - 1);
    if (!extendSelection)
        InvalidateSelection();
}

void EditorCtrl::MoveRight(bool extendSelection)
{
    MoveCaret(caretPos.line, caretPos.column + 1);
    if (!extendSelection)
        InvalidateSelection();
}

void EditorCtrl::MoveUp(bool extendSelection)
{
    MoveCaret(caretPos.line - 1, caretPos.goalColumn);
    if (!extendSelection)
        InvalidateSelection();
}

void EditorCtrl::MoveDown(bool extendSelection)
{
    MoveCaret(caretPos.line + 1, caretPos.goalColumn);
    if (!extendSelection)
        InvalidateSelection();
}

void EditorCtrl::MoveHome(bool extendSelection, bool absolute)
{
    if (absolute)
        MoveCaret(0, 0);
    else
        MoveCaret(caretPos.line, 0);
    if (!extendSelection)
        InvalidateSelection();
}

void EditorCtrl::MoveEnd(bool extendSelection, bool absolute)
{
    if (absolute)
        MoveCaret(buffer.GetLineCount() - 1, buffer.GetLineLength(buffer.GetLineCount() - 1));
    else
        MoveCaret(caretPos.line, buffer.GetLineLength(caretPos.line));
    if (!extendSelection)
        InvalidateSelection();
}

// ==================================================

wxSize EditorCtrl::GetGridSize()
{
    int width, height;
    GetSize(&width, &height);
    return wxSize(width / charSize.x, height / charSize.y);
}
