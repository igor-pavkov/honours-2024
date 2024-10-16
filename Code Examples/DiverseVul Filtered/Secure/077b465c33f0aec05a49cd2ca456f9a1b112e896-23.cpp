PJ_DEF(void) pj_cis_add_alpha(pj_cis_t *cis)
{
    pj_cis_add_range( cis, 'a', 'z'+1);
    pj_cis_add_range( cis, 'A', 'Z'+1);
}