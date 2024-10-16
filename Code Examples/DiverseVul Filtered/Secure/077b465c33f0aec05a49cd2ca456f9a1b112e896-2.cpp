PJ_DEF(void) pj_cis_add_num(pj_cis_t *cis)
{
    pj_cis_add_range( cis, '0', '9'+1);
}