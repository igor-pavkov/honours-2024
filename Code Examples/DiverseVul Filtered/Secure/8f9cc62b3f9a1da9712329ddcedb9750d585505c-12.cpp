static OPJ_BOOL opj_tcd_code_block_dec_allocate (opj_tcd_cblk_dec_t * p_code_block)
{
        if (! p_code_block->data) {

                p_code_block->data = (OPJ_BYTE*) opj_malloc(OPJ_J2K_DEFAULT_CBLK_DATA_SIZE);
                if (! p_code_block->data) {
                        return OPJ_FALSE;
                }
                p_code_block->data_max_size = OPJ_J2K_DEFAULT_CBLK_DATA_SIZE;
                /*fprintf(stderr, "Allocate 8192 elements of code_block->data\n");*/

                p_code_block->segs = (opj_tcd_seg_t *) opj_calloc(OPJ_J2K_DEFAULT_NB_SEGS,sizeof(opj_tcd_seg_t));
                if (! p_code_block->segs) {
                        return OPJ_FALSE;
                }
                /*fprintf(stderr, "Allocate %d elements of code_block->data\n", OPJ_J2K_DEFAULT_NB_SEGS * sizeof(opj_tcd_seg_t));*/

                p_code_block->m_current_max_segs = OPJ_J2K_DEFAULT_NB_SEGS;
                /*fprintf(stderr, "m_current_max_segs of code_block->data = %d\n", p_code_block->m_current_max_segs);*/
        } else {
					/* sanitize */
					OPJ_BYTE* l_data = p_code_block->data;
					OPJ_UINT32 l_data_max_size = p_code_block->data_max_size;
					opj_tcd_seg_t * l_segs = p_code_block->segs;
					OPJ_UINT32 l_current_max_segs = p_code_block->m_current_max_segs;

					memset(p_code_block, 0, sizeof(opj_tcd_cblk_dec_t));
					p_code_block->data = l_data;
					p_code_block->data_max_size = l_data_max_size;
					p_code_block->segs = l_segs;
					p_code_block->m_current_max_segs = l_current_max_segs;
				}

        return OPJ_TRUE;
}