void vp8_mb_init_dequantizer(VP8D_COMP *pbi, MACROBLOCKD *xd) {
  int i;
  int QIndex;
  MB_MODE_INFO *mbmi = &xd->mode_info_context->mbmi;
  VP8_COMMON *const pc = &pbi->common;

  /* Decide whether to use the default or alternate baseline Q value. */
  if (xd->segmentation_enabled) {
    /* Abs Value */
    if (xd->mb_segement_abs_delta == SEGMENT_ABSDATA) {
      QIndex = xd->segment_feature_data[MB_LVL_ALT_Q][mbmi->segment_id];

      /* Delta Value */
    } else {
      QIndex = pc->base_qindex +
               xd->segment_feature_data[MB_LVL_ALT_Q][mbmi->segment_id];
    }

    QIndex = (QIndex >= 0) ? ((QIndex <= MAXQ) ? QIndex : MAXQ)
                           : 0; /* Clamp to valid range */
  } else {
    QIndex = pc->base_qindex;
  }

  /* Set up the macroblock dequant constants */
  xd->dequant_y1_dc[0] = 1;
  xd->dequant_y1[0] = pc->Y1dequant[QIndex][0];
  xd->dequant_y2[0] = pc->Y2dequant[QIndex][0];
  xd->dequant_uv[0] = pc->UVdequant[QIndex][0];

  for (i = 1; i < 16; ++i) {
    xd->dequant_y1_dc[i] = xd->dequant_y1[i] = pc->Y1dequant[QIndex][1];
    xd->dequant_y2[i] = pc->Y2dequant[QIndex][1];
    xd->dequant_uv[i] = pc->UVdequant[QIndex][1];
  }
}