  ZipEntry(zip* z, int index) : m_zipFile(nullptr) {
    if (zip_stat_index(z, index, 0, &m_zipStat) == 0) {
      m_zipFile = zip_fopen_index(z, index, 0);
    }
  }