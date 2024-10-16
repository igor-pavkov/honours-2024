Status ModularFrameDecoder::DecodeGroup(const Rect& rect, BitReader* reader,
                                        int minShift, int maxShift,
                                        const ModularStreamId& stream,
                                        bool zerofill,
                                        PassesDecoderState* dec_state,
                                        ImageBundle* output) {
  JXL_DASSERT(stream.kind == ModularStreamId::kModularDC ||
              stream.kind == ModularStreamId::kModularAC);
  const size_t xsize = rect.xsize();
  const size_t ysize = rect.ysize();
  Image gi(xsize, ysize, full_image.bitdepth, 0);
  // start at the first bigger-than-groupsize non-metachannel
  size_t c = full_image.nb_meta_channels;
  for (; c < full_image.channel.size(); c++) {
    Channel& fc = full_image.channel[c];
    if (fc.w > frame_dim.group_dim || fc.h > frame_dim.group_dim) break;
  }
  size_t beginc = c;
  for (; c < full_image.channel.size(); c++) {
    Channel& fc = full_image.channel[c];
    int shift = std::min(fc.hshift, fc.vshift);
    if (shift > maxShift) continue;
    if (shift < minShift) continue;
    Rect r(rect.x0() >> fc.hshift, rect.y0() >> fc.vshift,
           rect.xsize() >> fc.hshift, rect.ysize() >> fc.vshift, fc.w, fc.h);
    if (r.xsize() == 0 || r.ysize() == 0) continue;
    if (zerofill && use_full_image) {
      for (size_t y = 0; y < r.ysize(); ++y) {
        pixel_type* const JXL_RESTRICT row_out = r.Row(&fc.plane, y);
        memset(row_out, 0, r.xsize() * sizeof(*row_out));
      }
    } else {
      Channel gc(r.xsize(), r.ysize());
      if (zerofill) ZeroFillImage(&gc.plane);
      gc.hshift = fc.hshift;
      gc.vshift = fc.vshift;
      gi.channel.emplace_back(std::move(gc));
    }
  }
  if (zerofill && use_full_image) return true;
  ModularOptions options;
  if (!zerofill) {
    if (!ModularGenericDecompress(
            reader, gi, /*header=*/nullptr, stream.ID(frame_dim), &options,
            /*undo_transforms=*/-1, &tree, &code, &context_map)) {
      return JXL_FAILURE("Failed to decode modular group");
    }
  }
  // Undo global transforms that have been pushed to the group level
  if (!use_full_image) {
    for (auto t : global_transform) {
      JXL_RETURN_IF_ERROR(t.Inverse(gi, global_header.wp_header));
    }
    JXL_RETURN_IF_ERROR(ModularImageToDecodedRect(
        gi, dec_state, nullptr, output, rect.Crop(dec_state->decoded)));
    return true;
  }
  int gic = 0;
  for (c = beginc; c < full_image.channel.size(); c++) {
    Channel& fc = full_image.channel[c];
    int shift = std::min(fc.hshift, fc.vshift);
    if (shift > maxShift) continue;
    if (shift < minShift) continue;
    Rect r(rect.x0() >> fc.hshift, rect.y0() >> fc.vshift,
           rect.xsize() >> fc.hshift, rect.ysize() >> fc.vshift, fc.w, fc.h);
    if (r.xsize() == 0 || r.ysize() == 0) continue;
    JXL_ASSERT(use_full_image);
    CopyImageTo(/*rect_from=*/Rect(0, 0, r.xsize(), r.ysize()),
                /*from=*/gi.channel[gic].plane,
                /*rect_to=*/r, /*to=*/&fc.plane);
    gic++;
  }
  return true;
}