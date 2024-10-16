static void yv12_extend_frame_bottom_c(YV12_BUFFER_CONFIG *ybf) {
  int i;
  unsigned char *src_ptr1, *src_ptr2;
  unsigned char *dest_ptr2;

  unsigned int Border;
  int plane_stride;
  int plane_height;

  /***********/
  /* Y Plane */
  /***********/
  Border = ybf->border;
  plane_stride = ybf->y_stride;
  plane_height = ybf->y_height;

  src_ptr1 = ybf->y_buffer - Border;
  src_ptr2 = src_ptr1 + (plane_height * plane_stride) - plane_stride;
  dest_ptr2 = src_ptr2 + plane_stride;

  for (i = 0; i < (int)Border; ++i) {
    memcpy(dest_ptr2, src_ptr2, plane_stride);
    dest_ptr2 += plane_stride;
  }

  /***********/
  /* U Plane */
  /***********/
  plane_stride = ybf->uv_stride;
  plane_height = ybf->uv_height;
  Border /= 2;

  src_ptr1 = ybf->u_buffer - Border;
  src_ptr2 = src_ptr1 + (plane_height * plane_stride) - plane_stride;
  dest_ptr2 = src_ptr2 + plane_stride;

  for (i = 0; i < (int)(Border); ++i) {
    memcpy(dest_ptr2, src_ptr2, plane_stride);
    dest_ptr2 += plane_stride;
  }

  /***********/
  /* V Plane */
  /***********/

  src_ptr1 = ybf->v_buffer - Border;
  src_ptr2 = src_ptr1 + (plane_height * plane_stride) - plane_stride;
  dest_ptr2 = src_ptr2 + plane_stride;

  for (i = 0; i < (int)(Border); ++i) {
    memcpy(dest_ptr2, src_ptr2, plane_stride);
    dest_ptr2 += plane_stride;
  }
}