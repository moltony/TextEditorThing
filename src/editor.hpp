#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "buffer.hpp"
#include "caret.hpp"
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <string>

class EditorCtrl : public wxWindow
{
public:
    EditorCtrl(wxWindow *parent);

private:
    wxFont editorFont;
    wxSize charSize;
    TextBuffer buffer;
    CaretPosition caretPos;

    bool dragging;
    CaretPosition selectionAnchor;

    void OnPaint(wxPaintEvent &event);
    void OnChar(wxKeyEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void OnLeftDown(wxMouseEvent &event);
    void OnLeftUp(wxMouseEvent &event);
    void OnMouseMove(wxMouseEvent &event);

    void RenderText(wxDC &dc, int x, int y, std::wstring string);
    void RenderCaret(wxDC &dc);
    void RenderSelection(wxDC &dc, int line);

    void MoveCaret(int line, int column);
    void NormalizeCaret();

    bool HasSelection();
    CaretPositionRange GetSelectionRange();
    void InvalidateSelection();
    void DeleteSelection();
    void SelectAll();
    std::wstring GetSelectionText();
    void CopySelection();
    void Paste();

    void InsertNewline();
    void DeleteForward();
    void DeleteBackward();
    void MoveLeft(bool extendSelection);
    void MoveRight(bool extendSelection);
    void MoveUp(bool extendSelection);
    void MoveDown(bool extendSelection);
    void MoveHome(bool extendSelection, bool absolute);
    void MoveEnd(bool extendSelection, bool absolute);

    wxSize GetGridSize();
};

#endif
