#include "app.hpp"
#include "frame.hpp"

bool App::OnInit()
{
    AppFrame *frame = new AppFrame();
    frame->Show();
    return true;
}

wxIMPLEMENT_APP(App);
