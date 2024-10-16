void DoInplaceOp(const CPUDevice& d, InplaceOpType op, const Tensor& i,
                 const Tensor& v, Tensor* y) {
  auto Ti = i.flat<int32>();
  auto Tv = v.flat_outer_dims<T>();
  auto Ty = y->flat_outer_dims<T>();
  auto nrows = Ty.dimension(0);
  for (int64_t j = 0; j < Ti.size(); ++j) {
    auto r = (Ti(j) % nrows + nrows) % nrows;  // Guard index range.
    switch (op) {
      case I_UPDATE:
        Ty.template chip<0>(r).device(d) = Tv.template chip<0>(j);
        break;
      case I_ADD:
        Ty.template chip<0>(r).device(d) += Tv.template chip<0>(j);
        break;
      case I_SUB:
        Ty.template chip<0>(r).device(d) -= Tv.template chip<0>(j);
        break;
    }
  }
}