#pragma once
#include <string>
#include <vector>

enum class TokenType
{	
	PLUS,
	MINUS,
	MUTLI,
	DIV,
	LPAREN,
	RPAREN,
	COMMA,
	ASSIGN,
	NUMBER,
	STRING,
	VAR,
	FUNCTION,
	EOL
};

struct Token {
	Token() = default;
	Token(TokenType type, const std::wstring& str) : t_type(type), str(str), value(0) {}
	Token(TokenType type, const std::wstring& str, int value) : t_type(type), str(str), value(value) {}
	TokenType t_type;
	std::wstring str;
	int value;
};

enum class ErrorType {
	FirstFormat,
	LastFormat,
	InvalidNumberFormat,
	InvalidStringFormat,
	NotDefined,
	OperationError,
	ParenError,
	ZeroDivisionError,
	InvalidType,
	NoError
};

struct Error {
	Error(ErrorType eType, int nline) : error_t(eType), line_no(nline) {}
	ErrorType error_t;
	int line_no;
};

class Tokenizer
{
public:
	Tokenizer() = default;
	Tokenizer(const std::wstring& f_name);
	
	Error process();
	bool successed() { return flag; }

	void show();
private:
	Error tokenizer(const std::wstring& str, int idx, int nline);
	int processNumber(const std::wstring& str);
	int processVariable(const std::wstring& str);

private:
	std::vector<std::wstring> lines;
	std::vector<std::vector<Token>> token_lists;
	bool flag = true;

	friend class Program;
};