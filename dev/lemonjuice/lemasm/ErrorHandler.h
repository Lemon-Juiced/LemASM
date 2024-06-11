#pragma once
#include <string>

using namespace std;

class ErrorHandler {
public:
    ErrorHandler();
    ~ErrorHandler();

    // Error Styling
    string applyErrorStyle(string text, bool bold);

    // Error Handling
    void handleErrorNoLine(string errorMessage);
    void handleErrorWithLine(string errorMessage, int lineNumber, string lineContents);
};
