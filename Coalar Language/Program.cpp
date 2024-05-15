#include "Program.h"
#include <iostream>
#include <sstream>

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
		default:
			m_successed = true;
			//m_tokenizer.show();
			break;
		}
	}
	else
	{
		std::cout << "������ �� �� �����ϴ�.";
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
		if (isError(e))
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
	{
		varIdx = m_currentToken.value;

		m_currentToken = nextToken();
		if (m_currentToken.t_type != TokenType::ASSIGN)
			return Error(ErrorType::OperationError, line);
		m_currentToken = nextToken();

		Error e = expression();
		if (isError(e))
			return e;
		variables[varIdx] = pop();
	}
		break;
	case TokenType::FUNCTION:
	{
		std::vector<Literal> params;
		m_parameterStack.push(params);
		Error e = processFunction();
		if (isError(e))
			return e;
		if (m_currentToken.t_type != TokenType::RPAREN)
			return Error(ErrorType::ParenError, line);
		m_currentToken = nextToken();
		m_parameterStack.pop();
	}
		break;
	}
	if (m_currentToken.t_type != TokenType::EOL)
		return Error(ErrorType::OperationError, line);
	while (!m_stack.empty())
		pop();
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
	case TokenType::FUNCTION:
	{
		std::vector<Literal> params;
		m_parameterStack.push(params);
		Error e = processFunction();
		if (isError(e))
			return e;
		if (m_currentToken.t_type != TokenType::RPAREN)
			return Error(ErrorType::ParenError, line);
		m_parameterStack.pop();
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

Error Program::processFunction()
{
	auto fname = m_currentToken.str;
	m_currentToken = nextToken();
	if (m_currentToken.t_type != TokenType::LPAREN)
		return Error(ErrorType::ParenError, line);

	if (fname == L"ȣ�D��") // print
	{
		Error e = getParameter(-1);
		if (isError(e))
			return e;

		// ���
		for (auto& liter : m_parameterStack.top()) {
			if (liter.lt == LiteralType::NUMBER)
				std::cout << liter.number;
			else if (liter.lt == LiteralType::STRING)
				std::wcout << liter.str;
			else
				std::cout << "None";
		}
		std::cout << "\n";
		m_stack.push(Literal(LiteralType::VOID, -1));
	}
	else if (fname == L"��Į���S����") // input
	{
		Error e = getParameter(1);
		if (isError(e))
			return e;
		if (m_parameterStack.top().begin()->lt != LiteralType::STRING)
			return Error(ErrorType::ParameterError, line);
		else
		{
			std::wstring result;
			std::wcout << m_parameterStack.top().begin()->str;
			std::wcin >> result;
			m_stack.push(Literal(LiteralType::STRING, result));
		}
	}
	else if (fname == L"���?") // int
	{
		Error e = getParameter(1);
		if (isError(e))
			return e;
		if (m_parameterStack.top().begin()->lt != LiteralType::STRING)
			return Error(ErrorType::ParameterError, line);
		else
		{
			std::wstringstream ssInt(m_parameterStack.top().begin()->str);
			int i;
			ssInt >> i;
			if (ssInt.fail())
				return Error(ErrorType::ToIntTypeError, line);
			m_stack.push(Literal(LiteralType::NUMBER, i));
		}
	}
	else if (fname == L"�ݶ�?") // str
	{
		Error e = getParameter(1);
		if (isError(e))
			return e;
		switch (m_parameterStack.top().begin()->lt)
		{
		case LiteralType::NUMBER:
			m_stack.push(Literal(LiteralType::STRING, std::to_wstring(m_parameterStack.top().begin()->number)));
			break;
		case LiteralType::STRING:
			m_stack.push(Literal(LiteralType::STRING, m_parameterStack.top().begin()->str));
			break;
		case LiteralType::VOID:
			m_stack.push(Literal(LiteralType::STRING, L"None"));
			break;
		}
	}
	else
		return Error(ErrorType::NotDefined, line);
	return NoError();
}

Error Program::getParameter(int size)
{
	if (size == -1)
	{
		do {
			m_currentToken = nextToken();
			Error e = term(1);
			if (isError(e))
				return e;
			m_parameterStack.top().push_back(pop());
		} while (m_currentToken.t_type == TokenType::COMMA);
	}
	else if (size == 0)
	{
		m_currentToken = nextToken();
	}
	else 
	{
		for (int i = 0; i < size; ++i)
		{
			m_currentToken = nextToken();
			Error e = term(1);
			if (isError(e))
				return e;
			m_parameterStack.top().push_back(pop());
		}		
	}
	if (m_currentToken.t_type != TokenType::RPAREN)
		return Error(ErrorType::ParameterError, line);
	return NoError();
}

LiteralType Program::getFunctionType(const std::wstring& name)
{
	if (name == L"ȣ�D��")
		return LiteralType::VOID;
	else if (name == L"��Į���S����")
		return LiteralType::STRING;
	else if (name == L"���?")
		return LiteralType::NUMBER;
	else if (name == L"�ݶ�?")
		return LiteralType::STRING;
}

bool Program::isError(Error& e)
{
	return e.error_t != ErrorType::NoError;
}

bool Program::isOperator(TokenType t)
{
	switch (t)
	{
	case TokenType::PLUS:
	case TokenType::MINUS:
	case TokenType::MUTLI:
	case TokenType::DIV:
	case TokenType::ASSIGN:
		return true;
	default:
		return false;
	}
}

Literal Program::pop()
{
	Literal result = m_stack.top();
	m_stack.pop();
	return result;
}