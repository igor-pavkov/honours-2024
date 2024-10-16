static Variant HHVM_METHOD(ZipArchive, statIndex, int64_t index,
                           int64_t flags) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(statIndex, zipDir);
  FAIL_IF_INVALID_INDEX(index);

  struct zip_stat zipStat;
  if (zip_stat_index(zipDir->getZip(), index, flags, &zipStat) != 0) {
    return false;
  }

  return zipStatToArray(&zipStat);
}