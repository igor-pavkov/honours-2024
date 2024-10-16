OPJ_UINT32 opj_tcd_get_decoded_tile_size ( opj_tcd_t *p_tcd )
{
        OPJ_UINT32 i;
        OPJ_UINT32 l_data_size = 0;
        opj_image_comp_t * l_img_comp = 00;
        opj_tcd_tilecomp_t * l_tile_comp = 00;
        opj_tcd_resolution_t * l_res = 00;
        OPJ_UINT32 l_size_comp, l_remaining;

        l_tile_comp = p_tcd->tcd_image->tiles->comps;
        l_img_comp = p_tcd->image->comps;

        for (i=0;i<p_tcd->image->numcomps;++i) {
                l_size_comp = l_img_comp->prec >> 3; /*(/ 8)*/
                l_remaining = l_img_comp->prec & 7;  /* (%8) */

                if(l_remaining) {
                        ++l_size_comp;
                }

                if (l_size_comp == 3) {
                        l_size_comp = 4;
                }

                l_res = l_tile_comp->resolutions + l_tile_comp->minimum_num_resolutions - 1;
                l_data_size += l_size_comp * (OPJ_UINT32)((l_res->x1 - l_res->x0) * (l_res->y1 - l_res->y0));
                ++l_img_comp;
                ++l_tile_comp;
        }

        return l_data_size;
}