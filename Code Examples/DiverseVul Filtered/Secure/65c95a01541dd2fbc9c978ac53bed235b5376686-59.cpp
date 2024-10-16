static bool HHVM_METHOD(ZipArchive, setArchiveComment, const String& comment) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(setArchiveComment, zipDir);

  if (zip_set_archive_comment(zipDir->getZip(), comment.c_str(),
                              comment.length()) != 0) {
    return false;
  }

  zip_error_clear(zipDir->getZip());
  return true;
}