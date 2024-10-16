Error HeifContext::decode_overlay_image(heif_item_id ID,
                                        std::shared_ptr<HeifPixelImage>& img,
                                        const std::vector<uint8_t>& overlay_data) const
{
  // find the IDs this image is composed of

  auto iref_box = m_heif_file->get_iref_box();

  if (!iref_box) {
    return Error(heif_error_Invalid_input,
                 heif_suberror_No_iref_box,
                 "No iref box available, but needed for iovl image");
  }

  std::vector<heif_item_id> image_references = iref_box->get_references(ID, fourcc("dimg"));

  /* TODO: probably, it is valid that an iovl image has no references ?

  if (image_references.empty()) {
    return Error(heif_error_Invalid_input,
                 heif_suberror_Missing_grid_images,
                 "'iovl' image with more than one reference image");
  }
  */


  ImageOverlay overlay;
  Error err = overlay.parse(image_references.size(), overlay_data);
  if (err) {
    return err;
  }

  if (image_references.size() != overlay.get_num_offsets()) {
    return Error(heif_error_Invalid_input,
                 heif_suberror_Invalid_overlay_data,
                 "Number of image offsets does not match the number of image references");
  }

  uint32_t w = overlay.get_canvas_width();
  uint32_t h = overlay.get_canvas_height();

  if (w >= MAX_IMAGE_WIDTH || h >= MAX_IMAGE_HEIGHT) {
    std::stringstream sstr;
    sstr << "Image size " << w << "x" << h << " exceeds the maximum image size "
         << MAX_IMAGE_WIDTH << "x" << MAX_IMAGE_HEIGHT << "\n";

    return Error(heif_error_Memory_allocation_error,
                 heif_suberror_Security_limit_exceeded,
                 sstr.str());
  }

  // TODO: seems we always have to compose this in RGB since the background color is an RGB value
  img = std::make_shared<HeifPixelImage>();
  img->create(w,h,
              heif_colorspace_RGB,
              heif_chroma_444);
  img->add_plane(heif_channel_R,w,h,8); // TODO: other bit depths
  img->add_plane(heif_channel_G,w,h,8); // TODO: other bit depths
  img->add_plane(heif_channel_B,w,h,8); // TODO: other bit depths

  uint16_t bkg_color[4];
  overlay.get_background_color(bkg_color);

  err = img->fill_RGB_16bit(bkg_color[0], bkg_color[1], bkg_color[2], bkg_color[3]);
  if (err) {
    return err;
  }


  for (size_t i=0;i<image_references.size();i++) {
    std::shared_ptr<HeifPixelImage> overlay_img;
    err = decode_image(image_references[i], overlay_img);
    if (err != Error::Ok) {
      return err;
    }

    overlay_img = overlay_img->convert_colorspace(heif_colorspace_RGB, heif_chroma_444);
    if (!overlay_img) {
      return Error(heif_error_Unsupported_feature, heif_suberror_Unsupported_color_conversion);
    }

    int32_t dx,dy;
    overlay.get_offset(i, &dx,&dy);

    err = img->overlay(overlay_img, dx,dy);
    if (err) {
      if (err.error_code == heif_error_Invalid_input &&
          err.sub_error_code == heif_suberror_Overlay_image_outside_of_canvas) {
        // NOP, ignore this error

        err = Error::Ok;
      }
      else {
        return err;
      }
    }
  }

  return err;
}