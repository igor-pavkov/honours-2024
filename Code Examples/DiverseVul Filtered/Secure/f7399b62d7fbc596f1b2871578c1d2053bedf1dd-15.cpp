Error HeifContext::encode_image(std::shared_ptr<HeifPixelImage> pixel_image,
                                struct heif_encoder* encoder,
                                const struct heif_encoding_options* options,
                                enum heif_image_input_class input_class,
                                std::shared_ptr<Image>& out_image)
{
  Error error;

  switch (encoder->plugin->compression_format) {
    case heif_compression_HEVC:
      {
        heif_item_id image_id = m_heif_file->add_new_image("hvc1");

        out_image = std::make_shared<Image>(this, image_id);
        m_top_level_images.push_back(out_image);

        error = out_image->encode_image_as_hevc(pixel_image,
                                                encoder,
                                                options,
                                                heif_image_input_class_normal);
      }
      break;

    default:
      return Error(heif_error_Encoder_plugin_error, heif_suberror_Unsupported_codec);
  }

  return error;
}