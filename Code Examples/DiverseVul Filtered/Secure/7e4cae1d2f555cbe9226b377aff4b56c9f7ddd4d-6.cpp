void bn_mul_high(BN_ULONG *r, BN_ULONG *a, BN_ULONG *b, BN_ULONG *l, int n2,
	     BN_ULONG *t)
	{
	int i,n;
	int c1,c2;
	int neg,oneg,zero;
	BN_ULONG ll,lc,*lp,*mp;

# ifdef BN_COUNT
	fprintf(stderr," bn_mul_high %d * %d\n",n2,n2);
# endif
	n=n2/2;

	/* Calculate (al-ah)*(bh-bl) */
	neg=zero=0;
	c1=bn_cmp_words(&(a[0]),&(a[n]),n);
	c2=bn_cmp_words(&(b[n]),&(b[0]),n);
	switch (c1*3+c2)
		{
	case -4:
		bn_sub_words(&(r[0]),&(a[n]),&(a[0]),n);
		bn_sub_words(&(r[n]),&(b[0]),&(b[n]),n);
		break;
	case -3:
		zero=1;
		break;
	case -2:
		bn_sub_words(&(r[0]),&(a[n]),&(a[0]),n);
		bn_sub_words(&(r[n]),&(b[n]),&(b[0]),n);
		neg=1;
		break;
	case -1:
	case 0:
	case 1:
		zero=1;
		break;
	case 2:
		bn_sub_words(&(r[0]),&(a[0]),&(a[n]),n);
		bn_sub_words(&(r[n]),&(b[0]),&(b[n]),n);
		neg=1;
		break;
	case 3:
		zero=1;
		break;
	case 4:
		bn_sub_words(&(r[0]),&(a[0]),&(a[n]),n);
		bn_sub_words(&(r[n]),&(b[n]),&(b[0]),n);
		break;
		}
		
	oneg=neg;
	/* t[10] = (a[0]-a[1])*(b[1]-b[0]) */
	/* r[10] = (a[1]*b[1]) */
# ifdef BN_MUL_COMBA
	if (n == 8)
		{
		bn_mul_comba8(&(t[0]),&(r[0]),&(r[n]));
		bn_mul_comba8(r,&(a[n]),&(b[n]));
		}
	else
# endif
		{
		bn_mul_recursive(&(t[0]),&(r[0]),&(r[n]),n,0,0,&(t[n2]));
		bn_mul_recursive(r,&(a[n]),&(b[n]),n,0,0,&(t[n2]));
		}

	/* s0 == low(al*bl)
	 * s1 == low(ah*bh)+low((al-ah)*(bh-bl))+low(al*bl)+high(al*bl)
	 * We know s0 and s1 so the only unknown is high(al*bl)
	 * high(al*bl) == s1 - low(ah*bh+s0+(al-ah)*(bh-bl))
	 * high(al*bl) == s1 - (r[0]+l[0]+t[0])
	 */
	if (l != NULL)
		{
		lp= &(t[n2+n]);
		c1=(int)(bn_add_words(lp,&(r[0]),&(l[0]),n));
		}
	else
		{
		c1=0;
		lp= &(r[0]);
		}

	if (neg)
		neg=(int)(bn_sub_words(&(t[n2]),lp,&(t[0]),n));
	else
		{
		bn_add_words(&(t[n2]),lp,&(t[0]),n);
		neg=0;
		}

	if (l != NULL)
		{
		bn_sub_words(&(t[n2+n]),&(l[n]),&(t[n2]),n);
		}
	else
		{
		lp= &(t[n2+n]);
		mp= &(t[n2]);
		for (i=0; i<n; i++)
			lp[i]=((~mp[i])+1)&BN_MASK2;
		}

	/* s[0] = low(al*bl)
	 * t[3] = high(al*bl)
	 * t[10] = (a[0]-a[1])*(b[1]-b[0]) neg is the sign
	 * r[10] = (a[1]*b[1])
	 */
	/* R[10] = al*bl
	 * R[21] = al*bl + ah*bh + (a[0]-a[1])*(b[1]-b[0])
	 * R[32] = ah*bh
	 */
	/* R[1]=t[3]+l[0]+r[0](+-)t[0] (have carry/borrow)
	 * R[2]=r[0]+t[3]+r[1](+-)t[1] (have carry/borrow)
	 * R[3]=r[1]+(carry/borrow)
	 */
	if (l != NULL)
		{
		lp= &(t[n2]);
		c1= (int)(bn_add_words(lp,&(t[n2+n]),&(l[0]),n));
		}
	else
		{
		lp= &(t[n2+n]);
		c1=0;
		}
	c1+=(int)(bn_add_words(&(t[n2]),lp,  &(r[0]),n));
	if (oneg)
		c1-=(int)(bn_sub_words(&(t[n2]),&(t[n2]),&(t[0]),n));
	else
		c1+=(int)(bn_add_words(&(t[n2]),&(t[n2]),&(t[0]),n));

	c2 =(int)(bn_add_words(&(r[0]),&(r[0]),&(t[n2+n]),n));
	c2+=(int)(bn_add_words(&(r[0]),&(r[0]),&(r[n]),n));
	if (oneg)
		c2-=(int)(bn_sub_words(&(r[0]),&(r[0]),&(t[n]),n));
	else
		c2+=(int)(bn_add_words(&(r[0]),&(r[0]),&(t[n]),n));
	
	if (c1 != 0) /* Add starting at r[0], could be +ve or -ve */
		{
		i=0;
		if (c1 > 0)
			{
			lc=c1;
			do	{
				ll=(r[i]+lc)&BN_MASK2;
				r[i++]=ll;
				lc=(lc > ll);
				} while (lc);
			}
		else
			{
			lc= -c1;
			do	{
				ll=r[i];
				r[i++]=(ll-lc)&BN_MASK2;
				lc=(lc > ll);
				} while (lc);
			}
		}
	if (c2 != 0) /* Add starting at r[1] */
		{
		i=n;
		if (c2 > 0)
			{
			lc=c2;
			do	{
				ll=(r[i]+lc)&BN_MASK2;
				r[i++]=ll;
				lc=(lc > ll);
				} while (lc);
			}
		else
			{
			lc= -c2;
			do	{
				ll=r[i];
				r[i++]=(ll-lc)&BN_MASK2;
				lc=(lc > ll);
				} while (lc);
			}
		}
	}