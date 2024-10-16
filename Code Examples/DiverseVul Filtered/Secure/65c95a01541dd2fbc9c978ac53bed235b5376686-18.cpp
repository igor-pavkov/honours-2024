static Variant HHVM_METHOD(ZipArchive, locateName, const String& name,
                           int64_t flags) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(locateName, zipDir);
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(locateName, name);

  auto index = zip_name_locate(zipDir->getZip(), name.c_str(), flags);
  FAIL_IF_INVALID_INDEX(index);

  return index;
}