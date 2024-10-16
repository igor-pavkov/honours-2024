static void calcstepsizes(uint_fast16_t refstepsize, int numrlvls,
  uint_fast16_t *stepsizes)
{
	int bandno;
	int numbands;
	uint_fast16_t expn;
	uint_fast16_t mant;
	expn = JPC_QCX_GETEXPN(refstepsize);
	mant = JPC_QCX_GETMANT(refstepsize);
	numbands = 3 * numrlvls - 2;
	for (bandno = 0; bandno < numbands; ++bandno) {
//jas_eprintf("DEBUG %d %d %d %d %d\n", bandno, expn, numrlvls, bandno, ((numrlvls - 1) - (numrlvls - 1 - ((bandno > 0) ? ((bandno + 2) / 3) : (0)))));
		stepsizes[bandno] = JPC_QCX_MANT(mant) | JPC_QCX_EXPN(expn +
		  (numrlvls - 1) - (numrlvls - 1 - ((bandno > 0) ? ((bandno + 2) / 3) : (0))));
	}
}