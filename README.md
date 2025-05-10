# LL(1) Parser
LL(1) Parser
Overview
This program implements an LL(1) parser in C++ that can parse a specific subset of a simple programming language. The parser reads a sequence of tokens from an input file, applies LL(1) parsing rules, and generates a derivation of the program's syntax. Any errors encountered during parsing are logged in a derivation.txt file.

The language consists of constructs like conditionals (if), loops (while), input/output (get, put), and assignments. The parser supports simple expressions involving terms and factors.

Features
LL(1) Parsing: The parser uses a top-down parsing approach based on an LL(1) grammar.

Error Handling: The program generates detailed error messages if parsing fails.

Derivation Output: The derivation of the program is written to derivation.txt, showing the left-most derivation process.

Token File Input: Tokens are read from an input file (e.g., input.txt).

Prerequisites
A C++ compiler supporting C++11 or higher.

An input token file, such as input.txt.

How to Run
Step 1: Compile the Code
bash
Copy
Edit
g++ LL1Parser.cpp -o LL1Parser
Step 2: Prepare the Input File
Create a text file (e.g., input.txt) with the tokens for the program. Each token should be on a new line. Here’s an example of an input file:

txt
Copy
Edit
begin
id
=
id
-
num
,
put
(
id
)
,
end.
Step 3: Run the Program
After compiling the program, run it using the following command:

bash
Copy
Edit
./LL1Parser
You will be prompted to enter the name of the token file (e.g., input.txt).

Step 4: View the Output
If the parsing is successful, a file called derivation.txt will be generated in the working directory, containing the left-most derivation of the program. If there are any errors during parsing, they will be logged in the same file.

How It Works
Core Logic
The parser class LL1Parser is responsible for:

Reading the tokens: The program reads the tokens from an input file and stores them in a vector.

Matching tokens: The match function checks if the current token matches the expected one and moves to the next token.

Deriving non-terminals: The parser applies production rules to replace non-terminals with corresponding productions using the replaceNonTerminal function.

Error handling: If any error occurs during parsing, such as an unexpected token, an error message is logged in derivation.txt.

Left-most derivation: The parser writes the step-by-step left-most derivation of the program to the derivation.txt file.

LL(1) Grammar
The grammar used by the parser is a simple programming language with the following constructs:

Statements: Can be conditionals, loops, input/output, assignments, or the nothing keyword.

Expression: An expression can be a term or a parenthesized expression.

Terms and Factors: Expressions consist of terms and factors, where terms are connected by + and -, and factors are connected by * and /.

Relational Expressions: Conditions consist of two factors connected by relational operators (<, >, ==, !=).

Example of Left-Most Derivation
Given an input file with the following tokens:

txt
Copy
Edit
begin
id
=
id
-
num
,
put
(
id
)
,
end.
The output derivation.txt will look like this:

txt
Copy
Edit
Left most derivation:
program -> begin statements end.
-> begin statement , moreStmts end.
-> begin statement , statement , moreStmts end.
-> begin assignment , statement , moreStmts end.
-> begin id = expr , statement , moreStmts end.
-> begin id = terms , statement , moreStmts end.
-> begin id = term moreTerms , statement , moreStmts end.
-> begin id = factor moreFactors , statement , moreStmts end.
-> begin id = id moreFactors , statement , moreStmts end.
-> begin id = id , statement , moreStmts end.
-> begin id = id , io , moreStmts end.
-> begin id = id , put ( id ) , moreStmts end.
-> begin id = id , put ( id ) , end.
Error Handling
Errors are written to the derivation.txt file, specifying the line number and the nature of the issue. For example:

pgsql
Copy
Edit
Error at line 3: Expected 'id', found 'num'
Notes
The input file should be formatted such that each token is on its own line.

The parser expects a valid program based on the LL(1) grammar. If the program deviates from the expected syntax, errors will be logged.
