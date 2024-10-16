static void yv12_extend_frame_top_c(YV12_BUFFER_CONFIG *ybf) {
  int i;
  unsigned char *src_ptr1;
  unsigned char *dest_ptr1;

  unsigned int Border;
  int plane_stride;

  /***********/
  /* Y Plane */
  /***********/
  Border = ybf->border;
  plane_stride = ybf->y_stride;
  src_ptr1 = ybf->y_buffer - Border;
  dest_ptr1 = src_ptr1 - (Border * plane_stride);

  for (i = 0; i < (int)Border; ++i) {
    memcpy(dest_ptr1, src_ptr1, plane_stride);
    dest_ptr1 += plane_stride;
  }

  /***********/
  /* U Plane */
  /***********/
  plane_stride = ybf->uv_stride;
  Border /= 2;
  src_ptr1 = ybf->u_buffer - Border;
  dest_ptr1 = src_ptr1 - (Border * plane_stride);

  for (i = 0; i < (int)(Border); ++i) {
    memcpy(dest_ptr1, src_ptr1, plane_stride);
    dest_ptr1 += plane_stride;
  }

  /***********/
  /* V Plane */
  /***********/

  src_ptr1 = ybf->v_buffer - Border;
  dest_ptr1 = src_ptr1 - (Border * plane_stride);

  for (i = 0; i < (int)(Border); ++i) {
    memcpy(dest_ptr1, src_ptr1, plane_stride);
    dest_ptr1 += plane_stride;
  }
}