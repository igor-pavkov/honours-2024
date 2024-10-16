Status ModularFrameDecoder::DecodeGlobalInfo(BitReader* reader,
                                             const FrameHeader& frame_header,
                                             bool allow_truncated_group) {
  bool decode_color = frame_header.encoding == FrameEncoding::kModular;
  const auto& metadata = frame_header.nonserialized_metadata->m;
  bool is_gray = metadata.color_encoding.IsGray();
  size_t nb_chans = 3;
  if (is_gray && frame_header.color_transform == ColorTransform::kNone) {
    nb_chans = 1;
  }
  bool has_tree = reader->ReadBits(1);
  if (has_tree) {
    size_t tree_size_limit =
        1024 + frame_dim.xsize * frame_dim.ysize * nb_chans;
    JXL_RETURN_IF_ERROR(DecodeTree(reader, &tree, tree_size_limit));
    JXL_RETURN_IF_ERROR(
        DecodeHistograms(reader, (tree.size() + 1) / 2, &code, &context_map));
  }
  do_color = decode_color;
  if (!do_color) nb_chans = 0;
  size_t nb_extra = metadata.extra_channel_info.size();

  bool fp = metadata.bit_depth.floating_point_sample;

  // bits_per_sample is just metadata for XYB images.
  if (metadata.bit_depth.bits_per_sample >= 32 && do_color &&
      frame_header.color_transform != ColorTransform::kXYB) {
    if (metadata.bit_depth.bits_per_sample == 32 && fp == false) {
      return JXL_FAILURE("uint32_t not supported in dec_modular");
    } else if (metadata.bit_depth.bits_per_sample > 32) {
      return JXL_FAILURE("bits_per_sample > 32 not supported");
    }
  }

  Image gi(frame_dim.xsize, frame_dim.ysize, metadata.bit_depth.bits_per_sample,
           nb_chans + nb_extra);

  if (frame_header.color_transform == ColorTransform::kYCbCr) {
    for (size_t c = 0; c < nb_chans; c++) {
      gi.channel[c].hshift = frame_header.chroma_subsampling.HShift(c);
      gi.channel[c].vshift = frame_header.chroma_subsampling.VShift(c);
      size_t xsize_shifted =
          DivCeil(frame_dim.xsize, 1 << gi.channel[c].hshift);
      size_t ysize_shifted =
          DivCeil(frame_dim.ysize, 1 << gi.channel[c].vshift);
      gi.channel[c].shrink(xsize_shifted, ysize_shifted);
    }
  }

  for (size_t ec = 0, c = nb_chans; ec < nb_extra; ec++, c++) {
    size_t ecups = frame_header.extra_channel_upsampling[ec];
    gi.channel[c].shrink(DivCeil(frame_dim.xsize_upsampled, ecups),
                         DivCeil(frame_dim.ysize_upsampled, ecups));
    gi.channel[c].hshift = gi.channel[c].vshift =
        CeilLog2Nonzero(ecups) - CeilLog2Nonzero(frame_header.upsampling);
  }

  ModularOptions options;
  options.max_chan_size = frame_dim.group_dim;
  options.group_dim = frame_dim.group_dim;
  Status dec_status = ModularGenericDecompress(
      reader, gi, &global_header, ModularStreamId::Global().ID(frame_dim),
      &options,
      /*undo_transforms=*/-2, &tree, &code, &context_map,
      allow_truncated_group);
  if (!allow_truncated_group) JXL_RETURN_IF_ERROR(dec_status);
  if (dec_status.IsFatalError()) {
    return JXL_FAILURE("Failed to decode global modular info");
  }

  // TODO(eustas): are we sure this can be done after partial decode?
  have_something = false;
  for (size_t c = 0; c < gi.channel.size(); c++) {
    Channel& gic = gi.channel[c];
    if (c >= gi.nb_meta_channels && gic.w <= frame_dim.group_dim &&
        gic.h <= frame_dim.group_dim)
      have_something = true;
  }
  // move global transforms to groups if possible
  if (!have_something) {
    if (gi.transform.size() == 1 && gi.transform[0].id == TransformId::kRCT) {
      global_transform = gi.transform;
      gi.transform.clear();
      // TODO(jon): also move no-delta-palette out (trickier though)
    }
  }
  full_image = std::move(gi);
  return dec_status;
}