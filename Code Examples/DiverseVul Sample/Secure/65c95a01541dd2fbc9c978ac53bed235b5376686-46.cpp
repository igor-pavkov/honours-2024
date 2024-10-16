static bool HHVM_METHOD(ZipArchive, setCommentIndex, int64_t index,
                        const String& comment) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(setCommentIndex, zipDir);

  struct zip_stat zipStat;
  if (zip_stat_index(zipDir->getZip(), index, 0, &zipStat) != 0) {
    return false;
  }

  if (zip_set_file_comment(zipDir->getZip(), index, comment.c_str(),
                           comment.length()) != 0 ) {
    return false;
  }

  zip_error_clear(zipDir->getZip());
  return true;
}