  bool close() {
    bool noError = true;
    if (isValid()) {
      if (zip_fclose(m_zipFile) != 0) {
        noError = false;
      }
      m_zipFile = nullptr;
    }
    return noError;
  }