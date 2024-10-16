Status ModularFrameDecoder::DecodeQuantTable(
    size_t required_size_x, size_t required_size_y, BitReader* br,
    QuantEncoding* encoding, size_t idx,
    ModularFrameDecoder* modular_frame_decoder) {
  JXL_RETURN_IF_ERROR(F16Coder::Read(br, &encoding->qraw.qtable_den));
  if (encoding->qraw.qtable_den < kAlmostZero) {
    // qtable[] values are already checked for <= 0 so the denominator may not
    // be negative.
    return JXL_FAILURE("Invalid qtable_den: value too small");
  }
  Image image(required_size_x, required_size_y, 8, 3);
  ModularOptions options;
  if (modular_frame_decoder) {
    JXL_RETURN_IF_ERROR(ModularGenericDecompress(
        br, image, /*header=*/nullptr,
        ModularStreamId::QuantTable(idx).ID(modular_frame_decoder->frame_dim),
        &options, /*undo_transforms=*/-1, &modular_frame_decoder->tree,
        &modular_frame_decoder->code, &modular_frame_decoder->context_map));
  } else {
    JXL_RETURN_IF_ERROR(ModularGenericDecompress(br, image, /*header=*/nullptr,
                                                 0, &options,
                                                 /*undo_transforms=*/-1));
  }
  if (!encoding->qraw.qtable) {
    encoding->qraw.qtable = new std::vector<int>();
  }
  encoding->qraw.qtable->resize(required_size_x * required_size_y * 3);
  for (size_t c = 0; c < 3; c++) {
    for (size_t y = 0; y < required_size_y; y++) {
      int* JXL_RESTRICT row = image.channel[c].Row(y);
      for (size_t x = 0; x < required_size_x; x++) {
        (*encoding->qraw.qtable)[c * required_size_x * required_size_y +
                                 y * required_size_x + x] = row[x];
        if (row[x] <= 0) {
          return JXL_FAILURE("Invalid raw quantization table");
        }
      }
    }
  }
  return true;
}