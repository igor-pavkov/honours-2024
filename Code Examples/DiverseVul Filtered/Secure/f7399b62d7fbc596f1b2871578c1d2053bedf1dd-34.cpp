Error HeifContext::read_from_memory(const void* data, size_t size, bool copy)
{
  m_heif_file = std::make_shared<HeifFile>();
  Error err = m_heif_file->read_from_memory(data,size, copy);
  if (err) {
    return err;
  }

  return interpret_heif_file();
}