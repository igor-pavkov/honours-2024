static void yv12_extend_frame_left_right_c(YV12_BUFFER_CONFIG *ybf,
                                           unsigned char *y_src,
                                           unsigned char *u_src,
                                           unsigned char *v_src) {
  int i;
  unsigned char *src_ptr1, *src_ptr2;
  unsigned char *dest_ptr1, *dest_ptr2;

  unsigned int Border;
  int plane_stride;
  int plane_height;
  int plane_width;

  /***********/
  /* Y Plane */
  /***********/
  Border = ybf->border;
  plane_stride = ybf->y_stride;
  plane_height = 16;
  plane_width = ybf->y_width;

  /* copy the left and right most columns out */
  src_ptr1 = y_src;
  src_ptr2 = src_ptr1 + plane_width - 1;
  dest_ptr1 = src_ptr1 - Border;
  dest_ptr2 = src_ptr2 + 1;

  for (i = 0; i < plane_height; ++i) {
    memset(dest_ptr1, src_ptr1[0], Border);
    memset(dest_ptr2, src_ptr2[0], Border);
    src_ptr1 += plane_stride;
    src_ptr2 += plane_stride;
    dest_ptr1 += plane_stride;
    dest_ptr2 += plane_stride;
  }

  /***********/
  /* U Plane */
  /***********/
  plane_stride = ybf->uv_stride;
  plane_height = 8;
  plane_width = ybf->uv_width;
  Border /= 2;

  /* copy the left and right most columns out */
  src_ptr1 = u_src;
  src_ptr2 = src_ptr1 + plane_width - 1;
  dest_ptr1 = src_ptr1 - Border;
  dest_ptr2 = src_ptr2 + 1;

  for (i = 0; i < plane_height; ++i) {
    memset(dest_ptr1, src_ptr1[0], Border);
    memset(dest_ptr2, src_ptr2[0], Border);
    src_ptr1 += plane_stride;
    src_ptr2 += plane_stride;
    dest_ptr1 += plane_stride;
    dest_ptr2 += plane_stride;
  }

  /***********/
  /* V Plane */
  /***********/

  /* copy the left and right most columns out */
  src_ptr1 = v_src;
  src_ptr2 = src_ptr1 + plane_width - 1;
  dest_ptr1 = src_ptr1 - Border;
  dest_ptr2 = src_ptr2 + 1;

  for (i = 0; i < plane_height; ++i) {
    memset(dest_ptr1, src_ptr1[0], Border);
    memset(dest_ptr2, src_ptr2[0], Border);
    src_ptr1 += plane_stride;
    src_ptr2 += plane_stride;
    dest_ptr1 += plane_stride;
    dest_ptr2 += plane_stride;
  }
}