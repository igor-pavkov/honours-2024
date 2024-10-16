void InitializeToZeroArray(T (&ptr)[M]) {
  memset(ptr, 0, sizeof(T) * M);
}