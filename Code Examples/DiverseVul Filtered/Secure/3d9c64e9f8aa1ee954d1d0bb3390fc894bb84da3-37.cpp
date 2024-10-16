static int setDecodeDefaults(struct jpeg_decompress_struct *dinfo,
                             int pixelFormat, int subsamp, int flags)
{
  int i;

  dinfo->scale_num = dinfo->scale_denom = 1;

  if (subsamp == TJSAMP_GRAY) {
    dinfo->num_components = dinfo->comps_in_scan = 1;
    dinfo->jpeg_color_space = JCS_GRAYSCALE;
  } else {
    dinfo->num_components = dinfo->comps_in_scan = 3;
    dinfo->jpeg_color_space = JCS_YCbCr;
  }

  dinfo->comp_info = (jpeg_component_info *)
    (*dinfo->mem->alloc_small) ((j_common_ptr)dinfo, JPOOL_IMAGE,
                                dinfo->num_components *
                                sizeof(jpeg_component_info));

  for (i = 0; i < dinfo->num_components; i++) {
    jpeg_component_info *compptr = &dinfo->comp_info[i];

    compptr->h_samp_factor = (i == 0) ? tjMCUWidth[subsamp] / 8 : 1;
    compptr->v_samp_factor = (i == 0) ? tjMCUHeight[subsamp] / 8 : 1;
    compptr->component_index = i;
    compptr->component_id = i + 1;
    compptr->quant_tbl_no = compptr->dc_tbl_no =
      compptr->ac_tbl_no = (i == 0) ? 0 : 1;
    dinfo->cur_comp_info[i] = compptr;
  }
  dinfo->data_precision = 8;
  for (i = 0; i < 2; i++) {
    if (dinfo->quant_tbl_ptrs[i] == NULL)
      dinfo->quant_tbl_ptrs[i] = jpeg_alloc_quant_table((j_common_ptr)dinfo);
  }

  return 0;
}