bool Tensor::CanUseDMA() const {
  CASES(dtype(), return is_simple_type<T>::value);
  return false;  // Makes compiler happy.
}