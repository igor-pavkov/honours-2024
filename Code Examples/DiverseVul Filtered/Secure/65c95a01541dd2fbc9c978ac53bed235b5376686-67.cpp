static bool HHVM_METHOD(ZipArchive, renameIndex, int64_t index,
                        const String& newname) {
  auto zipDir = getResource<ZipDirectory>(this_, "zipDir");

  FAIL_IF_INVALID_ZIPARCHIVE(renameIndex, zipDir);
  FAIL_IF_INVALID_INDEX(index);
  FAIL_IF_EMPTY_STRING_ZIPARCHIVE(renameIndex, newname);

  if (zip_rename(zipDir->getZip(), index, newname.c_str()) != 0) {
    return false;
  }

  zip_error_clear(zipDir->getZip());
  return true;
}