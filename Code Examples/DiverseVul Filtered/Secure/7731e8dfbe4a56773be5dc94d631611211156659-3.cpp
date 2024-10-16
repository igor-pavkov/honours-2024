int64_t UniqueConstantId() {
  static std::atomic_int_fast64_t unique_constant_id;
  return unique_constant_id.fetch_add(1);
}