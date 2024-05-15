#pragma once
#include "token.h"
#include <stack>
#define MAX_VARIABLE_COUNTS 512

enum class LiteralType
{
	NUMBER,
	STRING,
	VOID
};

struct Literal
{
	Literal() = default;
	Literal(LiteralType lt, int number) : lt(lt), number(number) {}
	Literal(LiteralType lt, const std::wstring& str) : lt(lt), str(str) {}

	LiteralType lt;
	int number;
	std::wstring str;
};

class Program
{
public:
	Program(const std::wstring& fname);

	Error parsing();
	bool successed() { return m_successed; }

private:
	Token nextToken();
	Error statement();
	Error expression();
	Error term(int priority);
	Error factor();

	int opOrder(TokenType t);
	Error operate(TokenType op);

	Error processFunction();
	Error getParameter(int size);
	LiteralType getFunctionType(const std::wstring& name);

	bool isError(Error& e);
	bool isOperator(TokenType t);
	Literal pop();

private:
	Tokenizer m_tokenizer;
	
	// 현재 라인의 토큰 리스트
	std::vector<Token> m_currentLine;
	Token m_currentToken;
	int m_idx = 0;

	// 연산 수행용 스택
	std::stack<Literal> m_stack;

	// 함수 매개 변수용 스택
	std::stack<std::vector<Literal>> m_parameterStack;

	// 변수 리스트 최대 512개
	Literal variables[MAX_VARIABLE_COUNTS];

	int		line = 1;
	bool	m_successed = false;
};

