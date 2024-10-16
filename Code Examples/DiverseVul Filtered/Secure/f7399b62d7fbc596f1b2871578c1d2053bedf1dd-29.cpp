Error HeifContext::add_exif_metadata(std::shared_ptr<Image> master_image, const void* data, int size)
{
  // find location of TIFF header
  uint32_t offset = 0;
  const char * tiffmagic1 = "MM\0*";
  const char * tiffmagic2 = "II*\0";
  while (offset+4 < (unsigned int)size) {
        if (!memcmp( (uint8_t *) data + offset, tiffmagic1, 4 )) break;
        if (!memcmp( (uint8_t *) data + offset, tiffmagic2, 4 )) break;
        offset++;
  }
  if (offset >= (unsigned int)size) {
    return Error(heif_error_Usage_error,
                  heif_suberror_Invalid_parameter_value,
                 "Could not find location of TIFF header in Exif metadata.");
  }

  // create an infe box describing what kind of data we are storing (this also creates a new ID)

  auto metadata_infe_box = m_heif_file->add_new_infe_box("Exif");
  metadata_infe_box->set_hidden_item(true);

  heif_item_id metadata_id = metadata_infe_box->get_item_ID();


  // we assign this data to the image

  m_heif_file->add_iref_reference(metadata_id,
                                  fourcc("cdsc"), { master_image->get_id() });


  // copy the Exif data into the file, store the pointer to it in an iloc box entry



  std::vector<uint8_t> data_array;
  data_array.resize(size+4);
  data_array[0] = (uint8_t) ((offset >> 24) & 0xFF);
  data_array[1] = (uint8_t) ((offset >> 16) & 0xFF);
  data_array[2] = (uint8_t) ((offset >> 8) & 0xFF);
  data_array[3] = (uint8_t) ((offset) & 0xFF);
  memcpy(data_array.data()+4, data, size);

  m_heif_file->append_iloc_data(metadata_id, data_array);

  return Error::Ok;
}