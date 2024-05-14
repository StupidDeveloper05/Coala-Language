#include "Program.h"
#include <iostream>

Error NoError() { return Error(ErrorType::NoError, -1); }

Program::Program(const std::wstring& fname)
{
	m_tokenizer = Tokenizer(fname);
	if (m_tokenizer.successed())
	{
		Error e = m_tokenizer.process();
		switch (e.error_t)
		{
		case ErrorType::FirstFormat:
			std::cout << "SyntaxError: FirstFormat, line:" << e.line_no;
			break;
		case ErrorType::LastFormat:
			std::cout << "SyntaxError: LastFormat, line:" << e.line_no;
			break;
		case ErrorType::InvalidNumberFormat:
			std::cout << "SyntaxError: Invalid Number Format, line:" << e.line_no;
			break;
		case ErrorType::InvalidStringFormat:
			std::cout << "SyntaxError: Invalid String Format, line:" << e.line_no;
			break;
		case ErrorType::NotDefined:
			std::cout << "SyntaxError: Invalid Variable Name, line:" << e.line_no;
		default:
			m_successed = true;
			m_tokenizer.show();
			break;
		}
	}
	else
	{
		std::cout << "파일을 열 수 없습니다.";
	}
}

Error Program::parsing()
{
	for (auto& list: m_tokenizer.token_lists)
	{
		line++;
		if (list.empty())
			continue;
		m_idx = 0;
		m_currentLine = list;
		m_currentToken = m_currentLine[m_idx];
		Error e = statement();
		if (e.error_t != ErrorType::NoError)
			return e;
	}
	return Error(ErrorType::NoError, -1);
}

Token Program::nextToken()
{
	if (m_currentLine.size() > ++m_idx)
		return m_currentLine[m_idx];
	else
		return Token(TokenType::EOL, L"");
}

Error Program::statement()
{
	int varIdx;
	switch (m_currentToken.t_type) 
	{
	case TokenType::VAR:
		varIdx = m_currentToken.value;

		m_currentToken = nextToken();
		if (m_currentToken.t_type != TokenType::ASSIGN)
			return Error(ErrorType::OperationError, line);
		m_currentToken = nextToken();

		Error e = expression();
		if (isError(e))
			return e;
		
		variables[varIdx] = pop();
		break;
	}
	if (m_currentToken.t_type != TokenType::EOL)
		return Error(ErrorType::OperationError, line);
	return NoError();
}

Error Program::expression()
{
	Error e = term(1);
	return e;
}

Error Program::term(int priority)
{
	TokenType op;
	
	if (priority == 3)
	{
		Error e = factor();
		return e;
	}

	Error e = term(priority + 1);
	if (isError(e))
		return e;

	while (priority == opOrder(m_currentToken.t_type)) {
		op = m_currentToken.t_type;
		m_currentToken = nextToken();
		Error e = term(priority + 1);
		if (isError(e))
			return e;
		e = operate(op);
		if (isError(e))
			return e;
	}
	return NoError();
}

Error Program::factor()
{
	switch (m_currentToken.t_type) 
	{
	case TokenType::VAR:
		m_stack.push(variables[m_currentToken.value]);
		break;
	case TokenType::NUMBER:
		m_stack.push(Literal(LiteralType::NUMBER, m_currentToken.value));
		break;
	case TokenType::STRING:
		m_stack.push(Literal(LiteralType::STRING, std::wstring(L"") + wchar_t(m_currentToken.value)));
		break;
	case TokenType::LPAREN:
	{
		m_currentToken = nextToken();

		Error e = expression();
		if (e.error_t != ErrorType::NoError)
			return e;
		if (m_currentToken.t_type != TokenType::RPAREN)
			return Error(ErrorType::ParenError, line);
	}
		break;
	default:
		return Error(ErrorType::OperationError, line);
	}
	m_currentToken = nextToken();
	return NoError();
}

int Program::opOrder(TokenType t)
{
	switch (t)
	{
	case TokenType::MUTLI: case TokenType::DIV: return 2;
	case TokenType::PLUS: case TokenType::MINUS: return 1;
	default: return 0;
	}
}

Error Program::operate(TokenType op)
{
	Literal l1 = pop();
	Literal l2 = pop();

	if (l1.lt == LiteralType::NUMBER && l2.lt == LiteralType::NUMBER) {
		if (op == TokenType::DIV && l1.number == 0)
			return Error(ErrorType::ZeroDivisionError, line);
		switch (op) {
		case TokenType::PLUS:
			m_stack.push(Literal(LiteralType::NUMBER, l1.number + l2.number));
			break;
		case TokenType::MINUS:
			m_stack.push(Literal(LiteralType::NUMBER, l1.number - l2.number));
			break;
		case TokenType::MUTLI:
			m_stack.push(Literal(LiteralType::NUMBER, l1.number * l2.number));
			break;
		case TokenType::DIV:
			m_stack.push(Literal(LiteralType::NUMBER, l2.number / l1.number));
			break;
		}
	}
	else if (l1.lt == LiteralType::STRING && l2.lt == LiteralType::STRING)
	{
		switch (op) {
		case TokenType::PLUS:
			m_stack.push(Literal(LiteralType::STRING, l2.str + l1.str));
			break;
		default:
			return Error(ErrorType::OperationError, line);
		}
	}
	else if (l1.lt == LiteralType::NUMBER && l2.lt == LiteralType::STRING)
	{
		switch (op) {
		case TokenType::MUTLI:
		{
			std::wstring str = L"";
			for (int i = 0; i < l1.number; ++i)
				str += l2.str;
			m_stack.push(Literal(LiteralType::STRING, str));
		}
			break;
		default:
			return Error(ErrorType::OperationError, line);
		}
	}
	else
		return Error(ErrorType::OperationError, line);
	return NoError();
}

bool Program::isError(Error& e)
{
	return e.error_t != ErrorType::NoError;
}


Literal Program::pop()
{
	Literal result = m_stack.top();
	m_stack.pop();
	return result;
}

void Program::show()
{
	for (int i = 0; i < MAX_VARIABLE_COUNTS; ++i)
	{
		if (variables->lt == LiteralType::NUMBER)
			std::cout << variables[i].number << std::endl;
		else
			std::wcout << variables[i].str << std::endl;
	}
}