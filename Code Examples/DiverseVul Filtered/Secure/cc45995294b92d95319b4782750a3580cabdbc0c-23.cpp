static void virtqueue_init(VirtQueue *vq)
{
    hwaddr pa = vq->pa;

    vq->vring.desc = pa;
    vq->vring.avail = pa + vq->vring.num * sizeof(VRingDesc);
    vq->vring.used = vring_align(vq->vring.avail +
                                 offsetof(VRingAvail, ring[vq->vring.num]),
                                 vq->vring.align);
}