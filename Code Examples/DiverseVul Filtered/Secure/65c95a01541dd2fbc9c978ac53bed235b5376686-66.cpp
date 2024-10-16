static bool HHVM_METHOD(ZipArchive, setCommentName, const String& name,
                        const String& comment) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(setCommentName, zipDir);
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(setCommentName, name);

  int index = zip_name_locate(zipDir->getZip(), name.c_str(), 0);
  FAIL_IF_INVALID_INDEX(index);

  if (zip_set_file_comment(zipDir->getZip(), index, comment.c_str(),
                           comment.length()) != 0 ) {
    return false;
  }

  zip_error_clear(zipDir->getZip());
  return true;
}