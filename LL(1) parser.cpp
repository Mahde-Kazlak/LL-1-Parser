#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class LL1Parser {
private:
    vector<string> tokens;
    size_t currentTokenIndex;
    ofstream derivationFile;
    int lineNumber;
    string currentDerivation;
    bool hasError;

    vector<string> split(const string& s) {
        vector<string> symbols;
        size_t start = 0;
        size_t end = s.find(' ');
        while (end != string::npos) {
            if (end != start) {
                symbols.push_back(s.substr(start, end - start));
            }
            start = end + 1;
            end = s.find(' ', start);
        }
        if (start < s.length()) {
            symbols.push_back(s.substr(start));
        }
        return symbols;
    }

    string join(const vector<string>& symbols) {
        string result;
        for (size_t i = 0; i < symbols.size(); ++i) {
            if (i > 0) result += " ";
            result += symbols[i];
        }
        return result;
    }

    void replaceNonTerminal(const string& nonTerminal, const vector<string>& production) {
        if (hasError) return;

        vector<string> symbols = split(currentDerivation);
        bool replaced = false;

        for (auto it = symbols.begin(); it != symbols.end(); ++it) {
            if (*it == nonTerminal) {
                it = symbols.erase(it);
                symbols.insert(it, production.begin(), production.end());
                replaced = true;
                break;
            }
        }

        if (!replaced) {
            derivationFile << "Error: Replacement failed for " << nonTerminal << endl;
            hasError = true;
            return;
        }

        currentDerivation = join(symbols);
        derivationFile << "-> " << currentDerivation << endl;
    }

    string currentToken() {
        return currentTokenIndex < tokens.size() ? tokens[currentTokenIndex] : "";
    }

    bool match(const string& expected) {
        if (hasError) return false;

        if (currentToken() == expected) {
            currentTokenIndex++;
            lineNumber++;
            return true;
        }

        derivationFile << "Error at line " << lineNumber << ": Expected '"
            << expected << "', found '" << currentToken() << "'" << endl;
        hasError = true;
        return false;
    }

public:
    LL1Parser(const vector<string>& tokens)
        : tokens(tokens), currentTokenIndex(0), lineNumber(1), hasError(false) {
        derivationFile.open("derivation.txt");
        if (!derivationFile.is_open()) {
            cout << "Error: Could not open derivation file" << endl;
            hasError = true;
            return;
        }
        derivationFile << "Left most derivation:" << endl;
        derivationFile << "program -> begin statements end." << endl;
        currentDerivation = "begin statements end.";

    }

    ~LL1Parser() {
        if (derivationFile.is_open()) {
            derivationFile.close();
        }
    }

    bool parse() {
        if (hasError) return false;

        return match("begin") && statements() && match("end.");
    }

private:
    bool statements() {
        replaceNonTerminal("statements", { "statement", ",", "moreStmts" });
        return statement() && match(",") && moreStmts();
    }

    bool moreStmts() {
        if (hasError) return false;

        if (currentToken() == "end.") {
            replaceNonTerminal("moreStmts", {});
            return true;
        }
        replaceNonTerminal("moreStmts", { "statements" });
        return statements();
    }

    bool statement() {
        if (hasError) return false;

        string token = currentToken();
        if (token == "if") return conditional();
        if (token == "while") return loop();
        if (token == "get" || token == "put") return io();
        if (token == "id") return assignment();
        if (token == "nothing") return match("nothing");

        derivationFile << "Error at line " << lineNumber << ": Unexpected statement" << endl;
        hasError = true;
        return false;
    }

    bool conditional() {
        replaceNonTerminal("statement", { "conditional" });
        replaceNonTerminal("conditional", { "if", "(", "condition", ")", "statement", "else", "statement" });
        return match("if") && match("(") && condition() && match(")") &&
            statement() && match("else") && statement();
    }

    bool loop() {
        replaceNonTerminal("statement", { "loop" });
        replaceNonTerminal("loop", { "while", "(", "condition", ")", "perform", "statement" });
        return match("while") && match("(") && condition() && match(")") &&
            match("perform") && statement();
    }

    bool io() {
        replaceNonTerminal("statement", { "io" });
        string token = currentToken();
        if (token == "get") return input();
        if (token == "put") return output();

        derivationFile << "Error at line " << lineNumber << ": Expected 'get' or 'put'" << endl;
        hasError = true;
        return false;
    }

    bool input() {
        replaceNonTerminal("io", { "input" });
        replaceNonTerminal("input", { "get", "(", "id", ")" });
        return match("get") && match("(") && match("id") && match(")");
    }

    bool output() {
        replaceNonTerminal("io", { "output" });
        replaceNonTerminal("output", { "put", "(", "id", ")" });
        return match("put") && match("(") && match("id") && match(")");
    }

    bool assignment() {
        replaceNonTerminal("statement", { "assignment" });
        replaceNonTerminal("assignment", { "id", "=", "expr" });
        return match("id") && match("=") && expr();
    }

    bool expr() {
        if (hasError) return false;

        if (currentToken() == "(") {
            replaceNonTerminal("expr", { "(", "expr", ")" });
            return match("(") && expr() && match(")");
        }
        replaceNonTerminal("expr", { "terms" });
        return terms();
    }

    bool terms() {
        replaceNonTerminal("terms", { "term", "moreTerms" });
        return term() && moreTerms();
    }

    bool moreTerms() {
        if (hasError) return false;

        string token = currentToken();
        if (token == "+" || token == "-") {
            replaceNonTerminal("moreTerms", { token, "terms" });
            return match(token) && terms();
        }
        replaceNonTerminal("moreTerms", {});
        return true;
    }

    bool term() {
        replaceNonTerminal("term", { "factor", "moreFactors" });
        return factor() && moreFactors();
    }

    bool moreFactors() {
        if (hasError) return false;

        string token = currentToken();
        if (token == "*" || token == "/") {
            replaceNonTerminal("moreFactors", { token, "term" });
            return match(token) && term();
        }
        replaceNonTerminal("moreFactors", {});
        return true;
    }

    bool factor() {
        if (hasError) return false;

        string token = currentToken();
        if (token == "id" || token == "num") {
            replaceNonTerminal("factor", { token });
            return match(token);
        }
        derivationFile << "Error at line " << lineNumber << ": Expected factor" << endl;
        hasError = true;
        return false;
    }

    bool condition() {
        replaceNonTerminal("condition", { "factor", "rel", "factor" });
        return factor() && rel() && factor();
    }

    bool rel() {
        if (hasError) return false;

        string token = currentToken();
        if (token == ">" || token == "<" || token == "==" || token == "!=") {
            replaceNonTerminal("rel", { token });
            return match(token);
        }
        derivationFile << "Error at line " << lineNumber << ": Expected relational operator" << endl;
        hasError = true;
        return false;
    }
};

bool readTokensFromFile(const string& filename, vector<string>& tokens) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return false;
    }

    string token;
    while (getline(file, token)) {
        if (!token.empty() && token.back() == '\r') {
            token.pop_back();
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return true;
}

int main() {
    string filename;
    cout << "Enter input file name: ";
    cin >> filename;

    vector<string> tokens;
    if (!readTokensFromFile(filename, tokens)) {
        return 1;
    }

    LL1Parser parser(tokens);
    if (!parser.parse()) {
        cout << "Parsing failed. Check derivation.txt for details." << endl;
        return 1;
    }

    cout << "Derivation written to derivation.txt" << endl;
    return 0;
}