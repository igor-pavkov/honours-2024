Error HeifContext::decode_and_paste_tile_image(heif_item_id tileID,
                                               std::shared_ptr<HeifPixelImage> img,
                                               int x0,int y0) const
{
  std::shared_ptr<HeifPixelImage> tile_img;

  Error err = decode_image(tileID, tile_img);
  if (err != Error::Ok) {
    return err;
  }


  const int w = img->get_width();
  const int h = img->get_height();


  // --- copy tile into output image

  int src_width  = tile_img->get_width();
  int src_height = tile_img->get_height();
  assert(src_width >= 0);
  assert(src_height >= 0);

  heif_chroma chroma = img->get_chroma_format();
  std::set<enum heif_channel> channels = img->get_channel_set();

  if (chroma != tile_img->get_chroma_format()) {
      return Error(heif_error_Invalid_input,
                   heif_suberror_Wrong_tile_image_chroma_format,
                   "Image tile has different chroma format than combined image");
  }

  for (heif_channel channel : channels) {
    int tile_stride;
    uint8_t* tile_data = tile_img->get_plane(channel, &tile_stride);

    int out_stride;
    uint8_t* out_data = img->get_plane(channel, &out_stride);

    if (w <= x0 || h <= y0) {
      return Error(heif_error_Invalid_input,
                   heif_suberror_Invalid_grid_data);
    }

    int copy_width  = std::min(src_width, w - x0);
    int copy_height = std::min(src_height, h - y0);

    int xs=x0, ys=y0;

    if (channel != heif_channel_Y) {
      int subH = chroma_h_subsampling(chroma);
      int subV = chroma_v_subsampling(chroma);
      copy_width /= subH;
      copy_height /= subV;
      xs /= subH;
      ys /= subV;
    }

    for (int py=0;py<copy_height;py++) {
      memcpy(out_data + xs + (ys+py)*out_stride,
             tile_data + py*tile_stride,
             copy_width);
    }
  }

  return Error::Ok;
}