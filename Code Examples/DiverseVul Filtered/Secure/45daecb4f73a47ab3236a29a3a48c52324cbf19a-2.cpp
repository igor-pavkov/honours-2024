void vp8cx_init_de_quantizer(VP8D_COMP *pbi) {
  int Q;
  VP8_COMMON *const pc = &pbi->common;

  for (Q = 0; Q < QINDEX_RANGE; ++Q) {
    pc->Y1dequant[Q][0] = (short)vp8_dc_quant(Q, pc->y1dc_delta_q);
    pc->Y2dequant[Q][0] = (short)vp8_dc2quant(Q, pc->y2dc_delta_q);
    pc->UVdequant[Q][0] = (short)vp8_dc_uv_quant(Q, pc->uvdc_delta_q);

    pc->Y1dequant[Q][1] = (short)vp8_ac_yquant(Q);
    pc->Y2dequant[Q][1] = (short)vp8_ac2quant(Q, pc->y2ac_delta_q);
    pc->UVdequant[Q][1] = (short)vp8_ac_uv_quant(Q, pc->uvac_delta_q);
  }
}