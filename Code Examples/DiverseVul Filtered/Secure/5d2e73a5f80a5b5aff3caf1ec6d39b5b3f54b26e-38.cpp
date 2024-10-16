static int buffer_setup(struct videobuf_queue *vq, unsigned int *count,
			unsigned int *size)
{
	struct zr364xx_camera *cam = vq->priv_data;

	*size = cam->width * cam->height * (cam->fmt->depth >> 3);

	if (*count == 0)
		*count = ZR364XX_DEF_BUFS;

	if (*size * *count > ZR364XX_DEF_BUFS * 1024 * 1024)
		*count = (ZR364XX_DEF_BUFS * 1024 * 1024) / *size;

	return 0;
}