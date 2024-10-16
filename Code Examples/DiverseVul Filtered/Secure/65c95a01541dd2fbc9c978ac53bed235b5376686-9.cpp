static bool HHVM_METHOD(ZipArchive, addFile, const String& filename,
                        const String& localname, int64_t start,
                        int64_t length) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(addFile, zipDir);
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(addFile, filename);

  return addFile(zipDir->getZip(), filename.c_str(),
                 localname.empty() ? filename.c_str() : localname.c_str(),
                 start, length);
}