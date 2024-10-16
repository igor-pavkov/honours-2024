void decoder_context::set_image_allocation_functions(de265_image_allocation* allocfunc,
                                                     void* userdata)
{
  if (allocfunc) {
    param_image_allocation_functions = *allocfunc;
    param_image_allocation_userdata  = userdata;
  }
  else {
    assert(false); // actually, it makes no sense to reset the allocation functions

    param_image_allocation_functions = de265_image::default_image_allocation;
    param_image_allocation_userdata  = NULL;
  }
}