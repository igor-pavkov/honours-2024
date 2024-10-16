static Variant HHVM_FUNCTION(zip_entry_filesize, const Resource& zip_entry) {
  auto zipEntry = cast<ZipEntry>(zip_entry);

  FAIL_IF_INVALID_ZIPENTRY(zip_entry_filesize, zipEntry);

  return zipEntry->getSize();
}