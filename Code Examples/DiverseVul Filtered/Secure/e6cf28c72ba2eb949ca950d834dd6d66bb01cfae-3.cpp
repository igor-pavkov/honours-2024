void wrapper_libxsmm_spmdm_createSparseSlice_generic_thread(
    empty_type_wrapper<float>, const libxsmm_spmdm_handle* handle, char transA,
    const float* A, libxsmm_CSR_sparseslice* libxsmm_output_csr_a, int block_id,
    int tid, int nthreads) {
  return libxsmm_spmdm_createSparseSlice_fp32_thread(
      handle, transA, A, libxsmm_output_csr_a, block_id, tid, nthreads);
}