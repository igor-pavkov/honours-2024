BIGNUM *BN_copy(BIGNUM *a, const BIGNUM *b)
	{
	int i;
	BN_ULONG *A;
	const BN_ULONG *B;

	bn_check_top(b);

	if (a == b) return(a);
	if (bn_wexpand(a,b->top) == NULL) return(NULL);

#if 1
	A=a->d;
	B=b->d;
	for (i=b->top>>2; i>0; i--,A+=4,B+=4)
		{
		BN_ULONG a0,a1,a2,a3;
		a0=B[0]; a1=B[1]; a2=B[2]; a3=B[3];
		A[0]=a0; A[1]=a1; A[2]=a2; A[3]=a3;
		}
	switch (b->top&3)
		{
		case 3: A[2]=B[2];
		case 2: A[1]=B[1];
		case 1: A[0]=B[0];
		case 0: ; /* ultrix cc workaround, see comments in bn_expand_internal */
		}
#else
	memcpy(a->d,b->d,sizeof(b->d[0])*b->top);
#endif

	a->top=b->top;
	a->neg=b->neg;
	bn_check_top(a);
	return(a);
	}