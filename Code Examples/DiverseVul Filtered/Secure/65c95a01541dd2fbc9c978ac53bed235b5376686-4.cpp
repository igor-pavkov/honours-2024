  bool close() override {
    bool noError = true;
    if (!eof()) {
      if (zip_fclose(m_zipFile) != 0) {
        noError = false;
      }
      m_zipFile = nullptr;
    }
    return noError;
  }