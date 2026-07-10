#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vector>
#include <string>

class TextBuffer
{
public:
    TextBuffer();
    TextBuffer(const TextBuffer &);
    TextBuffer(std::wstring source);

    void InsertChar(int line, int column, wchar_t ch);
    void InsertString(int line, int column, std::wstring newString, int *lastLine = nullptr, int *lastColumn = nullptr);
    void SetLine(int line, std::wstring newString);
    void RemoveChar(int line, int column);
    void RemoveChars(int line, int column, int count);
    void InsertLine(int index);
    void RemoveLine(int index);
    void RemoveRange(int startLine, int startColumn, int endLine, int endColumn);
    void MergeNext(int index);
    std::wstring GetLine(int index);
    std::wstring GetRange(int startLine, int startColumn, int endLine, int endColumn);
    wchar_t GetChar(int index, int column);
    size_t GetLineCount();
    size_t GetLineLength(int index);

private:
    std::vector<std::wstring> lines;
};

#endif
