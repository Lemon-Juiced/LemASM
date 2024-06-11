#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <map>
#include <random>
#include <stack>
#include <string>
#include <vector>
#include "ErrorHandler.h"
#include "Line.h"

using namespace std;

// Globals
bool debugMode = false; // Is debug mode enabled, default is false.
bool outputToFile = false; // Should the output be written to a file, default is false.
string outputFileName = ""; // The name of the output file, default is "".
int dataSectionLine = 0; // The line number where the data section starts. Default is 0. If there is no data section, this will remain 0.
int codeSectionLine = 0; // The line number where the code section starts. Default is 0. If there is no code section, this will remain 0. This should be an error state.
map<string, int> jumpMap; // A map to store integer values.
map<string, string> stringMap; // A map to store string values.
stack<int> lStack; // A stack to store integer values.
vector<Line> lines; // A vector to store the lines of the file.
ErrorHandler errorHandler; // An instance of the error handler.

/**
 * Interprets the data section of LemASM code.
 * The data section of the code just contains strings that can be used in the code section, that will be stored in the stringMap.
 * This function loops through the data section and stores the strings in the stringMap.
 * 
 * Here are the relevant mnemonics and symbols that LemASM supports:
 * //  > This symbol is used to comment out a line.
 * STR > This mnemonic is used to define a string in the data section. Example: "STR hello_world = \"Hello, World!\"".
 * 
 * These mnemonics must be at the beginning of the line or else the program will error.
 * Since whitespace lines are also legal, we can also skip those.
 * 
 * If the -d flag is set, the function will:
 * 1. Print the line number and the line.
 * 2. Print the stringMap.
 * 
 * @return 0 if the data section was interpreted successfully, 1 otherwise.
*/
int dataSection() {
    for (int i = dataSectionLine; i < codeSectionLine - 1; i++) {
        Line line = lines[i];
        string contents = line.getContents();
        if (contents.find("//") == 0) continue;
        else if (contents.empty() || all_of(contents.begin(),contents.end(),[](unsigned char c){return isspace(c);})) continue;
        else if (contents.find("STR") == 0) {
            string str = contents.substr(4);
            string key = str.substr(0, str.find(" "));
            string value = str.substr(str.find("\"") + 1);
            stringMap[key] = value;
        } else {
            errorHandler.handleErrorWithLine("Invalid data section line.", line.getLineNumber(), contents);
            return 1;
        }

        if (debugMode) {
            cout << endl << "Data Section:" << endl;
            cout << line.getLineNumber() << ": " << contents << endl << endl;
            cout << "String Map:" << endl;
            for (auto const& x : stringMap) {
                cout << x.first << ": " << x.second << endl;
            }
        }
    }

    return 0; // The data was interpreted successfully.
}

/**
 * Interprets the code section of LemASM code.
 * The code section of the code contains the assembly code that will be interpreted and executed.
 * This function loops through the code section and interprets the code line by line.
 * 
 * Here are the relevant mnemonics and symbols that LemASM supports:
 * //  > This symbol is used to comment out a line.
 * .   > This symbol is used to define a label. Example: ".label_name".
 * ADD > This mnemonic adds the top two values of the stack. Example: "ADD".
 * CPR > This mnemonic is used to print a string to the console. Example: "CPR hello_world".
 * DIV > This mnemonic divides the top two values of the stack. Example: "DIV".
 * DUP > This mnemonic duplicates the top value of the stack. Example: "DUP".
 * JEQ > This mnemonic jumps to a label if the top two values of the stack are equal. Example: "JEQ label_name".
 * JGT > This mnemonic jumps to a label if the top value of the stack is greater than the second value. Example: "JGT label_name".
 * JLT > This mnemonic jumps to a label if the top value of the stack is less than the second value. Example: "JLT label_name".
 * JMP > This mnemonic jumps to a label. Example: "JMP label_name".
 * JNE > This mnemonic jumps to a label if the top two values of the stack are not equal. Example: "JNE label_name".
 * MOD > This mnemonic takes the modulus of the top two values of the stack. Example: "MOD".
 * MUL > This mnemonic multiplies the top two values of the stack. Example: "MUL".
 * PSH > This mnemonic is used to push a value onto the stack. Example: "PSH 5".
 * POP > This mnemonic is used to pop a value off the stack. Example: "POP".
 * RAN > This mnemonic is used to generate a random number and push it onto the stack. Example: "RAN".
 * RET > This mnemonic is used to return from the program. It returns the top value of the stack. If the stack is empty, it returns 0.
 * ROR > This mnemonic randomizes the order of the stack. Example: "ROR".
 * SUB > This mnemonic subtracts the top two values of the stack. Example: "SUB".
 * SWP > This mnemonic swaps the top two values of the stack. Example: "SWP".
 * 
 * These mnemonics must be at the beginning of the line or else the program will error.
 * Since whitespace lines are also legal, we can also skip those.
 * 
 * If the -d flag is set, the function will:
 * 1. Print the line number and the line.
 * 2. Print the jumpMap.
 * 
 * @return 0 if the code section was interpreted successfully, 1 otherwise unless RET is called, in which case it will return what RET returns.
*/
int codeSection() {
    for (int i = codeSectionLine; i < lines.size(); i++) {
        Line line = lines[i];
        string contents = line.getContents();
        if (contents.find("//") == 0) continue;
        else if (contents.empty() || all_of(contents.begin(),contents.end(),[](unsigned char c){return isspace(c);})) continue;

        // If the line contains a label, add it to the jumpMap.
        else if (contents.find(".") == 0) {
            string label = contents.substr(1);
            jumpMap[label] = i;
        } 

        // Mnemonics
        // Add (ADD)
        else if (contents.find("ADD") == 0) {
            if (lStack.size() < 2) {
                errorHandler.handleErrorWithLine("Stack does not have enough values to add.", line.getLineNumber(), contents);
                return 1;
            } else {
                int a = lStack.top();
                lStack.pop();
                int b = lStack.top();
                lStack.pop();
                lStack.push(a + b);
            }
        }

        // Console Print (CPR)
        else if (contents.find("CPR") == 0) {
            string key = contents.substr(4);
            if (stringMap.find(key) != stringMap.end()) {
                cout << stringMap[key] << endl;
            } else {
                errorHandler.handleErrorWithLine("String not found in string map.", line.getLineNumber(), contents);
                return 1;
            }
        }

        // Divide (DIV)
        else if (contents.find("DIV") == 0) {
            if (lStack.size() < 2) {
                errorHandler.handleErrorWithLine("Stack does not have enough values to divide.", line.getLineNumber(), contents);
                return 1;
            } else {
                int a = lStack.top();
                lStack.pop();
                int b = lStack.top();
                lStack.pop();
                lStack.push(b / a);
            }
        }

        // Duplicate (DUP)
        else if (contents.find("DUP") == 0) {
            if (lStack.empty()) {
                errorHandler.handleErrorWithLine("Stack is empty.", line.getLineNumber(), contents);
                return 1;
            } else {
                lStack.push(lStack.top());
            }
        }

        // Jump Equal (JEQ)
        else if (contents.find("JEQ") == 0) {
            if (lStack.size() < 2) {
                errorHandler.handleErrorWithLine("Stack does not have enough values to jump.", line.getLineNumber(), contents);
                return 1;
            } else {
                int a = lStack.top();
                lStack.pop();
                int b = lStack.top();
                lStack.pop();
                if (a == b) {
                    string label = contents.substr(4);
                    i = jumpMap[label];
                }
            }
        }

        // Jump Greater Than (JGT)
        else if (contents.find("JGT") == 0) {
            if (lStack.size() < 2) {
                errorHandler.handleErrorWithLine("Stack does not have enough values to jump.", line.getLineNumber(), contents);
                return 1;
            } else {
                int a = lStack.top();
                lStack.pop();
                int b = lStack.top();
                lStack.pop();
                if (b > a) {
                    string label = contents.substr(4);
                    i = jumpMap[label];
                }
            }
        }

        // Jump Less Than (JLT)
        else if (contents.find("JLT") == 0) {
            if (lStack.size() < 2) {
                errorHandler.handleErrorWithLine("Stack does not have enough values to jump.", line.getLineNumber(), contents);
                return 1;
            } else {
                int a = lStack.top();
                lStack.pop();
                int b = lStack.top();
                lStack.pop();
                if (b < a) {
                    string label = contents.substr(4);
                    i = jumpMap[label];
                }
            }
        }

        // Jump (JMP)
        else if (contents.find("JMP") == 0) {
            string label = contents.substr(4);
            i = jumpMap[label];
        }

        // Jump Not Equal (JNE)
        else if (contents.find("JNE") == 0) {
            if (lStack.size() < 2) {
                errorHandler.handleErrorWithLine("Stack does not have enough values to jump.", line.getLineNumber(), contents);
                return 1;
            } else {
                int a = lStack.top();
                lStack.pop();
                int b = lStack.top();
                lStack.pop();
                if (a != b) {
                    string label = contents.substr(4);
                    i = jumpMap[label];
                }
            }
        }

        // Modulus (MOD)
        else if (contents.find("MOD") == 0) {
            if (lStack.size() < 2) {
                errorHandler.handleErrorWithLine("Stack does not have enough values to take the modulus.", line.getLineNumber(), contents);
                return 1;
            } else {
                int a = lStack.top();
                lStack.pop();
                int b = lStack.top();
                lStack.pop();
                lStack.push(b % a);
            }
        }

        // Multiply (MUL)
        else if (contents.find("MUL") == 0) {
            if (lStack.size() < 2) {
                errorHandler.handleErrorWithLine("Stack does not have enough values to multiply.", line.getLineNumber(), contents);
                return 1;
            } else {
                int a = lStack.top();
                lStack.pop();
                int b = lStack.top();
                lStack.pop();
                lStack.push(a * b);
            }
        }

        // Push (PSH)
        else if (contents.find("PSH") == 0) {
            int value = stoi(contents.substr(4));
            lStack.push(value);
        }

        // Pop (POP)
        else if (contents.find("POP") == 0) {
            if (lStack.empty()) {
                errorHandler.handleErrorWithLine("Stack is empty.", line.getLineNumber(), contents);
                return 1;
            } else {
                lStack.pop();
            }
        }

        // Random (RAN)
        else if (contents.find("RAN") == 0) {
            lStack.push(rand());
        }

        // Return (RET)
        else if (contents.find("RET") == 0) {
            if (lStack.empty()) {
                return 0;
            } else {
                return lStack.top();
            }
        }

        // Randomize Order (ROR)
        else if (contents.find("ROR") == 0) {
            vector<int> temp;
            while (!lStack.empty()) {
                temp.push_back(lStack.top());
                lStack.pop();
            }

            // Create a random number generator and shuffle the vector.
            random_device rd;
            default_random_engine engine(rd());
            shuffle(temp.begin(), temp.end(), engine);

            for (int i = 0; i < temp.size(); i++) {
                lStack.push(temp[i]);
            }
        }

        // Subtract (SUB)
        else if (contents.find("SUB") == 0) {
            if (lStack.size() < 2) {
                errorHandler.handleErrorWithLine("Stack does not have enough values to subtract.", line.getLineNumber(), contents);
                return 1;
            } else {
                int a = lStack.top();
                lStack.pop();
                int b = lStack.top();
                lStack.pop();
                lStack.push(b - a);
            }
        }

        // Swap (SWP)
        else if (contents.find("SWP") == 0) {
            if (lStack.size() < 2) {
                errorHandler.handleErrorWithLine("Stack does not have enough values to swap.", line.getLineNumber(), contents);
                return 1;
            } else {
                int a = lStack.top();
                lStack.pop();
                int b = lStack.top();
                lStack.pop();
                lStack.push(a);
                lStack.push(b);
            }
        }

        // After all the mnemonics are checked, check for errors.
        else {
            errorHandler.handleErrorWithLine("Invalid code section line.", line.getLineNumber(), contents);
            return 1;
        }

        if (debugMode) {
            cout << endl << "Code Section:" << endl;
            cout << line.getLineNumber() << ": " << contents << endl << endl;
            cout << "Jump Map:" << endl;
            for (auto const& x : jumpMap) {
                cout << x.first << ": " << x.second << endl;
            }
        }

    }

    return 0; // The code was interpreted successfully.
}

/**
 * Interprets the given LemASM file.
 * 
 * This function reads the file line by line and interprets each line.
 * The file is split into two sections: the data section and the code section.
 * The data section (the top half of the file) defines strings that can be used in output.
 * - The strings in this section are stored in a map.
 * The code section (the bottom half of the file) contains the actual assembly code.
 * - The code in this section is interpreted and executed line by line.
 * 
 * The main data structure of the LemASM interpreter is a stack, which is used to store integer values.
 * There are two supplementary maps, one that stores string values and another that stores integer values.
 * There is also a vector that stores the lines of the file.
 * 
 * To accomplish this, the interpreter splits the will read each section of the file seperately.
 * 
 * Here are the relevant mnemonics and symbols that LemASM supports:
 * - #   > This symbol is used to define a section of the file. Uses include: "#DATA" and "#CODE".
 * 
 * @param fileName The name of the file to interpret.
 * @return 0 if the file was interpreted successfully, 1 otherwise.
 */
int interpretFile(string fileName) {
    // Open the file
    ifstream file(fileName);
    if (!file.is_open()) {
        errorHandler.handleErrorNoLine("Could not open file: " + fileName);
        return 1;
    }

    // Read the file line by line
    string line; // The current line being read
    int lineNumber = 0; // The current line number

    /* First we need to determine if the line is a comment or empty.
     * If it is a comment or empty, we can skip it.
     *  If it is not a comment or empty, we can check if it is a section.
     * If it is a section, we can determine if it is the data section or the code section.
     *  If it is the data section, we can add the string to the string map.
     *      If the program has a data section, it must come before the code section.
     *  If it is the code section, we can interpret the line.
     *      Once the program enters the code section, it will run until it reaches the return statement.
     * 
     * For now, we will just read the file and place the lines into a vector.
     * Additionally, we can store the line numbers where the sections start.
     * If the -d flag is set, we will print the line number and the line as well.
     */
    while (getline(file, line)) {
        lineNumber++;
        lines.push_back(Line(lineNumber, line));
        if(line == "#DATA") dataSectionLine = lineNumber;
        else if(line == "#CODE") codeSectionLine = lineNumber;
        
        if (debugMode) cout << lineNumber << ": " << line << endl;
    }

    // Close the file as it is no longer needed
    file.close();

    // Interpret the data section
    int dataSectionSuccessful = dataSection();

    // Interpret the code section
    int codeSectionSuccessful;
    if (dataSectionSuccessful == 0) codeSectionSuccessful = codeSection();
    return codeSectionSuccessful;
}

/**
 * Main function for the LemASM (Lemon Assembly) interpreter.
 * 
 * This function does a few things:
 * 1. Check if the user provided an input file.
 *    - If not, print error and usage information and return 1.
 * 2. Check that the input file is of the correct format. 
 *    - Acceptable formats are: lasm and lemasm
 *    - If not, print an error message and return 1.
 * 3. Check if any additional command-line arguments were provided.
 *    - Acceptable arguments are:
 *    - Debug                                 > -d 
 *    - Output To File                        > -o <output_file>
 * 4. Pass the input file to the LemASM interpreter.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments. 
*/
int main(int argc, char* argv[]) {
    string usageString = "Usage: LemASM <input_file> [-d] [-p] [-o <output_file>]";

    // 1. Check if the user provided an input file.
    if (argc < 2) {
        errorHandler.handleErrorNoLine("No input file provided.\n" + usageString);
        return 1;
    }

    // 2. Check that the input file is of the correct format.
    string inputFileName = argv[1];
    string fileExtension = inputFileName.substr(inputFileName.find_last_of(".") + 1);
    if (fileExtension != "lasm" && fileExtension != "lemasm") {
        errorHandler.handleErrorNoLine("Invalid file format.\nAcceptable formats are: \".lasm\" or \".lemasm\".");
        return 1;
    }

    // 3. Check if any additional command-line arguments were provided.
    for (int i = 2; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-d") debugMode = true;
        else if (arg == "-o") {
            if (i + 1 < argc) {
                outputToFile = true;
                outputFileName = argv[i + 1];
                i++;
            } else {
                errorHandler.handleErrorNoLine("No output file provided.\n" + usageString);
                return 1;
            }
        } else {
            errorHandler.handleErrorNoLine("Invalid argument: " + arg + "\n" + usageString);
            return 1;
        }
    }

    // 4. Pass the input file to the LemASM interpreter.
    return interpretFile(inputFileName);
}