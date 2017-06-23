#include "graphics/include/model/complex_mesh.h"

#include <cstdio>

static void PrintInfo(const char* app_name)
{
	printf("Mesh converter (c) Shtille, 2017\n");
	printf("Usage:\n%s <file in> <file out>\n", app_name);
}

int main(int argc, char const *argv[])
{
	if (argc != 3)
	{
		PrintInfo(argv[0]);
		return 1;
	}
	sht::graphics::ComplexMesh complex_mesh(nullptr/* renderer */, nullptr/* material_binder */);
	const char * file_in = argv[1];
	const char * file_out = argv[2];
	printf("Loading file %s\n", file_in);
	if (!complex_mesh.LoadFromFile(file_in))
	{
		fprintf(stderr, "File loading failed (%s)\n", file_in);
		return 2;
	}
	printf("Saving file %s\n", file_out);
	if (!complex_mesh.SaveToFile(file_out))
	{
		fprintf(stderr, "File saving failed (%s)\n", file_out);
		return 3;
	}
	printf("Done!\n");
	return 0;
}