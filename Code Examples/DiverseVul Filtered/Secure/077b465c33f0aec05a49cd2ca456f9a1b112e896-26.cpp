PJ_DEF(void) pj_cis_add_str( pj_cis_t *cis, const char *str)
{
    while (*str) {
        PJ_CIS_SET(cis, *str);
	++str;
    }
}