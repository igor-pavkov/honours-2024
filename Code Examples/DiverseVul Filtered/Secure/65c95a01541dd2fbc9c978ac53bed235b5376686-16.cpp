static bool HHVM_FUNCTION(zip_entry_open, const Resource& zip, const Resource& zip_entry,
                          const String& mode) {
  auto zipDir   = cast<ZipDirectory>(zip);
  auto zipEntry = cast<ZipEntry>(zip_entry);

  FAIL_IF_INVALID_ZIPDIRECTORY(zip_entry_open, zipDir);
  FAIL_IF_INVALID_ZIPENTRY(zip_entry_open, zipEntry);

  zip_error_clear(zipDir->getZip());
  return true;
}