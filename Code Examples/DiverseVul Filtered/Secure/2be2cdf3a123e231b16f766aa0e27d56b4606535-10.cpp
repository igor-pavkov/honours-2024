  void operator()(OpKernelContext* ctx, const T* out_data, const T* filter_data,
                  const int filter_total_size, const int output_image_size,
                  const int dims_out_depth, T* im2col_buf) {
    // Inputs are in RowMajor order.
    //   im2col      = out_data    * filter_data^T
    //   [ois x fts] = [ois x dod] * [fts x dod]^T
    //
    // Dimension names:
    //   out_image_size    -> ois
    //   filter_total_size -> fts
    //   dims_out_depth    -> dod

    const int m = output_image_size;
    const int n = filter_total_size;
    const int k = dims_out_depth;  // contraction dim

    const char transposeA = 'N';  // sgemm(A) == filter_data
    const char transposeB = 'T';  // sgemm(B) == out_data

    const int ldA = dims_out_depth;
    const int ldB = dims_out_depth;
    const int ldC = filter_total_size;

    const float alpha = 1.0;
    const float beta = 0.0;

    // dnnl_sgemm code can't be instrumented with msan.
    ANNOTATE_MEMORY_IS_INITIALIZED(
        im2col_buf, filter_total_size * output_image_size * sizeof(T));

    dnnl_status_t st =
        dnnl_sgemm(transposeA, transposeB, m, n, k, alpha, out_data, ldA,
                   filter_data, ldB, beta, im2col_buf, ldC);

    OP_REQUIRES(
        ctx, st == 0,
        errors::Internal("Failed to call dnnl_sgemm. Error code: ", st));
  }