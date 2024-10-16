void virtio_queue_notify(VirtIODevice *vdev, int n)
{
    virtio_queue_notify_vq(&vdev->vq[n]);
}