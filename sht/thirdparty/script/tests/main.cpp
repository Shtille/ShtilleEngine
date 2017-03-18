#include "../src/script.h"

int plus(int a, int b) {
	return a + b;
}
void foo(int a, int b) {
	printf("%i\n", a + b);
}
void bar() {
	printf("void func\n");
}
void woo(console_script::String str) {
#ifndef PARSER_WIDE_STRING
	printf("woo %s\n", str.data());
#else
	wprintf(L"woo %ls\n", str.data());
#endif
}

class C {
public:
	C() {}

	void ci(int x)
	{
		printf("C::ci: %i\n", x);
	}
	void cv()
	{
		printf("C::cv\n");
	}
};

int main(int argc, char* argv[])
{
	console_script::Parser *script = new console_script::Parser();

	int x = 1;
	int y = 2;
	script->AddVariable(CS_TEXT("x"), &x);
	script->AddVariable(CS_TEXT("y"), &y);
	script->AddFunction(CS_TEXT("p"), &plus);
	script->AddFunction(CS_TEXT("f"), &foo);
	script->AddFunction(CS_TEXT("b"), &bar);
	script->AddFunction(CS_TEXT("w"), &woo);
	C c;
	script->AddClassFunction(CS_TEXT("ci"), &C::ci, &c);
	script->AddClassFunction(CS_TEXT("cv"), &C::cv, &c);

	console_script::String buffer(256, CS_TEXT('\0'));
	for (;;)
	{
#ifndef PARSER_WIDE_STRING
		if (fgets(&buffer[0], 256, stdin) != nullptr)
#else
		if (fgetws(&buffer[0], 256, stdin) != nullptr)
#endif
		{
			if (buffer[0] == CS_TEXT('\n'))
				break;
			console_script::String str;
			if (script->Evaluate(buffer, &str))
			{
#ifndef PARSER_WIDE_STRING
				printf("> %s\n", str.data());
#else
				wprintf(L"> %ls\n", str.data());
#endif
			}
			else
			{
#ifndef PARSER_WIDE_STRING
				printf(">! %s\n", script->error().c_str());
#else
				wprintf(L">! %ls\n", script->error().c_str());
#endif
			}
		}
	}

	delete script;

	return 0;
}

