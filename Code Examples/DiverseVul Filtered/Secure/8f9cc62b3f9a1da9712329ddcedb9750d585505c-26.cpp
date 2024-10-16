static OPJ_BOOL opj_tcd_dwt_decode ( opj_tcd_t *p_tcd )
{
        OPJ_UINT32 compno;
        opj_tcd_tile_t * l_tile = p_tcd->tcd_image->tiles;
        opj_tcd_tilecomp_t * l_tile_comp = l_tile->comps;
        opj_tccp_t * l_tccp = p_tcd->tcp->tccps;
        opj_image_comp_t * l_img_comp = p_tcd->image->comps;

        for (compno = 0; compno < l_tile->numcomps; compno++) {
                /*
                if (tcd->cp->reduce != 0) {
                        tcd->image->comps[compno].resno_decoded =
                                tile->comps[compno].numresolutions - tcd->cp->reduce - 1;
                        if (tcd->image->comps[compno].resno_decoded < 0)
                        {
                                return false;
                        }
                }
                numres2decode = tcd->image->comps[compno].resno_decoded + 1;
                if(numres2decode > 0){
                */

                if (l_tccp->qmfbid == 1) {
                        if (! opj_dwt_decode(l_tile_comp, l_img_comp->resno_decoded+1)) {
                                return OPJ_FALSE;
                        }
                }
                else {
                        if (! opj_dwt_decode_real(l_tile_comp, l_img_comp->resno_decoded+1)) {
                                return OPJ_FALSE;
                        }
                }

                ++l_tile_comp;
                ++l_img_comp;
                ++l_tccp;
        }

        return OPJ_TRUE;
}