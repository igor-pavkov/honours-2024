Error HeifContext::Image::encode_image_as_hevc(std::shared_ptr<HeifPixelImage> image,
                                               struct heif_encoder* encoder,
                                               const struct heif_encoding_options* options,
                                               enum heif_image_input_class input_class)
{
  /*
  const struct heif_encoder_plugin* encoder_plugin = nullptr;

  encoder_plugin = m_heif_context->get_encoder(heif_compression_HEVC);

  if (encoder_plugin == nullptr) {
    return Error(heif_error_Unsupported_feature,
                 heif_suberror_Unsupported_codec);
  }
  */



  // --- check whether we have to convert the image color space

  heif_colorspace colorspace = image->get_colorspace();
  heif_chroma chroma = image->get_chroma_format();
  auto color_profile = image->get_color_profile();

  encoder->plugin->query_input_colorspace(&colorspace, &chroma);

  if (colorspace != image->get_colorspace() ||
      chroma != image->get_chroma_format()) {
    // @TODO: use color profile when converting
    image = convert_colorspace(image, colorspace, chroma);
  }


  m_width  = image->get_width(heif_channel_Y);
  m_height = image->get_height(heif_channel_Y);

  if (color_profile) {
    m_heif_context->m_heif_file->set_color_profile(m_id, color_profile);
  }

  // --- if there is an alpha channel, add it as an additional image

  if (options->save_alpha_channel && image->has_channel(heif_channel_Alpha)) {

    // --- generate alpha image
    // TODO: can we directly code a monochrome image instead of the dummy color channels?

    std::shared_ptr<HeifPixelImage> alpha_image;
    alpha_image = create_alpha_image_from_image_alpha_channel(image);


    // --- encode the alpha image

    heif_item_id alpha_image_id = m_heif_context->m_heif_file->add_new_image("hvc1");

    std::shared_ptr<HeifContext::Image> heif_alpha_image;
    heif_alpha_image = std::make_shared<Image>(m_heif_context, alpha_image_id);


    Error error = heif_alpha_image->encode_image_as_hevc(alpha_image, encoder, options,
                                                         heif_image_input_class_alpha);
    if (error) {
      return error;
    }

    m_heif_context->m_heif_file->add_iref_reference(alpha_image_id, fourcc("auxl"), { m_id });
    m_heif_context->m_heif_file->set_auxC_property(alpha_image_id, "urn:mpeg:hevc:2015:auxid:1");
  }


  m_heif_context->m_heif_file->add_hvcC_property(m_id);



  heif_image c_api_image;
  c_api_image.image = image;

  struct heif_error err = encoder->plugin->encode_image(encoder->encoder, &c_api_image, input_class);
  if (err.code) {
    return Error(err.code,
                 err.subcode,
                 err.message);
  }

  for (;;) {
    uint8_t* data;
    int size;

    encoder->plugin->get_compressed_data(encoder->encoder, &data, &size, NULL);

    if (data==NULL) {
      break;
    }


    const uint8_t NAL_SPS = 33;

    if ((data[0] >> 1) == NAL_SPS) {
      int width,height;
      Box_hvcC::configuration config;

      parse_sps_for_hvcC_configuration(data, size, &config, &width, &height);

      m_heif_context->m_heif_file->set_hvcC_configuration(m_id, config);
      m_heif_context->m_heif_file->add_ispe_property(m_id, width, height);
    }

    switch (data[0] >> 1) {
    case 0x20:
    case 0x21:
    case 0x22:
      m_heif_context->m_heif_file->append_hvcC_nal_data(m_id, data, size);
      break;

    default:
      m_heif_context->m_heif_file->append_iloc_data_with_4byte_size(m_id, data, size);
    }
  }

  return Error::Ok;
}