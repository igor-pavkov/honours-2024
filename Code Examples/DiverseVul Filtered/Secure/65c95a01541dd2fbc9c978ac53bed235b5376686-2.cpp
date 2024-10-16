static Variant HHVM_FUNCTION(zip_entry_read, const Resource& zip_entry,
                             int64_t length) {
  auto zipEntry = cast<ZipEntry>(zip_entry);

  FAIL_IF_INVALID_ZIPENTRY(zip_entry_read, zipEntry);

  return zipEntry->read(length > 0 ? length : 1024);
}