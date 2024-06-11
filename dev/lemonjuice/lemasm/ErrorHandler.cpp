#include "ErrorHandler.h"
#include <iostream>
#include <string>

using namespace std;

// Constructor 
ErrorHandler::ErrorHandler() {}

// Destructor
ErrorHandler::~ErrorHandler() {}

/**
 * Applies a style to the error message, then returns the styled text.
 * The style is applied using ANSI escape codes.
 * The text is styled to be red, and optionally bold.
 * 
 * @param text The text to apply the style to.
 * @param bold Whether the text should be bold or not.
 * @return The styled text.
 */
string ErrorHandler::applyErrorStyle(string text, bool bold){
    if (bold) {
        return "\033[1;31m" + text + "\033[0m";
    } else {
        return "\033[31m" + text + "\033[0m";
    }
}

/**
 * Handles an error with no line number associated.
 * Typically, this is used for errors with the file.
 * 
 * @param errorMessage The error message to display.
 */
void ErrorHandler::handleErrorNoLine(string errorMessage) {
    cerr << applyErrorStyle("Error: ", true) << applyErrorStyle(errorMessage, false) << endl;
}

/**
 * Handles an error with a line number associated.
 * Typically, this is used for errors within the file.
 * 
 * @param errorMessage The error message to display.
 * @param lineNumber The line number the error occurred on.
 * @param lineContents The contents of the line the error occurred on.
 */
void ErrorHandler::handleErrorWithLine(string errorMessage, int lineNumber, string lineContents) {
    string lineNumberString = "At line: " + to_string(lineNumber);
    cerr << applyErrorStyle("Error: ", true) << applyErrorStyle(errorMessage, false) << endl 
         << applyErrorStyle(lineNumberString, false) << endl
         << applyErrorStyle(lineContents, false) << endl;
}