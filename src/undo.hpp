#ifndef UNDO_HPP
#define UNDO_HPP

#include <vector>
#include "buffer.hpp"
#include "caret.hpp"

struct UndoState
{
    TextBuffer buffer;
    CaretPosition caretPos;

    UndoState();
    UndoState(TextBuffer &buffer, CaretPosition &caretPos);
};

class UndoManager
{
public:
    UndoManager();

    void Record(TextBuffer &buffer, CaretPosition &caretPos);
    void Undo(TextBuffer &buffer, CaretPosition &caretPos);
    void Redo(TextBuffer &buffer, CaretPosition &caretPos);

private:
    std::vector<UndoState> states;
    int undoPosition;

    void RestoreState(TextBuffer &buffer, CaretPosition &caretPos);
};

#endif
