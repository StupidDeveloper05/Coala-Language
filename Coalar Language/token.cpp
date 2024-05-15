#include "token.h"
#include <iostream>
#include <fstream>
#include <sstream>

Tokenizer::Tokenizer(const std::wstring& f_name)
{
	std::locale::global(std::locale(".UTF-8"));
	std::wstring line;
	std::wifstream file(f_name);
	if (file.is_open()) {
		while (std::getline(file, line)) {
			lines.push_back(line);
		}
		file.close();
	}
	else
		flag = false;
}

Error Tokenizer::process()
{
	int nline = 0, token_list_idx = 0;
	bool start = true, eof = false;
	for (std::wstring& line : lines) {
		nline++;
		
		// �ش� ������ ��ū ����Ʈ ����
		token_lists.push_back(std::vector<Token>());

		// ���ڿ��� ����ٸ� �������� �̵�
		if (line.empty())
			continue;
		
		// �翷�� ������ ����
		line.erase(0, line.find_first_not_of(' '));
		line.erase(line.find_last_not_of(' ') + 1);
		
		// ù �����ΰ�� ������ ������� �Ǵ�
		if (start) {
			start = false;
			if (line.compare(L"�ھ˶� ��� ������ �Ƽ���?") != 0)
				return Error(ErrorType::FirstFormat, nline);
			continue; // �ٷ� �������� ����
		}
		// eof �Ǵ�
		else if (line.compare(L"��� �����") == 0) {
			eof = true;
			break;
		}

		// ���ڿ� split�� ���� ����
		std::wstring str;
		std::wistringstream ss(line);
		while (std::getline(ss, str, L' ')) {
			if (str.empty())
				continue;
			auto e = tokenizer(str, token_list_idx, nline);
			if (e.error_t != ErrorType::NoError)
				return e;
		}
		token_list_idx++;
	}
	if (!eof) {
		return Error(ErrorType::LastFormat, nline);
	}
	return Error(ErrorType::NoError, -1);
}

Error Tokenizer::tokenizer(const std::wstring& str, int idx, int nline)
{
	if (str.compare(L"�ڝڤ�") == 0) {
		token_lists[idx].push_back(Token(TokenType::PLUS, str));
	}
	else if (str.compare(L"�ڤ���") == 0) {
		token_lists[idx].push_back(Token(TokenType::MINUS, str));
	}
	else if (str.compare(L"�ˍ���") == 0) {
		token_lists[idx].push_back(Token(TokenType::MUTLI, str));
	}
	else if (str.compare(L"������") == 0) {
		token_lists[idx].push_back(Token(TokenType::DIV, str));
	}
	else if (str.compare(L"��Į����") == 0) {
		token_lists[idx].push_back(Token(TokenType::ASSIGN, str));
	}
	else if (str.compare(L"�����") == 0) {
		token_lists[idx].push_back(Token(TokenType::LPAREN, str));
	}
	else if (str.compare(L"�����") == 0) {
		token_lists[idx].push_back(Token(TokenType::RPAREN, str));
	}
	else if (str.compare(L"�ڤ���") == 0) {
		token_lists[idx].push_back(Token(TokenType::COMMA, str));
	}
	else if (str.find(L"����") == 0) {
		int num = processNumber(str);
		if (num != -1)
			token_lists[idx].push_back(Token(TokenType::NUMBER, str, num));
		else
			return Error(ErrorType::InvalidNumberFormat, nline);
	}
	else if (str.find(L"�ݤ�") == 0) {
		int num = processNumber(str);
		if (num != -1)
			token_lists[idx].push_back(Token(TokenType::STRING, str, num));
		else
			return Error(ErrorType::InvalidStringFormat, nline);
	}
	else if (str.find(L"�ھˍ�") == 0) {
		int value = processVariable(str);
		if (value != -1)
			token_lists[idx].push_back(Token(TokenType::VAR, str, value));
		else
			return Error(ErrorType::NotDefined, nline);
	}
	else {
		token_lists[idx].push_back(Token(TokenType::FUNCTION, str));
	}
	return Error(ErrorType::NoError, -1);
}

int Tokenizer::processNumber(const std::wstring& str)
{
	int power = 0;
	int result = 0;
	for (int i = str.size() - 1; i > 1; --i) {
		if (str[i] == L'��')
			result += pow(2, power++);
		else if (str[i] == L'��')
			power++;
		else
			return -1;
	}
	return result;
}

int Tokenizer::processVariable(const std::wstring& str)
{
	int result = 0;
	for (int i = 3; i < str.size(); ++i)
	{
		if (str[i] == L'��')
			result++;
		else
			return -1;
	}
	return result;
}

void Tokenizer::show()
{
	for (auto& list : token_lists) {
		for (auto& token : list) {
			std::wstring type;
			switch (token.t_type)
			{
			case TokenType::PLUS:
				type = L"PLUS";
				break;
			case TokenType::MINUS:
				type = L"MINUS";
				break;
			case TokenType::MUTLI:
				type = L"MULTI";
				break;
			case TokenType::DIV:
				type = L"DIV";
				break;
			case TokenType::LPAREN:
				type = L"LPAREN";
				break;
			case TokenType::RPAREN:
				type = L"RPAREN";
				break;
			case TokenType::COMMA:
				type = L"COMMA";
				break;
			case TokenType::ASSIGN:
				type = L"ASSIGN";
				break;
			case TokenType::NUMBER:
				type = L"NUMBER(" + std::to_wstring(token.value) + L")";
				break;
			case TokenType::STRING:
				type = std::wstring(L"STRING(") + wchar_t(token.value) + std::wstring(L")");
				break;
			case TokenType::VAR:
				type = L"VAR(" + std::to_wstring(token.value) + L")";
				break;
			case TokenType::FUNCTION:
				type = L"FUNCTION";
				break;
			}
			std::wcout << type << L" ";
		}
		std::cout << std::endl;
	}
}
