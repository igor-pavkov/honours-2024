Error HeifContext::set_primary_item(heif_item_id id)
{
  auto iter = m_all_images.find(id);
  if (iter == m_all_images.end()) {
    return Error(heif_error_Usage_error,
                 heif_suberror_No_or_invalid_primary_item,
                 "Cannot set primary item as the ID does not exist.");
  }

  set_primary_image(iter->second);

  return Error::Ok;
}