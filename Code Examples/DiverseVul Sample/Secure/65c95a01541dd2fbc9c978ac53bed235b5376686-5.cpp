  ZipStream(zip* z, const String& name) : m_zipFile(nullptr) {
    if (name.empty()) {
      return;
    }

    struct zip_stat zipStat;
    if (zip_stat(z, name.c_str(), 0, &zipStat) != 0) {
      return;
    }

    m_zipFile = zip_fopen(z, name.c_str(), 0);
  }