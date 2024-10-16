Error HeifContext::decode_full_grid_image(heif_item_id ID,
                                          std::shared_ptr<HeifPixelImage>& img,
                                          const std::vector<uint8_t>& grid_data) const
{
  ImageGrid grid;
  Error err = grid.parse(grid_data);
  if (err) {
    return err;
  }
  // std::cout << grid.dump();


  auto iref_box = m_heif_file->get_iref_box();

  if (!iref_box) {
    return Error(heif_error_Invalid_input,
                 heif_suberror_No_iref_box,
                 "No iref box available, but needed for grid image");
  }

  std::vector<heif_item_id> image_references = iref_box->get_references(ID, fourcc("dimg"));

  if ((int)image_references.size() != grid.get_rows() * grid.get_columns()) {
    std::stringstream sstr;
    sstr << "Tiled image with " << grid.get_rows() << "x" <<  grid.get_columns() << "="
         << (grid.get_rows() * grid.get_columns()) << " tiles, but only "
         << image_references.size() << " tile images in file";

    return Error(heif_error_Invalid_input,
                 heif_suberror_Missing_grid_images,
                 sstr.str());
  }


  // --- check that all image IDs are valid images

  for (heif_item_id tile_id : image_references) {
    if (!is_image(tile_id)) {
      std::stringstream sstr;
      sstr << "Tile image ID=" << tile_id << " is not a proper image.";

      return Error(heif_error_Invalid_input,
                   heif_suberror_Missing_grid_images,
                   sstr.str());
    }
  }


  const int w = grid.get_width();
  const int h = grid.get_height();
  const int bpp = 8; // TODO: how do we know ?


  // --- determine output image chroma size and make sure all tiles have same chroma

  assert(!image_references.empty());
  heif_item_id some_tile_id = image_references[0];
  heif_chroma tile_chroma = m_heif_file->get_image_chroma_from_configuration(some_tile_id);

  const int cw = w/chroma_h_subsampling(tile_chroma);
  const int ch = h/chroma_v_subsampling(tile_chroma);

  for (heif_item_id tile_id : image_references) {
    if (m_heif_file->get_image_chroma_from_configuration(tile_id) != tile_chroma) {
      return Error(heif_error_Invalid_input,
                   heif_suberror_Invalid_grid_data,
                   "Images in grid do not all have the same chroma format.");
    }
  }


  // --- generate image of full output size

  if (w >= MAX_IMAGE_WIDTH || h >= MAX_IMAGE_HEIGHT) {
    std::stringstream sstr;
    sstr << "Image size " << w << "x" << h << " exceeds the maximum image size "
         << MAX_IMAGE_WIDTH << "x" << MAX_IMAGE_HEIGHT << "\n";

    return Error(heif_error_Memory_allocation_error,
                 heif_suberror_Security_limit_exceeded,
                 sstr.str());
  }


  img = std::make_shared<HeifPixelImage>();
  img->create(w,h,
              heif_colorspace_YCbCr, // TODO: how do we know ?
              tile_chroma);

  img->add_plane(heif_channel_Y,  w,h, bpp);

  if (tile_chroma != heif_chroma_monochrome) {
    img->add_plane(heif_channel_Cb, cw,ch, bpp);
    img->add_plane(heif_channel_Cr, cw,ch, bpp);
  }


  int y0=0;
  int reference_idx = 0;

#if ENABLE_PARALLEL_TILE_DECODING
  // remember which tile to put where into the image
  struct tile_data {
    heif_item_id tileID;
    int x_origin,y_origin;
  };

  std::deque<tile_data> tiles;
  tiles.resize(grid.get_rows() * grid.get_columns() );

  std::deque<std::future<Error> > errs;
#endif

  for (int y=0;y<grid.get_rows();y++) {
    int x0=0;
    int tile_height=0;

    for (int x=0;x<grid.get_columns();x++) {

      heif_item_id tileID = image_references[reference_idx];

      auto iter = m_all_images.find(tileID);
      if (iter == m_all_images.end()) {
        return Error(heif_error_Invalid_input,
                     heif_suberror_Missing_grid_images,
                     "Unexisting grid image referenced");
      }

      const std::shared_ptr<Image> tileImg = iter->second;
      int src_width = tileImg->get_width();
      int src_height = tileImg->get_height();

#if ENABLE_PARALLEL_TILE_DECODING
      tiles[x+y*grid.get_columns()] = tile_data { tileID, x0,y0 };
#else
      Error err = decode_and_paste_tile_image(tileID, img, x0,y0);
      if (err) {
        return err;
      }
#endif

      x0 += src_width;
      tile_height = src_height; // TODO: check that all tiles have the same height

      reference_idx++;
    }

    y0 += tile_height;
  }

#if ENABLE_PARALLEL_TILE_DECODING
  // Process all tiles in a set of background threads.
  // Do not start more than the maximum number of threads.

  while (tiles.empty()==false) {

    // If maximum number of threads running, wait until first thread finishes

    if (errs.size() >= (size_t)m_max_decoding_threads) {
      Error e = errs.front().get();
      if (e) {
        return e;
      }

      errs.pop_front();
    }


    // Start a new decoding thread

    tile_data data = tiles.front();
    tiles.pop_front();

    errs.push_back( std::async(std::launch::async,
                               &HeifContext::decode_and_paste_tile_image, this,
                               data.tileID, img, data.x_origin,data.y_origin) );
  }

  // check for decoding errors in remaining tiles

  while (errs.empty() == false) {
    Error e = errs.front().get();
    if (e) {
      return e;
    }

    errs.pop_front();
  }
#endif

  return Error::Ok;
}