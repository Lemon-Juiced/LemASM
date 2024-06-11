#include "Line.h"
#include <string>

using namespace std;

// Constructor
Line::Line(int lineNumber, string contents): lineNumber(lineNumber), contents(contents) {}

/**
 * Gets the line number of the line.
 * 
 * @return The line number.
 */
int Line::getLineNumber() {
    return lineNumber;
}

/**
 * Gets the contents of the line.
 * 
 * @return The contents of the line.
 */
string Line::getContents() {
    return contents;
}

/**
 * Gets the line as a string.
 * 
 * @return The line as a string.
 */
string Line::getLineAsString() {
    return to_string(lineNumber) + ": " + contents;
}