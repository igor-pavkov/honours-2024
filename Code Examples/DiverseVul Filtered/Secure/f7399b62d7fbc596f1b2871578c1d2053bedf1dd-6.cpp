int HeifContext::Image::get_chroma_bits_per_pixel() const
{
  heif_item_id id;
  Error err = m_heif_context->get_id_of_non_virtual_child_image(m_id, id);
  if (err) {
    return -1;
  }

  return m_heif_context->m_heif_file->get_chroma_bits_per_pixel_from_configuration(id);
}