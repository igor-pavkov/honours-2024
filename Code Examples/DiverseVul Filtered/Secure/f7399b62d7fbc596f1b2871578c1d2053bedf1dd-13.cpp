Error HeifContext::assign_thumbnail(std::shared_ptr<Image> master_image,
                                    std::shared_ptr<Image> thumbnail_image)
{
  m_heif_file->add_iref_reference(thumbnail_image->get_id(),
                                  fourcc("thmb"), { master_image->get_id() });

  return Error::Ok;
}