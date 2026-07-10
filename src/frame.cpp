#include "frame.hpp"
#include "editor.hpp"
#include <wx/splitter.h>
#include <wx/treectrl.h>

AppFrame::AppFrame() : wxFrame(nullptr, wxID_ANY, "Text editor thing")
{
    wxSplitterWindow *splitter = new wxSplitterWindow(this);

    wxTreeCtrl *sidebar = new wxTreeCtrl(splitter, wxID_ANY);
    wxTreeItemId root = sidebar->AddRoot("something");

    CreateStatusBar();
    SetStatusText("Status bar reserved for future use.");

    EditorCtrl *editor = new EditorCtrl(splitter);

    splitter->SplitVertically(sidebar, editor, 120);
    splitter->SetSashGravity(0.0);
    splitter->SetMinimumPaneSize(100);

    editor->SetFocus();
}
