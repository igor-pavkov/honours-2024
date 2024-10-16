Error HeifContext::read(std::shared_ptr<StreamReader> reader)
{
  m_heif_file = std::make_shared<HeifFile>();
  Error err = m_heif_file->read(reader);
  if (err) {
    return err;
  }

  return interpret_heif_file();
}