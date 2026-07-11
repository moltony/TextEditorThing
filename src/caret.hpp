#ifndef CARET_HPP
#define CARET_HPP

struct CaretPosition
{
    int column;
    int line;
    int goalColumn;

    CaretPosition();
    bool operator==(const CaretPosition &) const;
    bool operator!=(const CaretPosition &) const;
    bool operator<(const CaretPosition &) const;
    CaretPosition &operator=(const CaretPosition &);
};

struct CaretPositionRange
{
    CaretPosition start;
    CaretPosition end;

    CaretPositionRange();
    CaretPositionRange(CaretPosition start, CaretPosition end);
    CaretPositionRange(CaretPosition caretPos);
};

#endif
