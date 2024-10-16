  Variant nextFile() {
    if (m_curIndex >= m_numFiles) {
      return false;
    }

    auto zipEntry = req::make<ZipEntry>(m_zip, m_curIndex);

    if (!zipEntry->isValid()) {
      return false;
    }

    ++m_curIndex;

    return Variant(std::move(zipEntry));
  }