Error HeifContext::encode_thumbnail(std::shared_ptr<HeifPixelImage> image,
                                    struct heif_encoder* encoder,
                                    const struct heif_encoding_options* options,
                                    int bbox_size,
                                    std::shared_ptr<Image>& out_thumbnail_handle)
{
  Error error;

  int orig_width  = image->get_width();
  int orig_height = image->get_height();

  int thumb_width, thumb_height;

  if (orig_width <= bbox_size && orig_height <= bbox_size) {
    // original image is smaller than thumbnail size -> do not encode any thumbnail

    out_thumbnail_handle.reset();
    return Error::Ok;
  }
  else if (orig_width > orig_height) {
    thumb_height = orig_height * bbox_size / orig_width;
    thumb_width  = bbox_size;
  }
  else {
    thumb_width  = orig_width * bbox_size / orig_height;
    thumb_height = bbox_size;
  }


  // round size to even width and height

  thumb_width  &= ~1;
  thumb_height &= ~1;


  std::shared_ptr<HeifPixelImage> thumbnail_image;
  error = image->scale_nearest_neighbor(thumbnail_image, thumb_width, thumb_height);
  if (error) {
    return error;
  }

  error = encode_image(thumbnail_image,
                       encoder, options,
                       heif_image_input_class_thumbnail,
                       out_thumbnail_handle);
  if (error) {
    return error;
  }

  return error;
}