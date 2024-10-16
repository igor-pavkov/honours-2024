Status DoParallelConcatUpdate(const Device& d, const Tensor& value, int32_t loc,
                              Tensor* output) {
  auto Tvalue = value.shaped<T, 2>({1, value.NumElements()});
  auto Toutput = output->flat_outer_dims<T>();
  auto nrows = Toutput.dimension(0);
  auto r = (loc % nrows + nrows) % nrows;  // Guard index range.
  Toutput.template chip<0>(r).device(d) = Tvalue.template chip<0>(0);
  return Status::OK();
}