const struct heif_decoder_plugin* HeifContext::get_decoder(enum heif_compression_format type) const
{
  int highest_priority = 0;
  const struct heif_decoder_plugin* best_plugin = nullptr;


  // search global plugins

  best_plugin = heif::get_decoder(type);
  if (best_plugin != nullptr) {
    highest_priority = best_plugin->does_support_format(type);
  }


  // search context-local plugins (DEPRECATED)

  for (const auto* plugin : m_decoder_plugins) {
    int priority = plugin->does_support_format(type);
    if (priority > highest_priority) {
      highest_priority = priority;
      best_plugin = plugin;
    }
  }

  return best_plugin;
}