void DoInplaceStringUpdateOp(const CPUDevice& d, const Tensor& i,
                             const Tensor& v, Tensor* y) {
  auto Ti = i.flat<int32>();
  auto Tv = v.flat_outer_dims<tstring>();
  auto Ty = y->flat_outer_dims<tstring>();
  auto nrows = Ty.dimension(0);
  for (int64_t j = 0; j < Ti.size(); ++j) {
    auto r = (Ti(j) % nrows + nrows) % nrows;  // Guard index range.
    Ty.template chip<0>(r).device(d) = Tv.template chip<0>(j);
  }
}