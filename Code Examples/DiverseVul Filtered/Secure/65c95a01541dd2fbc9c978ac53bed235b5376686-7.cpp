static bool HHVM_METHOD(ZipArchive, addGlob, const String& pattern,
                        int64_t flags, const Array& options) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(addGlob, zipDir);
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(addGlob, pattern);

  return addPattern(zipDir->getZip(), pattern, options, "", flags, true);
}