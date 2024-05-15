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
		
		// 해당 라인의 토큰 리스트 생성
		token_lists.push_back(std::vector<Token>());

		// 문자열이 비었다면 다음으로 이동
		if (line.empty())
			continue;
		
		// 양옆의 공백을 제거
		line.erase(0, line.find_first_not_of(' '));
		line.erase(line.find_last_not_of(' ') + 1);
		
		// 첫 라인인경우 형식을 맞췄는지 판단
		if (start) {
			start = false;
			if (line.compare(L"코알라가 어떻게 웃는지 아세요?") != 0)
				return Error(ErrorType::FirstFormat, nline);
			continue; // 바로 다음으로 진행
		}
		// eof 판단
		else if (line.compare(L"라고 웃어요") == 0) {
			eof = true;
			break;
		}

		// 문자열 split을 위한 변수
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
	if (str.compare(L"코앜ㅋ") == 0) {
		token_lists[idx].push_back(Token(TokenType::PLUS, str));
	}
	else if (str.compare(L"앜ㅋㅋ") == 0) {
		token_lists[idx].push_back(Token(TokenType::MINUS, str));
	}
	else if (str.compare(L"알랔ㅋ") == 0) {
		token_lists[idx].push_back(Token(TokenType::MUTLI, str));
	}
	else if (str.compare(L"랔ㅋㅋ") == 0) {
		token_lists[idx].push_back(Token(TokenType::DIV, str));
	}
	else if (str.compare(L"유칼맄ㅋ") == 0) {
		token_lists[idx].push_back(Token(TokenType::ASSIGN, str));
	}
	else if (str.compare(L"콸랔ㅋ") == 0) {
		token_lists[idx].push_back(Token(TokenType::LPAREN, str));
	}
	else if (str.compare(L"ㅋ콸랔") == 0) {
		token_lists[idx].push_back(Token(TokenType::RPAREN, str));
	}
	else if (str.compare(L"코ㅇㅏ") == 0) {
		token_lists[idx].push_back(Token(TokenType::COMMA, str));
	}
	else if (str.find(L"콬ㅋ") == 0) {
		int num = processNumber(str);
		if (num != -1)
			token_lists[idx].push_back(Token(TokenType::NUMBER, str, num));
		else
			return Error(ErrorType::InvalidNumberFormat, nline);
	}
	else if (str.find(L"콜ㅋ") == 0) {
		int num = processNumber(str);
		if (num != -1)
			token_lists[idx].push_back(Token(TokenType::STRING, str, num));
		else
			return Error(ErrorType::InvalidStringFormat, nline);
	}
	else if (str.find(L"코알랔") == 0) {
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
		if (str[i] == L'코')
			result += pow(2, power++);
		else if (str[i] == L'알')
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
		if (str[i] == L'ㅋ')
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
