

foop(i, j)
short i, j;
{
}

float foo()
{
}

	WORD bletch;
	void *ptr;

	farp("Hi\014\037\253\036");
	farp(&foo, &bar, &bletch);
	bletch = foo + foop(1, 2);
	bletch = foo + foop;
	ptr = (void *)  &foo;
	ptr = (void *)  &bar;
	ptr = (void *)  &bletch;
	foo();
	bar();
	(*ptr)();
	(*bletch)();
	(*ptr)(1, 2);

