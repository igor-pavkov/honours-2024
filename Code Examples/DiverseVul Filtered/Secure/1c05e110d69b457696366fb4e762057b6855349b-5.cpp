Status ModularFrameDecoder::ModularImageToDecodedRect(
    Image& gi, PassesDecoderState* dec_state, jxl::ThreadPool* pool,
    ImageBundle* output, Rect rect) {
  auto& decoded = dec_state->decoded;
  const auto& frame_header = dec_state->shared->frame_header;
  const auto* metadata = frame_header.nonserialized_metadata;
  size_t xsize = rect.xsize();
  size_t ysize = rect.ysize();
  if (!xsize || !ysize) {
    return true;
  }
  JXL_DASSERT(rect.IsInside(decoded));

  int c = 0;
  if (do_color) {
    const bool rgb_from_gray =
        metadata->m.color_encoding.IsGray() &&
        frame_header.color_transform == ColorTransform::kNone;
    const bool fp = metadata->m.bit_depth.floating_point_sample;

    for (; c < 3; c++) {
      float factor = full_image.bitdepth < 32
                         ? 1.f / ((1u << full_image.bitdepth) - 1)
                         : 0;
      int c_in = c;
      if (frame_header.color_transform == ColorTransform::kXYB) {
        factor = dec_state->shared->matrices.DCQuants()[c];
        // XYB is encoded as YX(B-Y)
        if (c < 2) c_in = 1 - c;
      } else if (rgb_from_gray) {
        c_in = 0;
      }
      Channel& ch_in = gi.channel[c_in];
      // TODO(eustas): could we detect it on earlier stage?
      if (ch_in.w == 0 || ch_in.h == 0) {
        return JXL_FAILURE("Empty image");
      }
      size_t xsize_shifted = DivCeil(xsize, 1 << ch_in.hshift);
      size_t ysize_shifted = DivCeil(ysize, 1 << ch_in.vshift);
      Rect r(rect.x0() >> ch_in.hshift, rect.y0() >> ch_in.vshift,
             rect.xsize() >> ch_in.hshift, rect.ysize() >> ch_in.vshift,
             DivCeil(decoded.xsize(), 1 << ch_in.hshift),
             DivCeil(decoded.ysize(), 1 << ch_in.vshift));
      if (r.ysize() != ch_in.h || r.xsize() != ch_in.w) {
        return JXL_FAILURE(
            "Dimension mismatch: trying to fit a %zux%zu modular channel into "
            "a %zux%zu rect",
            ch_in.w, ch_in.h, r.xsize(), r.ysize());
      }
      if (frame_header.color_transform == ColorTransform::kXYB && c == 2) {
        JXL_ASSERT(!fp);
        RunOnPool(
            pool, 0, ysize_shifted, jxl::ThreadPool::SkipInit(),
            [&](const int task, const int thread) {
              const size_t y = task;
              const pixel_type* const JXL_RESTRICT row_in = ch_in.Row(y);
              const pixel_type* const JXL_RESTRICT row_in_Y =
                  gi.channel[0].Row(y);
              float* const JXL_RESTRICT row_out = r.PlaneRow(&decoded, c, y);
              HWY_DYNAMIC_DISPATCH(MultiplySum)
              (xsize_shifted, row_in, row_in_Y, factor, row_out);
            },
            "ModularIntToFloat");
      } else if (fp) {
        int bits = metadata->m.bit_depth.bits_per_sample;
        int exp_bits = metadata->m.bit_depth.exponent_bits_per_sample;
        RunOnPool(
            pool, 0, ysize_shifted, jxl::ThreadPool::SkipInit(),
            [&](const int task, const int thread) {
              const size_t y = task;
              const pixel_type* const JXL_RESTRICT row_in = ch_in.Row(y);
              float* const JXL_RESTRICT row_out = r.PlaneRow(&decoded, c, y);
              int_to_float(row_in, row_out, xsize_shifted, bits, exp_bits);
            },
            "ModularIntToFloat_losslessfloat");
      } else {
        RunOnPool(
            pool, 0, ysize_shifted, jxl::ThreadPool::SkipInit(),
            [&](const int task, const int thread) {
              const size_t y = task;
              const pixel_type* const JXL_RESTRICT row_in = ch_in.Row(y);
              if (rgb_from_gray) {
                HWY_DYNAMIC_DISPATCH(RgbFromSingle)
                (xsize_shifted, row_in, factor, &decoded, c, y, r);
              } else {
                HWY_DYNAMIC_DISPATCH(SingleFromSingle)
                (xsize_shifted, row_in, factor, &decoded, c, y, r);
              }
            },
            "ModularIntToFloat");
      }
      if (rgb_from_gray) {
        break;
      }
    }
    if (rgb_from_gray) {
      c = 1;
    }
  }
  for (size_t ec = 0; ec < dec_state->extra_channels.size(); ec++, c++) {
    const ExtraChannelInfo& eci = output->metadata()->extra_channel_info[ec];
    int bits = eci.bit_depth.bits_per_sample;
    int exp_bits = eci.bit_depth.exponent_bits_per_sample;
    bool fp = eci.bit_depth.floating_point_sample;
    JXL_ASSERT(fp || bits < 32);
    const float mul = fp ? 0 : (1.0f / ((1u << bits) - 1));
    size_t ecups = frame_header.extra_channel_upsampling[ec];
    const size_t ec_xsize = DivCeil(frame_dim.xsize_upsampled, ecups);
    const size_t ec_ysize = DivCeil(frame_dim.ysize_upsampled, ecups);
    Channel& ch_in = gi.channel[c];
    // For x0, y0 there's no need to do a DivCeil().
    JXL_DASSERT(rect.x0() % (1ul << ch_in.hshift) == 0);
    JXL_DASSERT(rect.y0() % (1ul << ch_in.vshift) == 0);
    Rect r(rect.x0() >> ch_in.hshift, rect.y0() >> ch_in.vshift,
           DivCeil(rect.xsize(), 1lu << ch_in.hshift),
           DivCeil(rect.ysize(), 1lu << ch_in.vshift), ec_xsize, ec_ysize);

    JXL_DASSERT(r.IsInside(dec_state->extra_channels[ec]));
    JXL_DASSERT(Rect(0, 0, r.xsize(), r.ysize()).IsInside(ch_in.plane));
    for (size_t y = 0; y < r.ysize(); ++y) {
      float* const JXL_RESTRICT row_out =
          r.Row(&dec_state->extra_channels[ec], y);
      const pixel_type* const JXL_RESTRICT row_in = ch_in.Row(y);
      if (fp) {
        int_to_float(row_in, row_out, r.xsize(), bits, exp_bits);
      } else {
        for (size_t x = 0; x < r.xsize(); ++x) {
          row_out[x] = row_in[x] * mul;
        }
      }
    }
    JXL_CHECK_IMAGE_INITIALIZED(dec_state->extra_channels[ec], r);
  }
  return true;
}