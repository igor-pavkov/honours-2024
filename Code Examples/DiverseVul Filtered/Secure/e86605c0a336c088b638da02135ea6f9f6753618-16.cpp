Status DoParallelConcat(const CPUDevice& d, const Tensor& value, int32_t loc,
                        Tensor* output) {
  CHECK_EQ(value.dtype(), output->dtype());
  switch (value.dtype()) {
#define CASE(type)                  \
  case DataTypeToEnum<type>::value: \
    return DoParallelConcatUpdate<CPUDevice, type>(d, value, loc, output);
    TF_CALL_POD_TYPES(CASE);
    TF_CALL_tstring(CASE);
    TF_CALL_variant(CASE);
#undef CASE
    default:
      return errors::InvalidArgument("Unsupported data type: ",
                                     DataTypeString(value.dtype()));
  }
}