Status DoInplace(const CPUDevice& device, InplaceOpType op, const Tensor& i,
                 const Tensor& v, Tensor* y) {
  CHECK_EQ(v.dtype(), y->dtype());
  if (op == I_UPDATE) {
    if (v.dtype() == DT_STRING) {
      DoInplaceStringUpdateOp(device, i, v, y);
      return Status::OK();
    } else if (v.dtype() == DT_BOOL) {
      DoInplaceOp<bool>(device, op, i, v, y);
      return Status::OK();
    }
  }
  switch (v.dtype()) {
#define CASE(type)                          \
  case DataTypeToEnum<type>::value:         \
    DoInplaceOp<type>(device, op, i, v, y); \
    break;
    TF_CALL_NUMBER_TYPES(CASE);
#undef CASE
    default:
      return errors::InvalidArgument("Unsupported data type: ",
                                     DataTypeString(v.dtype()));
  }
  return Status::OK();
}