static void decode_mb_rows(VP8D_COMP *pbi) {
  VP8_COMMON *const pc = &pbi->common;
  MACROBLOCKD *const xd = &pbi->mb;

  MODE_INFO *lf_mic = xd->mode_info_context;

  int ibc = 0;
  int num_part = 1 << pc->multi_token_partition;

  int recon_yoffset, recon_uvoffset;
  int mb_row, mb_col;
  int mb_idx = 0;

  YV12_BUFFER_CONFIG *yv12_fb_new = pbi->dec_fb_ref[INTRA_FRAME];

  int recon_y_stride = yv12_fb_new->y_stride;
  int recon_uv_stride = yv12_fb_new->uv_stride;

  unsigned char *ref_buffer[MAX_REF_FRAMES][3];
  unsigned char *dst_buffer[3];
  unsigned char *lf_dst[3];
  unsigned char *eb_dst[3];
  int i;
  int ref_fb_corrupted[MAX_REF_FRAMES];

  ref_fb_corrupted[INTRA_FRAME] = 0;

  for (i = 1; i < MAX_REF_FRAMES; ++i) {
    YV12_BUFFER_CONFIG *this_fb = pbi->dec_fb_ref[i];

    ref_buffer[i][0] = this_fb->y_buffer;
    ref_buffer[i][1] = this_fb->u_buffer;
    ref_buffer[i][2] = this_fb->v_buffer;

    ref_fb_corrupted[i] = this_fb->corrupted;
  }

  /* Set up the buffer pointers */
  eb_dst[0] = lf_dst[0] = dst_buffer[0] = yv12_fb_new->y_buffer;
  eb_dst[1] = lf_dst[1] = dst_buffer[1] = yv12_fb_new->u_buffer;
  eb_dst[2] = lf_dst[2] = dst_buffer[2] = yv12_fb_new->v_buffer;

  xd->up_available = 0;

  /* Initialize the loop filter for this frame. */
  if (pc->filter_level) vp8_loop_filter_frame_init(pc, xd, pc->filter_level);

  vp8_setup_intra_recon_top_line(yv12_fb_new);

  /* Decode the individual macro block */
  for (mb_row = 0; mb_row < pc->mb_rows; ++mb_row) {
    if (num_part > 1) {
      xd->current_bc = &pbi->mbc[ibc];
      ibc++;

      if (ibc == num_part) ibc = 0;
    }

    recon_yoffset = mb_row * recon_y_stride * 16;
    recon_uvoffset = mb_row * recon_uv_stride * 8;

    /* reset contexts */
    xd->above_context = pc->above_context;
    memset(xd->left_context, 0, sizeof(ENTROPY_CONTEXT_PLANES));

    xd->left_available = 0;

    xd->mb_to_top_edge = -((mb_row * 16) << 3);
    xd->mb_to_bottom_edge = ((pc->mb_rows - 1 - mb_row) * 16) << 3;

    xd->recon_above[0] = dst_buffer[0] + recon_yoffset;
    xd->recon_above[1] = dst_buffer[1] + recon_uvoffset;
    xd->recon_above[2] = dst_buffer[2] + recon_uvoffset;

    xd->recon_left[0] = xd->recon_above[0] - 1;
    xd->recon_left[1] = xd->recon_above[1] - 1;
    xd->recon_left[2] = xd->recon_above[2] - 1;

    xd->recon_above[0] -= xd->dst.y_stride;
    xd->recon_above[1] -= xd->dst.uv_stride;
    xd->recon_above[2] -= xd->dst.uv_stride;

    /* TODO: move to outside row loop */
    xd->recon_left_stride[0] = xd->dst.y_stride;
    xd->recon_left_stride[1] = xd->dst.uv_stride;

    setup_intra_recon_left(xd->recon_left[0], xd->recon_left[1],
                           xd->recon_left[2], xd->dst.y_stride,
                           xd->dst.uv_stride);

    for (mb_col = 0; mb_col < pc->mb_cols; ++mb_col) {
      /* Distance of Mb to the various image edges.
       * These are specified to 8th pel as they are always compared to values
       * that are in 1/8th pel units
       */
      xd->mb_to_left_edge = -((mb_col * 16) << 3);
      xd->mb_to_right_edge = ((pc->mb_cols - 1 - mb_col) * 16) << 3;

#if CONFIG_ERROR_CONCEALMENT
      {
        int corrupt_residual =
            (!pbi->independent_partitions && pbi->frame_corrupt_residual) ||
            vp8dx_bool_error(xd->current_bc);
        if (pbi->ec_active &&
            xd->mode_info_context->mbmi.ref_frame == INTRA_FRAME &&
            corrupt_residual) {
          /* We have an intra block with corrupt coefficients, better to
           * conceal with an inter block. Interpolate MVs from neighboring
           * MBs.
           *
           * Note that for the first mb with corrupt residual in a frame,
           * we might not discover that before decoding the residual. That
           * happens after this check, and therefore no inter concealment
           * will be done.
           */
          vp8_interpolate_motion(xd, mb_row, mb_col, pc->mb_rows, pc->mb_cols);
        }
      }
#endif

      xd->dst.y_buffer = dst_buffer[0] + recon_yoffset;
      xd->dst.u_buffer = dst_buffer[1] + recon_uvoffset;
      xd->dst.v_buffer = dst_buffer[2] + recon_uvoffset;

      if (xd->mode_info_context->mbmi.ref_frame >= LAST_FRAME) {
        const MV_REFERENCE_FRAME ref = xd->mode_info_context->mbmi.ref_frame;
        xd->pre.y_buffer = ref_buffer[ref][0] + recon_yoffset;
        xd->pre.u_buffer = ref_buffer[ref][1] + recon_uvoffset;
        xd->pre.v_buffer = ref_buffer[ref][2] + recon_uvoffset;
      } else {
        // ref_frame is INTRA_FRAME, pre buffer should not be used.
        xd->pre.y_buffer = 0;
        xd->pre.u_buffer = 0;
        xd->pre.v_buffer = 0;
      }

      /* propagate errors from reference frames */
      xd->corrupted |= ref_fb_corrupted[xd->mode_info_context->mbmi.ref_frame];

      decode_macroblock(pbi, xd, mb_idx);

      mb_idx++;
      xd->left_available = 1;

      /* check if the boolean decoder has suffered an error */
      xd->corrupted |= vp8dx_bool_error(xd->current_bc);

      xd->recon_above[0] += 16;
      xd->recon_above[1] += 8;
      xd->recon_above[2] += 8;
      xd->recon_left[0] += 16;
      xd->recon_left[1] += 8;
      xd->recon_left[2] += 8;

      recon_yoffset += 16;
      recon_uvoffset += 8;

      ++xd->mode_info_context; /* next mb */

      xd->above_context++;
    }

    /* adjust to the next row of mbs */
    vp8_extend_mb_row(yv12_fb_new, xd->dst.y_buffer + 16, xd->dst.u_buffer + 8,
                      xd->dst.v_buffer + 8);

    ++xd->mode_info_context; /* skip prediction column */
    xd->up_available = 1;

    if (pc->filter_level) {
      if (mb_row > 0) {
        if (pc->filter_type == NORMAL_LOOPFILTER) {
          vp8_loop_filter_row_normal(pc, lf_mic, mb_row - 1, recon_y_stride,
                                     recon_uv_stride, lf_dst[0], lf_dst[1],
                                     lf_dst[2]);
        } else {
          vp8_loop_filter_row_simple(pc, lf_mic, mb_row - 1, recon_y_stride,
                                     recon_uv_stride, lf_dst[0], lf_dst[1],
                                     lf_dst[2]);
        }
        if (mb_row > 1) {
          yv12_extend_frame_left_right_c(yv12_fb_new, eb_dst[0], eb_dst[1],
                                         eb_dst[2]);

          eb_dst[0] += recon_y_stride * 16;
          eb_dst[1] += recon_uv_stride * 8;
          eb_dst[2] += recon_uv_stride * 8;
        }

        lf_dst[0] += recon_y_stride * 16;
        lf_dst[1] += recon_uv_stride * 8;
        lf_dst[2] += recon_uv_stride * 8;
        lf_mic += pc->mb_cols;
        lf_mic++; /* Skip border mb */
      }
    } else {
      if (mb_row > 0) {
        /**/
        yv12_extend_frame_left_right_c(yv12_fb_new, eb_dst[0], eb_dst[1],
                                       eb_dst[2]);
        eb_dst[0] += recon_y_stride * 16;
        eb_dst[1] += recon_uv_stride * 8;
        eb_dst[2] += recon_uv_stride * 8;
      }
    }
  }

  if (pc->filter_level) {
    if (pc->filter_type == NORMAL_LOOPFILTER) {
      vp8_loop_filter_row_normal(pc, lf_mic, mb_row - 1, recon_y_stride,
                                 recon_uv_stride, lf_dst[0], lf_dst[1],
                                 lf_dst[2]);
    } else {
      vp8_loop_filter_row_simple(pc, lf_mic, mb_row - 1, recon_y_stride,
                                 recon_uv_stride, lf_dst[0], lf_dst[1],
                                 lf_dst[2]);
    }

    yv12_extend_frame_left_right_c(yv12_fb_new, eb_dst[0], eb_dst[1],
                                   eb_dst[2]);
    eb_dst[0] += recon_y_stride * 16;
    eb_dst[1] += recon_uv_stride * 8;
    eb_dst[2] += recon_uv_stride * 8;
  }
  yv12_extend_frame_left_right_c(yv12_fb_new, eb_dst[0], eb_dst[1], eb_dst[2]);
  yv12_extend_frame_top_c(yv12_fb_new);
  yv12_extend_frame_bottom_c(yv12_fb_new);
}