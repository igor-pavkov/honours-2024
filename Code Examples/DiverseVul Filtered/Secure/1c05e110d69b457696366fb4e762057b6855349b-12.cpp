Status ModularFrameDecoder::DecodeAcMetadata(size_t group_id, BitReader* reader,
                                             PassesDecoderState* dec_state) {
  const Rect r = dec_state->shared->DCGroupRect(group_id);
  size_t upper_bound = r.xsize() * r.ysize();
  reader->Refill();
  size_t count = reader->ReadBits(CeilLog2Nonzero(upper_bound)) + 1;
  size_t stream_id = ModularStreamId::ACMetadata(group_id).ID(frame_dim);
  // YToX, YToB, ACS + QF, EPF
  Image image(r.xsize(), r.ysize(), full_image.bitdepth, 4);
  static_assert(kColorTileDimInBlocks == 8, "Color tile size changed");
  Rect cr(r.x0() >> 3, r.y0() >> 3, (r.xsize() + 7) >> 3, (r.ysize() + 7) >> 3);
  image.channel[0] = Channel(cr.xsize(), cr.ysize(), 3, 3);
  image.channel[1] = Channel(cr.xsize(), cr.ysize(), 3, 3);
  image.channel[2] = Channel(count, 2, 0, 0);
  ModularOptions options;
  if (!ModularGenericDecompress(
          reader, image, /*header=*/nullptr, stream_id, &options,
          /*undo_transforms=*/-1, &tree, &code, &context_map)) {
    return JXL_FAILURE("Failed to decode AC metadata");
  }
  ConvertPlaneAndClamp(Rect(image.channel[0].plane), image.channel[0].plane, cr,
                       &dec_state->shared_storage.cmap.ytox_map);
  ConvertPlaneAndClamp(Rect(image.channel[1].plane), image.channel[1].plane, cr,
                       &dec_state->shared_storage.cmap.ytob_map);
  size_t num = 0;
  bool is444 = dec_state->shared->frame_header.chroma_subsampling.Is444();
  auto& ac_strategy = dec_state->shared_storage.ac_strategy;
  size_t xlim = std::min(ac_strategy.xsize(), r.x0() + r.xsize());
  size_t ylim = std::min(ac_strategy.ysize(), r.y0() + r.ysize());
  uint32_t local_used_acs = 0;
  for (size_t iy = 0; iy < r.ysize(); iy++) {
    size_t y = r.y0() + iy;
    int* row_qf = r.Row(&dec_state->shared_storage.raw_quant_field, iy);
    uint8_t* row_epf = r.Row(&dec_state->shared_storage.epf_sharpness, iy);
    int* row_in_1 = image.channel[2].plane.Row(0);
    int* row_in_2 = image.channel[2].plane.Row(1);
    int* row_in_3 = image.channel[3].plane.Row(iy);
    for (size_t ix = 0; ix < r.xsize(); ix++) {
      size_t x = r.x0() + ix;
      int sharpness = row_in_3[ix];
      if (sharpness < 0 || sharpness >= LoopFilter::kEpfSharpEntries) {
        return JXL_FAILURE("Corrupted sharpness field");
      }
      row_epf[ix] = sharpness;
      if (ac_strategy.IsValid(x, y)) {
        continue;
      }

      if (num >= count) return JXL_FAILURE("Corrupted stream");

      if (!AcStrategy::IsRawStrategyValid(row_in_1[num])) {
        return JXL_FAILURE("Invalid AC strategy");
      }
      local_used_acs |= 1u << row_in_1[num];
      AcStrategy acs = AcStrategy::FromRawStrategy(row_in_1[num]);
      if ((acs.covered_blocks_x() > 1 || acs.covered_blocks_y() > 1) &&
          !is444) {
        return JXL_FAILURE(
            "AC strategy not compatible with chroma subsampling");
      }
      // Ensure that blocks do not overflow *AC* groups.
      size_t next_x_ac_block = (x / kGroupDimInBlocks + 1) * kGroupDimInBlocks;
      size_t next_y_ac_block = (y / kGroupDimInBlocks + 1) * kGroupDimInBlocks;
      size_t next_x_dct_block = x + acs.covered_blocks_x();
      size_t next_y_dct_block = y + acs.covered_blocks_y();
      if (next_x_dct_block > next_x_ac_block || next_x_dct_block > xlim) {
        return JXL_FAILURE("Invalid AC strategy, x overflow");
      }
      if (next_y_dct_block > next_y_ac_block || next_y_dct_block > ylim) {
        return JXL_FAILURE("Invalid AC strategy, y overflow");
      }
      JXL_RETURN_IF_ERROR(
          ac_strategy.SetNoBoundsCheck(x, y, AcStrategy::Type(row_in_1[num])));
      row_qf[ix] =
          1 + std::max(0, std::min(Quantizer::kQuantMax - 1, row_in_2[num]));
      num++;
    }
  }
  dec_state->used_acs |= local_used_acs;
  if (dec_state->shared->frame_header.loop_filter.epf_iters > 0) {
    ComputeSigma(r, dec_state);
  }
  return true;
}