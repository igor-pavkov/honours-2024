struct heif_error heif_encoder::alloc()
{
  if (encoder == nullptr) {
    struct heif_error error = plugin->new_encoder(&encoder);
    // TODO: error handling
    return error;
  }

  struct heif_error err = { heif_error_Ok, heif_suberror_Unspecified, kSuccess };
  return err;
}