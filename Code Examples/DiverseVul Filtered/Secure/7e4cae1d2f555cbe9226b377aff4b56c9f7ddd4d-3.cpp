void bn_mul_low_recursive(BN_ULONG *r, BN_ULONG *a, BN_ULONG *b, int n2,
	     BN_ULONG *t)
	{
	int n=n2/2;

# ifdef BN_COUNT
	fprintf(stderr," bn_mul_low_recursive %d * %d\n",n2,n2);
# endif

	bn_mul_recursive(r,a,b,n,0,0,&(t[0]));
	if (n >= BN_MUL_LOW_RECURSIVE_SIZE_NORMAL)
		{
		bn_mul_low_recursive(&(t[0]),&(a[0]),&(b[n]),n,&(t[n2]));
		bn_add_words(&(r[n]),&(r[n]),&(t[0]),n);
		bn_mul_low_recursive(&(t[0]),&(a[n]),&(b[0]),n,&(t[n2]));
		bn_add_words(&(r[n]),&(r[n]),&(t[0]),n);
		}
	else
		{
		bn_mul_low_normal(&(t[0]),&(a[0]),&(b[n]),n);
		bn_mul_low_normal(&(t[n]),&(a[n]),&(b[0]),n);
		bn_add_words(&(r[n]),&(r[n]),&(t[0]),n);
		bn_add_words(&(r[n]),&(r[n]),&(t[n]),n);
		}
	}