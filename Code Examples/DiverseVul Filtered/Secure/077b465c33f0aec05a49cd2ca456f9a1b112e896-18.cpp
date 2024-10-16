PJ_DEF(void) pj_cis_del_str( pj_cis_t *cis, const char *str)
{
    while (*str) {
        PJ_CIS_CLR(cis, *str);
	++str;
    }
}