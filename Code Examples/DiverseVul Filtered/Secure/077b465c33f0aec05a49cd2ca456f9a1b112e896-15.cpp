PJ_DEF(void) pj_cis_del_range( pj_cis_t *cis, int cstart, int cend)
{
    while (cstart != cend) {
        PJ_CIS_CLR(cis, cstart);
        cstart++;
    }
}