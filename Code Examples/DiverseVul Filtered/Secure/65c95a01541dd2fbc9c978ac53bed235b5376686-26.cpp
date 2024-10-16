static bool HHVM_FUNCTION(zip_entry_close, const Resource& zip_entry) {
  auto zipEntry = cast<ZipEntry>(zip_entry);

  FAIL_IF_INVALID_ZIPENTRY(zip_entry_close, zipEntry);

  return zipEntry->close();
}