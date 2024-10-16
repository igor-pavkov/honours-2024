static OPJ_BOOL opj_tcd_t1_decode ( opj_tcd_t *p_tcd )
{
        OPJ_UINT32 compno;
        opj_t1_t * l_t1;
        opj_tcd_tile_t * l_tile = p_tcd->tcd_image->tiles;
        opj_tcd_tilecomp_t* l_tile_comp = l_tile->comps;
        opj_tccp_t * l_tccp = p_tcd->tcp->tccps;


        l_t1 = opj_t1_create(OPJ_FALSE);
        if (l_t1 == 00) {
                return OPJ_FALSE;
        }

        for (compno = 0; compno < l_tile->numcomps; ++compno) {
                /* The +3 is headroom required by the vectorized DWT */
                if (OPJ_FALSE == opj_t1_decode_cblks(l_t1, l_tile_comp, l_tccp)) {
                        opj_t1_destroy(l_t1);
                        return OPJ_FALSE;
                }
                ++l_tile_comp;
                ++l_tccp;
        }

        opj_t1_destroy(l_t1);

        return OPJ_TRUE;
}