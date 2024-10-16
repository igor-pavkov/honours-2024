static bool HHVM_METHOD(ZipArchive, addFromString, const String& localname,
                        const String& contents) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(addFromString, zipDir);
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(addFromString, localname);

  auto data = malloc(contents.length());
  FAIL_IF_INVALID_PTR(data);

  memcpy(data, contents.c_str(), contents.length());

  auto zipSource = zip_source_buffer(zipDir->getZip(), data, contents.length(),
                                     1); // this will free data ptr
  if (zipSource == nullptr) {
    free(data);
    return false;
  }

  auto index = zip_name_locate(zipDir->getZip(), localname.c_str(), 0);
  if (index < 0) {
    if (zip_add(zipDir->getZip(), localname.c_str(), zipSource) == -1) {
      zip_source_free(zipSource);
      return false;
    }
  } else {
    if (zip_replace(zipDir->getZip(), index, zipSource) == -1) {
      zip_source_free(zipSource);
      return false;
    }
  }

  zip_error_clear(zipDir->getZip());
  return true;
}