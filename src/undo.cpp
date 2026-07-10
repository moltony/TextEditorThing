#include "undo.hpp"
#include <assert.h>

UndoState::UndoState()
{
}

UndoState::UndoState(TextBuffer &buffer, CaretPosition &caretPos)
{
    this->buffer = buffer;
    this->caretPos = caretPos;
}

UndoManager::UndoManager() : undoPosition(0)
{
}

void UndoManager::Record(TextBuffer &buffer, CaretPosition &caretPos)
{
    states.resize(undoPosition + 1);
    states.emplace_back(buffer, caretPos);
    undoPosition = states.size() - 1;
}

void UndoManager::Undo(TextBuffer &buffer, CaretPosition &caretPos)
{
    if (undoPosition == 0) {
        return;
    }

    undoPosition--;
    RestoreState(buffer, caretPos);
}

void UndoManager::Redo(TextBuffer &buffer, CaretPosition &caretPos)
{
    if (undoPosition + 1 >= states.size()) {
        return;
    }

    undoPosition++;
    RestoreState(buffer, caretPos);
}

void UndoManager::RestoreState(TextBuffer &buffer, CaretPosition &caretPos)
{
    buffer = states[undoPosition].buffer;
    caretPos = states[undoPosition].caretPos;
}
