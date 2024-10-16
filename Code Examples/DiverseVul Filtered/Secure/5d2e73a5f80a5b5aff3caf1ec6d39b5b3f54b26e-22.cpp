static int zr364xx_vidioc_s_fmt_vid_cap(struct file *file, void *priv,
				    struct v4l2_format *f)
{
	struct zr364xx_camera *cam = video_drvdata(file);
	struct videobuf_queue *q = &cam->vb_vidq;
	char pixelformat_name[5];
	int ret = zr364xx_vidioc_try_fmt_vid_cap(file, cam, f);
	int i;

	if (ret < 0)
		return ret;

	mutex_lock(&q->vb_lock);

	if (videobuf_queue_is_busy(&cam->vb_vidq)) {
		DBG("%s queue busy\n", __func__);
		ret = -EBUSY;
		goto out;
	}

	if (cam->owner) {
		DBG("%s can't change format after started\n", __func__);
		ret = -EBUSY;
		goto out;
	}

	cam->width = f->fmt.pix.width;
	cam->height = f->fmt.pix.height;
	DBG("%s: %dx%d mode selected\n", __func__,
		 cam->width, cam->height);
	f->fmt.pix.bytesperline = f->fmt.pix.width * 2;
	f->fmt.pix.sizeimage = f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_JPEG;
	cam->vb_vidq.field = f->fmt.pix.field;

	if (f->fmt.pix.width == 160 && f->fmt.pix.height == 120)
		mode = 1;
	else if (f->fmt.pix.width == 640 && f->fmt.pix.height == 480)
		mode = 2;
	else
		mode = 0;

	m0d1[0] = mode;
	m1[2].value = 0xf000 + mode;
	m2[1].value = 0xf000 + mode;

	/* special case for METHOD3, the modes are different */
	if (cam->method == METHOD3) {
		switch (mode) {
		case 1:
			m2[1].value = 0xf000 + 4;
			break;
		case 2:
			m2[1].value = 0xf000 + 0;
			break;
		default:
			m2[1].value = 0xf000 + 1;
			break;
		}
	}

	header2[437] = cam->height / 256;
	header2[438] = cam->height % 256;
	header2[439] = cam->width / 256;
	header2[440] = cam->width % 256;

	for (i = 0; init[cam->method][i].size != -1; i++) {
		ret =
		    send_control_msg(cam->udev, 1, init[cam->method][i].value,
				     0, init[cam->method][i].bytes,
				     init[cam->method][i].size);
		if (ret < 0) {
			dev_err(&cam->udev->dev,
			   "error during resolution change sequence: %d\n", i);
			goto out;
		}
	}

	/* Added some delay here, since opening/closing the camera quickly,
	 * like Ekiga does during its startup, can crash the webcam
	 */
	mdelay(100);
	cam->skip = 2;
	ret = 0;

out:
	mutex_unlock(&q->vb_lock);

	DBG("%s: V4L2_PIX_FMT_%s (%d) ok!\n", __func__,
	    decode_fourcc(f->fmt.pix.pixelformat, pixelformat_name),
	    f->fmt.pix.field);
	return ret;
}