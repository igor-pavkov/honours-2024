Status ModularFrameDecoder::DecodeVarDCTDC(size_t group_id, BitReader* reader,
                                           PassesDecoderState* dec_state) {
  const Rect r = dec_state->shared->DCGroupRect(group_id);
  // TODO(eustas): investigate if we could reduce the impact of
  //               EvalRationalPolynomial; generally speaking, the limit is
  //               2**(128/(3*magic)), where 128 comes from IEEE 754 exponent,
  //               3 comes from XybToRgb that cubes the values, and "magic" is
  //               the sum of all other contributions. 2**18 is known to lead
  //               to NaN on input found by fuzzing (see commit message).
  Image image(r.xsize(), r.ysize(), full_image.bitdepth, 3);
  size_t stream_id = ModularStreamId::VarDCTDC(group_id).ID(frame_dim);
  reader->Refill();
  size_t extra_precision = reader->ReadFixedBits<2>();
  float mul = 1.0f / (1 << extra_precision);
  ModularOptions options;
  for (size_t c = 0; c < 3; c++) {
    Channel& ch = image.channel[c < 2 ? c ^ 1 : c];
    ch.w >>= dec_state->shared->frame_header.chroma_subsampling.HShift(c);
    ch.h >>= dec_state->shared->frame_header.chroma_subsampling.VShift(c);
    ch.shrink();
  }
  if (!ModularGenericDecompress(
          reader, image, /*header=*/nullptr, stream_id, &options,
          /*undo_transforms=*/-1, &tree, &code, &context_map)) {
    return JXL_FAILURE("Failed to decode modular DC group");
  }
  DequantDC(r, &dec_state->shared_storage.dc_storage,
            &dec_state->shared_storage.quant_dc, image,
            dec_state->shared->quantizer.MulDC(), mul,
            dec_state->shared->cmap.DCFactors(),
            dec_state->shared->frame_header.chroma_subsampling,
            dec_state->shared->block_ctx_map);
  return true;
}