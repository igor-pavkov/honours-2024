static void init_frame(VP8D_COMP *pbi) {
  VP8_COMMON *const pc = &pbi->common;
  MACROBLOCKD *const xd = &pbi->mb;

  if (pc->frame_type == KEY_FRAME) {
    /* Various keyframe initializations */
    memcpy(pc->fc.mvc, vp8_default_mv_context, sizeof(vp8_default_mv_context));

    vp8_init_mbmode_probs(pc);

    vp8_default_coef_probs(pc);

    /* reset the segment feature data to 0 with delta coding (Default state). */
    memset(xd->segment_feature_data, 0, sizeof(xd->segment_feature_data));
    xd->mb_segement_abs_delta = SEGMENT_DELTADATA;

    /* reset the mode ref deltasa for loop filter */
    memset(xd->ref_lf_deltas, 0, sizeof(xd->ref_lf_deltas));
    memset(xd->mode_lf_deltas, 0, sizeof(xd->mode_lf_deltas));

    /* All buffers are implicitly updated on key frames. */
    pc->refresh_golden_frame = 1;
    pc->refresh_alt_ref_frame = 1;
    pc->copy_buffer_to_gf = 0;
    pc->copy_buffer_to_arf = 0;

    /* Note that Golden and Altref modes cannot be used on a key frame so
     * ref_frame_sign_bias[] is undefined and meaningless
     */
    pc->ref_frame_sign_bias[GOLDEN_FRAME] = 0;
    pc->ref_frame_sign_bias[ALTREF_FRAME] = 0;
  } else {
    /* To enable choice of different interploation filters */
    if (!pc->use_bilinear_mc_filter) {
      xd->subpixel_predict = vp8_sixtap_predict4x4;
      xd->subpixel_predict8x4 = vp8_sixtap_predict8x4;
      xd->subpixel_predict8x8 = vp8_sixtap_predict8x8;
      xd->subpixel_predict16x16 = vp8_sixtap_predict16x16;
    } else {
      xd->subpixel_predict = vp8_bilinear_predict4x4;
      xd->subpixel_predict8x4 = vp8_bilinear_predict8x4;
      xd->subpixel_predict8x8 = vp8_bilinear_predict8x8;
      xd->subpixel_predict16x16 = vp8_bilinear_predict16x16;
    }

    if (pbi->decoded_key_frame && pbi->ec_enabled && !pbi->ec_active) {
      pbi->ec_active = 1;
    }
  }

  xd->left_context = &pc->left_context;
  xd->mode_info_context = pc->mi;
  xd->frame_type = pc->frame_type;
  xd->mode_info_context->mbmi.mode = DC_PRED;
  xd->mode_info_stride = pc->mode_info_stride;
  xd->corrupted = 0; /* init without corruption */

  xd->fullpixel_mask = 0xffffffff;
  if (pc->full_pixel) xd->fullpixel_mask = 0xfffffff8;
}