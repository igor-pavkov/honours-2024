  explicit ZipDirectory(zip *z) : m_zip(z),
                                  m_numFiles(zip_get_num_files(z)),
                                  m_curIndex(0) {}