void LogUnexpectedSize(int64_t actual, int64_t expected) {
  LOG(ERROR) << "Input size was " << actual << " and expected " << expected;
}