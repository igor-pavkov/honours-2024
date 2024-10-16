static int jpc_dec_cp_prepare(jpc_dec_cp_t *cp)
{
	jpc_dec_ccp_t *ccp;
	int compno;
	int i;
	for (compno = 0, ccp = cp->ccps; compno < cp->numcomps;
	  ++compno, ++ccp) {
		if (!(ccp->csty & JPC_COX_PRT)) {
			for (i = 0; i < JPC_MAXRLVLS; ++i) {
				ccp->prcwidthexpns[i] = 15;
				ccp->prcheightexpns[i] = 15;
			}
		}
		if (ccp->qsty == JPC_QCX_SIQNT) {
			if (calcstepsizes(ccp->stepsizes[0], ccp->numrlvls, ccp->stepsizes)) {
				return -1;
			}
		}
	}
	return 0;
}