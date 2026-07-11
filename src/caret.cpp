#include "caret.hpp"

CaretPosition::CaretPosition()
{
    column = 0;
    line = 0;
    goalColumn = 0;
}

bool CaretPosition::operator==(const CaretPosition &other) const
{
    return column == other.column && line == other.line;
}

bool CaretPosition::operator!=(const CaretPosition &other) const
{
    return !(*this == other);
}

bool CaretPosition::operator<(const CaretPosition &other) const
{
    if (line == other.line) {
        return column < other.column;
    }
    return line < other.line;
}

CaretPosition &CaretPosition::operator=(const CaretPosition &other)
{
    column = other.column;
    line = other.line;
    goalColumn = other.goalColumn;
    return *this;
}

CaretPositionRange::CaretPositionRange()
{
}

CaretPositionRange::CaretPositionRange(CaretPosition start, CaretPosition end)
{
    this->start = start;
    this->end = end;
}

CaretPositionRange::CaretPositionRange(CaretPosition caretPos)
{
    start = caretPos;
    end = caretPos;
}
