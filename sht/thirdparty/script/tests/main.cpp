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

// class C {
// public:
// 	C() {}

// 	void c(int x)
// 	{
// 		printf("C::c: %i\n", x);
// 	}
// };

int main(int argc, char* argv[])
{
	console_script::Parser *script = new console_script::Parser();

	int x = 1;
	int y = 2;
	script->AddVariable("x", &x);
	script->AddVariable("y", &y);
	script->AddFunction("p", &plus);
	script->AddFunction("f", &foo);
	script->AddFunction("b", &bar);
	//C c;
	//script->AddFunction("c", std::bind(&C::c, &c));

	char buffer[256];
	for (;;)
	{
		if (fgets(buffer, 256, stdin) != nullptr)
		{
			if (buffer[0] == '\0')
				break;
			std::string str;
			if (script->Evaluate(buffer, &str))
			{
				printf("> %s\n", str.data());
			}
			else
			{
				printf(">! %s\n", script->error().c_str());
			}
		}
	}

	delete script;

	return 0;
}

