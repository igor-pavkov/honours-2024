static int vidioc_g_std(struct file *file, void *private_data, v4l2_std_id *norm)
{
	if (norm)
		*norm = V4L2_STD_ALL;
	return 0;
}