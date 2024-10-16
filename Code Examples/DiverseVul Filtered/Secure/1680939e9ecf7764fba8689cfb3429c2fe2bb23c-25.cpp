static void rpmsg_remove(struct virtio_device *vdev)
{
	struct virtproc_info *vrp = vdev->priv;
	size_t total_buf_space = vrp->num_bufs * vrp->buf_size;
	int ret;

	virtio_reset_device(vdev);

	ret = device_for_each_child(&vdev->dev, NULL, rpmsg_remove_device);
	if (ret)
		dev_warn(&vdev->dev, "can't remove rpmsg device: %d\n", ret);

	idr_destroy(&vrp->endpoints);

	vdev->config->del_vqs(vrp->vdev);

	dma_free_coherent(vdev->dev.parent, total_buf_space,
			  vrp->rbufs, vrp->bufs_dma);

	kfree(vrp);
}