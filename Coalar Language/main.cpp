#include "Program.h"
#include <iostream>

int wmain(int argc, wchar_t** argv)
{
	std::wcin.imbue(std::locale("korean"));
	if (argc == 1)
	{
		std::cout << "Please enter filename";
	}
	else
	{
		Program program(argv[1]);
		if (program.successed())
		{
			Error e = program.parsing();
			switch (e.error_t)
			{
			case ErrorType::OperationError:
				std::cout << "SyntaxError: OperationError, line:" << e.line_no;
				break;
			case ErrorType::ParenError:
				std::cout << "SyntaxError: Unmatched 'ㅋ콸랔', line:" << e.line_no;
				break;
			case ErrorType::ZeroDivisionError:
				std::cout << "SyntaxError: Division by 0 is not Allowed, line:" << e.line_no;
				break;
			case ErrorType::InvalidType:
				std::cout << "SyntaxError: Invalid Type, line:" << e.line_no;
				break;
			case ErrorType::NotDefined:
				std::cout << "SyntaxError: Undefined Identifier, line:" << e.line_no;
				break;
			case ErrorType::ParameterError:
				std::cout << "SyntaxError: Invalid Parameter, line:" << e.line_no;
				break;
			case ErrorType::ToIntTypeError:
				std::cout << "SyntaxError: Can't convert to int, line:" << e.line_no;
				break;
			default:
				system("pause");
				break;
			}
		}
	}	
	return 0;
}