static Variant HHVM_FUNCTION(zip_entry_compressedsize, const Resource& zip_entry) {
  auto zipEntry = cast<ZipEntry>(zip_entry);

  FAIL_IF_INVALID_ZIPENTRY(zip_entry_compressedsize, zipEntry);

  return zipEntry->getCompressedSize();
}