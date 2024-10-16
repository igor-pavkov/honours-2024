static bool HHVM_METHOD(ZipArchive, renameName, const String& name,
                        const String& newname) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(renameName, zipDir);
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(renameName, newname);

  struct zip_stat zipStat;
  if (zip_stat(zipDir->getZip(), name.c_str(), 0, &zipStat) != 0) {
    return false;
  }

  if (zip_rename(zipDir->getZip(), zipStat.index, newname.c_str()) != 0) {
    return false;
  }

  zip_error_clear(zipDir->getZip());
  return true;
}