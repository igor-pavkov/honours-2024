static char * rv_alloc(int i) {
	int j, k, *r;

	j = sizeof(ULong);
	for(k = 0;
			sizeof(Bigint) - sizeof(ULong) - sizeof(int) + j <= i;
			j <<= 1) {
		k++;
	}
	r = (int*)Balloc(k);
	*r = k;
	return (char *)(r+1);
}