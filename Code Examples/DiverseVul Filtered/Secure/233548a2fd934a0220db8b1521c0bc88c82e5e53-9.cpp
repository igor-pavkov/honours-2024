static void uvc_unregister_video(struct uvc_device *dev)
{
	if (dev->video.vdev) {
		if (dev->video.vdev->minor == -1)
			video_device_release(dev->video.vdev);
		else
			video_unregister_device(dev->video.vdev);
		dev->video.vdev = NULL;
	}
}