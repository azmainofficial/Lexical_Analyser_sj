#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Check if the given word is a C++ keyword
bool isKeyword(const string& word) {
    static const vector<string> keywords = {
        "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "break",
        "case", "catch", "char", "char8_t", "char16_t", "char32_t", "class", "compl", "concept", "const",
        "consteval", "constexpr", "constinit", "const_cast", "continue", "co_await", "co_return", "co_yield",
        "decltype", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit", "export",
        "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int", "long", "mutable",
        "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private",
        "protected", "public", "register", "reinterpret_cast", "requires", "return", "short", "signed", "sizeof",
        "static", "static_assert", "static_cast", "struct", "switch", "template", "this", "thread_local", "throw",
        "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void",
        "volatile", "wchar_t", "while", "xor", "xor_eq"
    };
    return find(keywords.begin(), keywords.end(), word) != keywords.end();
}

// Check if the given word is a valid identifier
bool isIdentifier(const string& word) {
    return regex_match(word, regex("[a-zA-Z_][a-zA-Z0-9_]*"));
}

// // Check if the given word is a constant (integer or floating-point)
// bool isConstant(const string& word) {
//     return regex_match(word, regex("[0-9]+(\\.[0-9]+)?"));
// }
bool isConstant(const string& word) {
    bool decimalPoint = false; // Flag to indicate if a decimal point has been encountered
    bool digitFound = false;   // Flag to indicate if a digit has been found
    
    for (size_t i = 0; i < word.size(); ++i) { // Iterate through each character in the string
        if (isdigit(word[i])) {                // Check if the current character is a digit
            digitFound = true;                 // Set digitFound to true if a digit is found
        } else if (word[i] == '.' && !decimalPoint) { // Check if the current character is a decimal point and if it's the first one encountered
            decimalPoint = true;               // Set decimalPoint to true if a decimal point is found
        } else {                               // If the current character is neither a digit nor a valid decimal point
            return false;                      // Return false as the string is not a constant
        }
    }
    return digitFound;                         // Return true if at least one digit was found, indicating a valid constant
}

// Check if the given word is an operator
bool isOperator(const string& word) {
    static const vector<string> operators = {
        "+", "-", "*", "/", "=", "<<", ">>", ">", "<", ">=", "<=", "==", "!="
    };
    return find(operators.begin(), operators.end(), word) != operators.end();
}

// Check if the given word is a punctuation character
bool isPunctuation(const string& word) {
    static const vector<string> punctuations = { ";", ",", ".", "!", "?" };
    return find(punctuations.begin(), punctuations.end(), word) != punctuations.end();
}

// Check if the given word is a parenthesis character
bool isParenthesis(const string& word) {
    static const vector<string> parenthesis = { "(", ")", "{", "}", "[", "]" };
    return find(parenthesis.begin(), parenthesis.end(), word) != parenthesis.end();
}

// Check if the given word is a string literal
bool isStringLiteral(const string& word) {
    return regex_match(word, regex("\".*\""));
}

// Check if the given word is a preprocessor directive
bool isPreprocessor(const string& word) {
    return !word.empty() && word[0] == '#';
}

// Handle unknown tokens by breaking them into identifiable components
void handleUnknownToken(const string& token, vector<string>& identifiers, vector<string>& operators, vector<string>& constants, vector<string>& punctuations, vector<string>& parenthesis) {
    string current;
    for (char c : token) {
        if (isalnum(c) || c == '_') {
            current += c;
        } else {
            if (!current.empty()) {
                if (isIdentifier(current)) {
                    identifiers.push_back(current);
                } else if (isConstant(current)) {
                    constants.push_back(current);
                }
                current.clear();
            }
            string singleChar(1, c);
            if (isOperator(singleChar)) {
                operators.push_back(singleChar);
            } else if (isPunctuation(singleChar)) {
                punctuations.push_back(singleChar);
            } else if (isParenthesis(singleChar)) {
                parenthesis.push_back(singleChar);
            }
        }
    }
    if (!current.empty()) {
        if (isIdentifier(current)) {
            identifiers.push_back(current);
        } else if (isConstant(current)) {
            constants.push_back(current);
        }
    }
}

// Check if a line contains a comment and ignore it
bool containsComment(const string& line) {
    static const regex singleLineCommentRegex("//.*");
    static const regex multiLineCommentStartRegex("/\\*.*");
    static const regex multiLineCommentEndRegex(".*\\*/");

    return regex_search(line, singleLineCommentRegex) || regex_search(line, multiLineCommentStartRegex) || regex_search(line, multiLineCommentEndRegex);
}

int main() {
    ifstream file("input.txt");
    if (!file) {
        cerr << "Error: Could not open the file." << endl;
        return 1;
    }

    string line;
    vector<string> keywords, identifiers, constants, operators, punctuations, parenthesis, stringLiterals, preProcessors;
    bool inMultiLineComment = false;

    while (getline(file, line)) {
        if (containsComment(line)) {
            continue;
        }
        if (regex_search(line, regex("/\\*"))) {
            inMultiLineComment = true;
            continue;
        }
        if (regex_search(line, regex("\\*/"))) {
            inMultiLineComment = false;
            continue;
        }
        if (inMultiLineComment) {
            continue;
        }

        stringstream ss(line);
        string token;
        while (ss >> token) {
            if (isPreprocessor(token)) {
                preProcessors.push_back(token);
            } else if (isKeyword(token)) {
                keywords.push_back(token);
            } else if (isStringLiteral(token)) {
                stringLiterals.push_back(token);
            } else if (isIdentifier(token)) {
                identifiers.push_back(token);
            } else if (isConstant(token)) {
                constants.push_back(token);
            } else if (isOperator(token)) {
                operators.push_back(token);
            } else if (isPunctuation(token)) {
                punctuations.push_back(token);
            } else if (isParenthesis(token)) {
                parenthesis.push_back(token);
            } else {
                handleUnknownToken(token, identifiers, operators, constants, punctuations, parenthesis);
            }
        }
    }

    file.close();

    // Print the results
    auto printTokens = [](const string& label, const vector<string>& tokens) {
        cout << label << ": (" << tokens.size() << ") -> ";
        for (const string& token : tokens) cout << token << ", ";
        cout << endl;
    };

    printTokens("Keywords", keywords);
    printTokens("Identifiers", identifiers);
    printTokens("Constants", constants);
    printTokens("Operators", operators);
    printTokens("Punctuations", punctuations);
    printTokens("Parenthesis", parenthesis);
    printTokens("String Literals", stringLiterals);
    printTokens("Pre Processors", preProcessors);

    return 0;
}
