Status DoCopy(const CPUDevice& device, const Tensor& x, Tensor* y) {
  CHECK_EQ(x.dtype(), y->dtype());
  switch (x.dtype()) {
#define CASE(type)                                   \
  case DataTypeToEnum<type>::value:                  \
    y->flat<type>().device(device) = x.flat<type>(); \
    break;

    TF_CALL_NUMBER_TYPES(CASE);
    TF_CALL_bool(CASE);
    TF_CALL_tstring(CASE);
#undef CASE
    default:
      return errors::InvalidArgument("Unsupported data type: ",
                                     DataTypeString(x.dtype()));
  }
  return Status::OK();
}