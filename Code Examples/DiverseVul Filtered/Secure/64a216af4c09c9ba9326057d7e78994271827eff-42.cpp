static int vidioc_querystd(struct file *file, void *private_data, v4l2_std_id *norm)
{
	if (norm)
		*norm = V4L2_STD_ALL;
	return 0;
}