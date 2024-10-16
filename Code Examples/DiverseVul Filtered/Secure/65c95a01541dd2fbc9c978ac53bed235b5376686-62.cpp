  int64_t readImpl(char *buffer, int64_t length) override {
    auto n = zip_fread(m_zipFile, buffer, length);
    if (n <= 0) {
      if (n == -1) {
        raise_warning("Zip stream error");
        n = 0;
      }
      close();
    }
    return n;
  }