PJ_DEF(void) pj_cis_add_range(pj_cis_t *cis, int cstart, int cend)
{
    /* Can not set zero. This is the requirement of the parser. */
    pj_assert(cstart > 0);

    while (cstart != cend) {
        PJ_CIS_SET(cis, cstart);
	++cstart;
    }
}