void wrapper_libxsmm_spmdm_compute_generic_thread(
    empty_type_wrapper<bfloat16>, const libxsmm_spmdm_handle* handle,
    char transA, char transB, const bfloat16* alpha,
    libxsmm_CSR_sparseslice* A_sparse, const bfloat16* B, char transC,
    const bfloat16* beta, float* C, int block_id, int tid, int nthreads) {
  return libxsmm_spmdm_compute_bfloat16_thread(
      handle, transA, transB, reinterpret_cast<const libxsmm_bfloat16*>(alpha),
      A_sparse, reinterpret_cast<const libxsmm_bfloat16*>(B), transC,
      reinterpret_cast<const libxsmm_bfloat16*>(beta), C, block_id, tid,
      nthreads);
}