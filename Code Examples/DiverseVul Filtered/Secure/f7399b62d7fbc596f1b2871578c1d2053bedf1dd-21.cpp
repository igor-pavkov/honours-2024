Error HeifContext::decode_image(heif_item_id ID,
                                std::shared_ptr<HeifPixelImage>& img,
                                const struct heif_decoding_options* options) const
{
  std::string image_type = m_heif_file->get_item_type(ID);

  Error error;


  // --- decode image, depending on its type

  if (image_type == "hvc1") {
    const struct heif_decoder_plugin* decoder_plugin = get_decoder(heif_compression_HEVC);
    if (!decoder_plugin) {
      return Error(heif_error_Unsupported_feature, heif_suberror_Unsupported_codec);
    }

    std::vector<uint8_t> data;
    error = m_heif_file->get_compressed_image_data(ID, &data);
    if (error) {
      return error;
    }

    void* decoder;
    struct heif_error err = decoder_plugin->new_decoder(&decoder);
    if (err.code != heif_error_Ok) {
      return Error(err.code, err.subcode, err.message);
    }

    err = decoder_plugin->push_data(decoder, data.data(), data.size());
    if (err.code != heif_error_Ok) {
      decoder_plugin->free_decoder(decoder);
      return Error(err.code, err.subcode, err.message);
    }

    //std::shared_ptr<HeifPixelImage>* decoded_img;

    heif_image* decoded_img = nullptr;
    err = decoder_plugin->decode_image(decoder, &decoded_img);
    if (err.code != heif_error_Ok) {
      decoder_plugin->free_decoder(decoder);
      return Error(err.code, err.subcode, err.message);
    }

    if (!decoded_img) {
      // TODO(farindk): The plugin should return an error in this case.
      decoder_plugin->free_decoder(decoder);
      return Error(heif_error_Decoder_plugin_error, heif_suberror_Unspecified);
    }

    img = std::move(decoded_img->image);
    heif_image_release(decoded_img);

    decoder_plugin->free_decoder(decoder);

#if 0
    FILE* fh = fopen("out.bin", "wb");
    fwrite(data.data(), 1, data.size(), fh);
    fclose(fh);
#endif
  }
  else if (image_type == "grid") {
    std::vector<uint8_t> data;
    error = m_heif_file->get_compressed_image_data(ID, &data);
    if (error) {
      return error;
    }

    error = decode_full_grid_image(ID, img, data);
    if (error) {
      return error;
    }
  }
  else if (image_type == "iden") {
    error = decode_derived_image(ID, img);
    if (error) {
      return error;
    }
  }
  else if (image_type == "iovl") {
    std::vector<uint8_t> data;
    error = m_heif_file->get_compressed_image_data(ID, &data);
    if (error) {
      return error;
    }

    error = decode_overlay_image(ID, img, data);
    if (error) {
      return error;
    }
  }
  else {
    // Should not reach this, was already rejected by "get_image_data".
    return Error(heif_error_Unsupported_feature,
                 heif_suberror_Unsupported_image_type);
  }



  // --- add alpha channel, if available

  // TODO: this if statement is probably wrong. When we have a tiled image with alpha
  // channel, then the alpha images should be associated with their respective tiles.
  // However, the tile images are not part of the m_all_images list.
  // Fix this, when we have a test image available.
  if (m_all_images.find(ID) != m_all_images.end()) {
    const auto imginfo = m_all_images.find(ID)->second;

    std::shared_ptr<Image> alpha_image = imginfo->get_alpha_channel();
    if (alpha_image) {
      std::shared_ptr<HeifPixelImage> alpha;
      Error err = alpha_image->decode_image(alpha);
      if (err) {
        return err;
      }

      // TODO: check that sizes are the same and that we have an Y channel
      // BUT: is there any indication in the standard that the alpha channel should have the same size?

      img->transfer_plane_from_image_as(alpha, heif_channel_Y, heif_channel_Alpha);
    }
  }


  // --- apply image transformations

  if (!options || options->ignore_transformations == false) {
    std::vector<Box_ipco::Property> properties;
    auto ipco_box = m_heif_file->get_ipco_box();
    auto ipma_box = m_heif_file->get_ipma_box();
    error = ipco_box->get_properties_for_item_ID(ID, ipma_box, properties);

    for (const auto& property : properties) {
      auto rot = std::dynamic_pointer_cast<Box_irot>(property.property);
      if (rot) {
        std::shared_ptr<HeifPixelImage> rotated_img;
        error = img->rotate_ccw(rot->get_rotation(), rotated_img);
        if (error) {
          return error;
        }

        img = rotated_img;
      }


      auto mirror = std::dynamic_pointer_cast<Box_imir>(property.property);
      if (mirror) {
        error = img->mirror_inplace(mirror->get_mirror_axis() == Box_imir::MirrorAxis::Horizontal);
        if (error) {
          return error;
        }
      }


      auto clap = std::dynamic_pointer_cast<Box_clap>(property.property);
      if (clap) {
        std::shared_ptr<HeifPixelImage> clap_img;

        int img_width = img->get_width();
        int img_height = img->get_height();
        assert(img_width >= 0);
        assert(img_height >= 0);

        int left = clap->left_rounded(img_width);
        int right = clap->right_rounded(img_width);
        int top = clap->top_rounded(img_height);
        int bottom = clap->bottom_rounded(img_height);

        if (left < 0) { left = 0; }
        if (top  < 0) { top  = 0; }

        if (right >= img_width) { right = img_width-1; }
        if (bottom >= img_height) { bottom = img_height-1; }

        if (left >= right ||
            top >= bottom) {
          return Error(heif_error_Invalid_input,
                       heif_suberror_Invalid_clean_aperture);
        }

        std::shared_ptr<HeifPixelImage> cropped_img;
        error = img->crop(left,right,top,bottom, cropped_img);
        if (error) {
          return error;
        }

        img = cropped_img;
      }
    }
  }

  return Error::Ok;
}