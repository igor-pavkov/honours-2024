HeifContext::Image::Image(HeifContext* context, heif_item_id id)
  : m_heif_context(context),
    m_id(id)
{
  memset(&m_depth_representation_info, 0, sizeof(m_depth_representation_info));
}