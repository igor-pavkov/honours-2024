void HeifContext::remove_top_level_image(std::shared_ptr<Image> image)
{
  std::vector<std::shared_ptr<Image>> new_list;

  for (auto img : m_top_level_images) {
    if (img != image) {
      new_list.push_back(img);
    }
  }

  m_top_level_images = new_list;
}