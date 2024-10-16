Error HeifContext::decode_derived_image(heif_item_id ID,
                                        std::shared_ptr<HeifPixelImage>& img) const
{
  // find the ID of the image this image is derived from

  auto iref_box = m_heif_file->get_iref_box();

  if (!iref_box) {
    return Error(heif_error_Invalid_input,
                 heif_suberror_No_iref_box,
                 "No iref box available, but needed for iden image");
  }

  std::vector<heif_item_id> image_references = iref_box->get_references(ID, fourcc("dimg"));

  if ((int)image_references.size() != 1) {
    return Error(heif_error_Invalid_input,
                 heif_suberror_Missing_grid_images,
                 "'iden' image with more than one reference image");
  }


  heif_item_id reference_image_id = image_references[0];


  Error error = decode_image(reference_image_id, img);
  return error;
}