PJ_DEF(void) pj_cis_add_cis( pj_cis_t *cis, const pj_cis_t *rhs)
{
    int i;
    for (i=0; i<256; ++i) {
	if (PJ_CIS_ISSET(rhs, i))
	    PJ_CIS_SET(cis, i);
    }
}