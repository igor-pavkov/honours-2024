static bool HHVM_METHOD(ZipArchive, addEmptyDir, const String& dirname) {
  if (dirname.empty()) {
    return false;
  }

  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(addEmptyDir, zipDir);
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(addEmptyDir, dirname);

  std::string dirStr(dirname.c_str());
  if (dirStr[dirStr.length() - 1] != '/') {
    dirStr.push_back('/');
  }

  struct zip_stat zipStat;
  if (zip_stat(zipDir->getZip(), dirStr.c_str(), 0, &zipStat) != -1) {
    return false;
  }

  if (zip_add_dir(zipDir->getZip(), dirStr.c_str()) == -1) {
    return false;
  }

  zip_error_clear(zipDir->getZip());
  return true;
}