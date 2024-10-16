inline void GEPP(
    const std::vector<SparseSlice<TL>*>& left_slices,
    const Eigen::TensorMap<Eigen::Tensor<const TR, 2, Eigen::RowMajor>,
                           Eigen::Aligned>& right,
    const int num_cols, Matrix* output) {
  const int cols = (Cols == -1) ? num_cols : Cols;
  DCHECK_EQ(num_cols, cols);
  const int right_num_cols = right.dimension(1);
  const int output_num_cols = output->dimension(1);
  static const int kNumOperandsR = kNumOperands * sizeof(float) / sizeof(TR);
  const int cols_mod = cols % kNumOperandsR;
  int k_offset = 0;
  // Pre-compute pointers for output matrix.
  float* out_ptrs[M];
  float* const out_start = &(*output)(0, 0);
  for (int j = 0; j < M; ++j) {
    out_ptrs[j] = out_start + output_num_cols * j;
  }
  for (const auto* left_slice : left_slices) {
    const auto& left = *left_slice;
    const auto* data3 = (!left.data3.empty()) ? &left.data3[0] : nullptr;
    const auto* data = (!left.data.empty()) ? &left.data[0] : nullptr;
    const int num_blocks = left.index3_offset.size();
    int begin3 = 0;
    int begin = 0;
    for (int i = 0; i < num_blocks; ++i) {
      // Pre-compute pointers for right matrix
      const TR* right_ptrs[K];
      const auto* const right_start = &right(k_offset, 0);
      DCHECK_LT(k_offset, right.dimension(0));
      for (int j = 0; j < K; ++j) {
        right_ptrs[j] = right_start + right_num_cols * j;
      }

      const int end3 = left.index3_offset[i];
      int j = begin3;
      // Loop unrolled for 2 iterations.
      for (; j + 1 < end3; j += 2) {
        Packet l1, l2, l3, nl1, nl2, nl3;
        LoadSixScalars(&data3, &l1, &l2, &l3, &nl1, &nl2, &nl3);
        const auto& index = left.index3[j];
        const auto& nindex = left.index3[j + 1];
        float* out = out_ptrs[index.m];
        float* nout = out_ptrs[nindex.m];
        const auto* r1 = right_ptrs[index.k1];
        const auto* r2 = right_ptrs[index.k2];
        const auto* r3 = right_ptrs[index.k3];

        const auto* nr1 = right_ptrs[nindex.k1];
        const auto* nr2 = right_ptrs[nindex.k2];
        const auto* nr3 = right_ptrs[nindex.k3];
        if (cols == 128) {
          MulAdd3Way128(l1, l2, l3, &r1, &r2, &r3, &out);
          MulAdd3Way128(nl1, nl2, nl3, &nr1, &nr2, &nr3, &nout);
        } else {
          for (int n = 0; n < cols / kNumOperandsR; ++n) {
            MulAdd3Way(l1, l2, l3, &r1, &r2, &r3, &out);
            MulAdd3Way(nl1, nl2, nl3, &nr1, &nr2, &nr3, &nout);
          }

          const float sl1 = Eigen::internal::pfirst<Packet>(l1);
          const float sl2 = Eigen::internal::pfirst<Packet>(l2);
          const float sl3 = Eigen::internal::pfirst<Packet>(l3);
          const float nsl1 = Eigen::internal::pfirst<Packet>(nl1);
          const float nsl2 = Eigen::internal::pfirst<Packet>(nl2);
          const float nsl3 = Eigen::internal::pfirst<Packet>(nl3);
          for (int k = 0; k < cols_mod; ++k) {
            ScalarMulAdd3Way(sl1, sl2, sl3, &r1, &r2, &r3, &out);
            ScalarMulAdd3Way(nsl1, nsl2, nsl3, &nr1, &nr2, &nr3, &nout);
          }
        }
      }
      if (j < end3) {
        Packet l1, l2, l3;
        LoadThreeScalars(&data3, &l1, &l2, &l3);

        const auto& index = left.index3[j];
        float* out = out_ptrs[index.m];
        const auto* r1 = right_ptrs[index.k1];
        const auto* r2 = right_ptrs[index.k2];
        const auto* r3 = right_ptrs[index.k3];
        if (cols == 128) {
          MulAdd3Way128(l1, l2, l3, &r1, &r2, &r3, &out);
        } else {
          for (int n = 0; n < cols / kNumOperandsR; ++n) {
            MulAdd3Way(l1, l2, l3, &r1, &r2, &r3, &out);
          }
          const float sl1 = Eigen::internal::pfirst<Packet>(l1);
          const float sl2 = Eigen::internal::pfirst<Packet>(l2);
          const float sl3 = Eigen::internal::pfirst<Packet>(l3);
          for (int k = 0; k < cols_mod; ++k) {
            ScalarMulAdd3Way(sl1, sl2, sl3, &r1, &r2, &r3, &out);
          }
        }
      }
      begin3 = end3;
      int end = left.index_offset[i];
      // Loop unrolled for 4 iterations.
      j = begin;
      for (; j + 3 < end; j += 4) {
        Packet l, nl, n2l, n3l;
        LoadFourScalars(&data, &l, &nl, &n2l, &n3l);

        const auto& index = left.index[j];
        const auto& nindex = left.index[j + 1];
        const auto& n2index = left.index[j + 2];
        const auto& n3index = left.index[j + 3];
        const auto* r = right_ptrs[index.k];
        const auto* nr = right_ptrs[nindex.k];
        const auto* n2r = right_ptrs[n2index.k];
        const auto* n3r = right_ptrs[n3index.k];
        float* out = out_ptrs[index.m];
        float* nout = out_ptrs[nindex.m];
        float* n2out = out_ptrs[n2index.m];
        float* n3out = out_ptrs[n3index.m];

        for (int n = 0; n < cols / kNumOperandsR; ++n) {
          MulAdd(l, &r, &out);
          MulAdd(nl, &nr, &nout);
          MulAdd(n2l, &n2r, &n2out);
          MulAdd(n3l, &n3r, &n3out);
        }

        const float sl1 = Eigen::internal::pfirst<Packet>(l);
        const float sl2 = Eigen::internal::pfirst<Packet>(nl);
        const float sl3 = Eigen::internal::pfirst<Packet>(n2l);
        const float sl4 = Eigen::internal::pfirst<Packet>(n3l);
        for (int k = 0; k < cols_mod; ++k) {
          ScalarMulAdd(sl1, &r, &out);
          ScalarMulAdd(sl2, &nr, &nout);
          ScalarMulAdd(sl3, &n2r, &n2out);
          ScalarMulAdd(sl4, &n3r, &n3out);
        }
      }
      while (j < end) {
        Packet l;
        LoadSingleScalar(&data, &l);
        const auto& index = left.index[j];
        const auto* r = right_ptrs[index.k];
        float* out = out_ptrs[index.m];
        for (int n = 0; n < cols / kNumOperandsR; ++n) {
          MulAdd(l, &r, &out);
        }
        const float sl = Eigen::internal::pfirst<Packet>(l);
        for (int k = 0; k < cols_mod; ++k) {
          ScalarMulAdd(sl, &r, &out);
        }
        j++;
      }
      k_offset += left.block_size;
      begin = end;
    }
  }
}