  SparseSlice(int nrows, int ncols, int bsize)
      : num_rows(nrows), num_cols(ncols), block_size(bsize) {
    DCHECK_LE(nrows, 256);
    DCHECK_LE(block_size, 256);
  }