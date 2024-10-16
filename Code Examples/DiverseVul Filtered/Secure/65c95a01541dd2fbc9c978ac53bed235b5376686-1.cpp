  bool close() {
    bool noError = true;
    if (isValid()) {
      if (zip_close(m_zip) != 0) {
        zip_discard(m_zip);
        noError = false;
      }
      m_zip = nullptr;
    }
    return noError;
  }