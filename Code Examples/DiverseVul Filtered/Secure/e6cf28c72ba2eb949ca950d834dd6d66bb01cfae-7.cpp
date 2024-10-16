void wrapper_libxsmm_spmdm_createSparseSlice_generic_thread(
    empty_type_wrapper<bfloat16>, const libxsmm_spmdm_handle* handle,
    char transA, const bfloat16* A,
    libxsmm_CSR_sparseslice* libxsmm_output_csr_a, int block_id, int tid,
    int nthreads) {
  return libxsmm_spmdm_createSparseSlice_bfloat16_thread(
      handle, transA, reinterpret_cast<const libxsmm_bfloat16*>(A),
      libxsmm_output_csr_a, block_id, tid, nthreads);
}