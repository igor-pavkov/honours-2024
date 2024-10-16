static bool HHVM_METHOD(ZipArchive, addPattern, const String& pattern,
                        const String& path, const Array& options) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(addPattern, zipDir);
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(addPattern, pattern);

  return addPattern(zipDir->getZip(), pattern, options, path.c_str(), 0, false);
}