Error HeifContext::add_XMP_metadata(std::shared_ptr<Image> master_image, const void* data, int size)
{
  // create an infe box describing what kind of data we are storing (this also creates a new ID)

  auto metadata_infe_box = m_heif_file->add_new_infe_box("mime");
  metadata_infe_box->set_content_type("application/rdf+xml");
  metadata_infe_box->set_hidden_item(true);

  heif_item_id metadata_id = metadata_infe_box->get_item_ID();


  // we assign this data to the image

  m_heif_file->add_iref_reference(metadata_id,
                                  fourcc("cdsc"), { master_image->get_id() });


  // copy the XMP data into the file, store the pointer to it in an iloc box entry

  std::vector<uint8_t> data_array;
  data_array.resize(size);
  memcpy(data_array.data(), data, size);

  m_heif_file->append_iloc_data(metadata_id, data_array);

  return Error::Ok;
}