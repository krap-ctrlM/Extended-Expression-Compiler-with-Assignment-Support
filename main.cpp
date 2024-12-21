#include <iostream>
#include <cctype>
#include <string>

using namespace std;

char Look;

void GetChar() {
    cin.get(Look);
}

void Error(const string& s) {
    cout << endl << "\aError: " << s << "." << endl;
}

void Abort(const string& s) {
    Error(s);
    exit(1);
}

void Expected(const string& s) {
    Abort(s + " Expected");
}

bool IsAlpha(char c) {
    return isalpha(c);
}

bool IsDigit(char c) {
    return isdigit(c);
}

bool IsAlNum(char c) {
    return isalnum(c);
}

bool IsAddop(char c) {
    return (c == '+' || c == '-');
}

bool IsWhite(char c) {
    return (c == ' ' || c == '\t');
}

void SkipWhite() {
    while (IsWhite(Look))
        GetChar();
}

void Match(char x) {
    if (Look != x)
        Expected("'" + string(1, x) + "'");
    else {
        GetChar();
        SkipWhite();
    }
}

string GetName() {
    string token;
    if (!IsAlpha(Look))
        Expected("Name");
    while (IsAlNum(Look)) {
        token += toupper(Look);
        GetChar();
    }
    SkipWhite();
    return token;
}

string GetNum() {
    string value;
    if (!IsDigit(Look))
        Expected("Integer");
    while (IsDigit(Look)) {
        value += Look;
        GetChar();
    }
    SkipWhite();
    return value;
}

void Emit(const string& s) {
    cout << '\t' << s;
}

void EmitLn(const string& s) {
    Emit(s);
    cout << endl;
}

void Ident() {
    string name = GetName();
    if (Look == '(') {
        Match('(');
        Match(')');
        EmitLn("BSR " + name);
    } else {
        EmitLn("MOVE " + name + "(PC),D0");
    }
}

void Expression();

void Factor() {
    if (Look == '(') {
        Match('(');
        Expression();
        Match(')');
    } else if (IsAlpha(Look)) {
        Ident();
    } else {
        EmitLn("MOVE #" + GetNum() + ",D0");
    }
}

void Multiply() {
    Match('*');
    Factor();
    EmitLn("MULS (SP)+,D0");
}

void Divide() {
    Match('/');
    Factor();
    EmitLn("MOVE (SP)+,D1");
    EmitLn("EXS.L D0");
    EmitLn("DIVS D1,D0");
}

void Term() {
    Factor();
    while (Look == '*' || Look == '/') {
        EmitLn("MOVE D0,-(SP)");
        switch (Look) {
        case '*':
            Multiply();
            break;
        case '/':
            Divide();
            break;
        default:
            Expected("Mulop");
            break;
        }
    }
}

void Add() {
    Match('+');
    Term();
    EmitLn("ADD (SP)+,D0");
}

void Subtract() {
    Match('-');
    Term();
    EmitLn("SUB (SP)+,D0");
    EmitLn("NEG D0");
}

void Expression() {
    if (IsAddop(Look))
        EmitLn("CLR D0");
    else
        Term();

    while (IsAddop(Look)) {
        EmitLn("MOVE D0,-(SP)");
        switch (Look) {
        case '+':
            Add();
            break;
        case '-':
            Subtract();
            break;
        default:
            Expected("Addop");
            break;
        }
    }
}

void Assignment() {
    string name = GetName();
    Match('=');
    Expression();
    EmitLn("LEA " + name + "(PC),A0");
    EmitLn("MOVE D0,(A0)");
}

void Init() {
    GetChar();
    SkipWhite();
}

int main() {
    Init();
    Assignment();
    if (Look != '\n')
        Expected("NewLine");
    return 0;
}
