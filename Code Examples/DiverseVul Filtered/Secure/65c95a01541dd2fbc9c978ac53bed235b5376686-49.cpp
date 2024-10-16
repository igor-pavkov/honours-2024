static Variant HHVM_FUNCTION(zip_entry_name, const Resource& zip_entry) {
  auto zipEntry = cast<ZipEntry>(zip_entry);

  FAIL_IF_INVALID_ZIPENTRY(zip_entry_name, zipEntry);

  return zipEntry->getName();
}