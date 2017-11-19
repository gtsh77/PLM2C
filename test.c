

extern farp();


slug()
{
	void *ptr;
	short i;
	short **iptr = (short **) &ptr;
	float j;
	float k;
	float l;
	WORD mqaFOO;
	DWORD fooBAR;

	ptr = (void *)  &i;
	(**iptr) = 72;
	iptfil();
	setinterrput(0, farp);
	signal(abs(i), (short) (i));
	j = (float) ((short) (i));

} /* slug */
	

