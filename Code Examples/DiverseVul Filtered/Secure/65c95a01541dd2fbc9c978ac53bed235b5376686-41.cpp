static Variant HHVM_METHOD(ZipArchive, statName, const String& name,
                           int64_t flags) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(statName, zipDir);
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(statName, name);

  struct zip_stat zipStat;
  if (zip_stat(zipDir->getZip(), name.c_str(), flags, &zipStat) != 0) {
    return false;
  }

  return zipStatToArray(&zipStat);
}