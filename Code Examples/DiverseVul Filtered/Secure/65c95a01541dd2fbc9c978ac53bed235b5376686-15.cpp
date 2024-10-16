static bool HHVM_METHOD(ZipArchive, unchangeName, const String& name) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(unchangeName, zipDir);
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(unchangeName, name);

  struct zip_stat zipStat;
  if (zip_stat(zipDir->getZip(), name.c_str(), 0, &zipStat) != 0) {
    return false;
  }

  if (zip_unchange(zipDir->getZip(), zipStat.index) != 0) {
    return false;
  }

  zip_error_clear(zipDir->getZip());
  return true;
}