static double ratio (Bigint *a, Bigint *b)
{
	_double da, db;
	int k, ka, kb;

	value(da) = b2d(a, &ka);
	value(db) = b2d(b, &kb);
#ifdef Pack_32
	k = ka - kb + 32*(a->wds - b->wds);
#else
	k = ka - kb + 16*(a->wds - b->wds);
#endif
#ifdef IBM
	if (k > 0) {
		word0(da) += (k >> 2)*Exp_msk1;
		if (k &= 3) {
			da *= 1 << k;
		}
	} else {
		k = -k;
		word0(db) += (k >> 2)*Exp_msk1;
		if (k &= 3)
			db *= 1 << k;
	}
#else
	if (k > 0) {
		word0(da) += k*Exp_msk1;
	} else {
		k = -k;
		word0(db) += k*Exp_msk1;
	}
#endif
	return value(da) / value(db);
}