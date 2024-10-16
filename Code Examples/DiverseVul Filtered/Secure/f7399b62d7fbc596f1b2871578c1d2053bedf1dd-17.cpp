void HeifContext::register_decoder(const heif_decoder_plugin* decoder_plugin)
{
  if (decoder_plugin->init_plugin) {
    (*decoder_plugin->init_plugin)();
  }

  m_decoder_plugins.insert(decoder_plugin);
}