static const char *print_matrix(const double *mat) {
  delete gstr_buff0;

  gstr_buff0 =  GooString::format("[{0:g} {1:g} {2:g} {3:g} {4:g} {5:g}]",
                                  *mat, mat[1], mat[2], mat[3], mat[4], mat[5]);
  return gstr_buff0->c_str();
}