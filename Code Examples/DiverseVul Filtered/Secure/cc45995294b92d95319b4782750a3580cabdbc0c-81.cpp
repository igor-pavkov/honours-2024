hwaddr virtio_queue_get_used_size(VirtIODevice *vdev, int n)
{
    return offsetof(VRingUsed, ring) +
        sizeof(VRingUsedElem) * vdev->vq[n].vring.num;
}