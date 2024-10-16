static __poll_t zr364xx_poll(struct file *file,
			       struct poll_table_struct *wait)
{
	struct zr364xx_camera *cam = video_drvdata(file);
	struct videobuf_queue *q = &cam->vb_vidq;
	__poll_t res = v4l2_ctrl_poll(file, wait);

	_DBG("%s\n", __func__);

	return res | videobuf_poll_stream(file, q, wait);
}