#pragma once
#include <string>

using namespace std;

class Line {
private:
    int lineNumber;
    string contents;

public:
    Line(int lineNumber, string contents);
    int getLineNumber();
    string getContents();
    string getLineAsString();
};
