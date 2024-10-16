void wrapper_libxsmm_spmdm_compute_generic_thread(
    empty_type_wrapper<float>, const libxsmm_spmdm_handle* handle, char transA,
    char transB, const float* alpha, libxsmm_CSR_sparseslice* A_sparse,
    const float* B, char transC, const float* beta, float* C, int block_id,
    int tid, int nthreads) {
  return libxsmm_spmdm_compute_fp32_thread(handle, transA, transB, alpha,
                                           A_sparse, B, transC, beta, C,
                                           block_id, tid, nthreads);
}