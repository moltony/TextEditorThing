#include "buffer.hpp"
#include <sstream>
#include <stdio.h>

void SplitString(std::wstring string, std::vector<std::wstring> &lines)
{
    std::wistringstream stream(string);
    std::wstring line;

    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
}

TextBuffer::TextBuffer()
{
}

TextBuffer::TextBuffer(const TextBuffer &other)
{
    lines = other.lines;
}

TextBuffer::TextBuffer(std::wstring source)
{
    SplitString(source, lines);
}

void TextBuffer::InsertChar(int line, int column, wchar_t ch)
{
    if (line >= lines.size())
        return;

    std::wstring &string = lines[line];
    if (column > string.length())
        return;

    string.insert(column, 1, ch);
}

void TextBuffer::InsertString(int line, int column, std::wstring newString, int *lastLine, int *lastColumn)
{
    if (line >= lines.size())
        return;

    std::wstring &string = lines[line];
    if (column > string.length())
        return;

    if (newString.find('\n') != std::wstring::npos) {
        std::wstring lineEnd = string.substr(column);
        RemoveChars(line, column, lineEnd.size());

        std::vector<std::wstring> insertLines;
        SplitString(newString, insertLines);
        string.insert(column, insertLines[0]);
        int i;
        for (i = 1; i < insertLines.size(); i++) {
            InsertLine(i + line);
            SetLine(i + line, insertLines[i]);
        }
        i--;
        if (lastColumn) {
            *lastColumn = GetLineLength(i + line);
        }

        InsertString(i + line, GetLineLength(i + line), lineEnd);

        if (lastLine) {
            *lastLine = i + line;
        }
    } else {
        string.insert(column, newString);
        if (lastLine) {
            *lastLine = line;
        }
        if (lastColumn) {
            *lastColumn = column + newString.length();
        }
    }
}

void TextBuffer::SetLine(int line, std::wstring newString)
{
    if (line >= lines.size())
        return;

    lines[line] = newString;
}

void TextBuffer::RemoveChar(int line, int column)
{
    RemoveChars(line, column, 1);
}

void TextBuffer::RemoveChars(int line, int column, int count)
{
    if (line >= lines.size())
        return;

    std::wstring &string = lines[line];
    if (column >= string.length())
        return;

    string.erase(column, count);
}

void TextBuffer::InsertLine(int index)
{
    lines.insert(lines.begin() + index, L"");
}

void TextBuffer::RemoveLine(int index)
{
    lines.erase(lines.begin() + index);
}

void TextBuffer::RemoveRange(int startLine, int startColumn, int endLine, int endColumn)
{
    if (startLine == endLine) {
        RemoveChars(startLine, startColumn, endColumn - startColumn);
    } else {
        RemoveChars(startLine, startColumn, GetLineLength(startLine) - startColumn);
        RemoveChars(endLine, 0, endColumn);

        // delete lines in the middle if any
        for (int i = 0; i < endLine - startLine - 1; i++) {
            RemoveLine(startLine + 1);
        }

        MergeNext(startLine);
    }
}

void TextBuffer::MergeNext(int index)
{
    if (index >= lines.size() - 1)
        return;

    InsertString(index, GetLineLength(index), GetLine(index + 1));
    RemoveLine(index + 1);
}

std::wstring TextBuffer::GetLine(int index)
{
    if (index >= lines.size()) {
        return L"";
    }
    return lines[index];
}

std::wstring TextBuffer::GetRange(int startLine, int startColumn, int endLine, int endColumn)
{
    std::wstring text;
    
    if (startLine == endLine) {
        text = GetLine(startLine).substr(startColumn, endColumn);
        return text;
    }

    text += GetLine(startLine).substr(startColumn);
    for (int i = 1; i <= endLine - startLine - 1; i++) {
        text += L"\n" + GetLine(i + startLine);
    }
    text += L"\n" + GetLine(endLine).substr(0, endColumn);
    return text;
}

wchar_t TextBuffer::GetChar(int index, int column)
{
    std::wstring line = GetLine(index);
    if (column >= line.length()) {
        return 0;
    }
    return line[column];
}

size_t TextBuffer::GetLineCount()
{
    return lines.size();
}

size_t TextBuffer::GetLineLength(int index)
{
    return lines[index].length();
}
