#include "Program.h"
#include <iostream>

int main()
{
	Program program(L"C:\\Users\\yuksu\\Desktop\\코알라가 어떻게 웃는지 아세요.coala");
	if (program.successed())
	{
		Error e = program.parsing();
		switch (e.error_t)
		{
		case ErrorType::OperationError:
			std::cout << "SyntaxError: OperationError, line:" << e.line_no;
			break;
		case ErrorType::ParenError:
			std::cout << "SyntaxError: Unmatched ')', line:" << e.line_no;
			break;
		case ErrorType::ZeroDivisionError:
			std::cout << "SyntaxError: Division by 0 is not Allowed, line:" << e.line_no;
			break;
		case ErrorType::InvalidType:
			std::cout << "SyntaxError: Invalid Type, line:" << e.line_no;
		default:
			program.show();
			break;
		}
	}
	return 0;
}