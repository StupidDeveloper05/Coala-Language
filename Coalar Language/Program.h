#pragma once
#include "token.h"
#include <stack>
#define MAX_VARIABLE_COUNTS 10

enum class LiteralType
{
	NUMBER,
	STRING
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

	void show();

private:
	Token nextToken();
	Error statement();
	Error expression();
	Error term(int priority);
	Error factor();

	int opOrder(TokenType t);
	Error operate(TokenType op);

	bool isError(Error& e);
	Literal pop();

private:
	Tokenizer m_tokenizer;
	
	// ���� ������ ��ū ����Ʈ
	std::vector<Token> m_currentLine;
	Token m_currentToken;
	int m_idx = 0;

	// ���� ����� ����
	std::stack<Literal> m_stack;

	// ���� ����Ʈ �ִ� 512��
	Literal variables[MAX_VARIABLE_COUNTS];

	int		line = 1;
	bool	m_successed = false;
};

