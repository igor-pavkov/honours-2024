void virtqueue_fill(VirtQueue *vq, const VirtQueueElement *elem,
                    unsigned int len, unsigned int idx)
{
    unsigned int offset;
    int i;

    trace_virtqueue_fill(vq, elem, len, idx);

    offset = 0;
    for (i = 0; i < elem->in_num; i++) {
        size_t size = MIN(len - offset, elem->in_sg[i].iov_len);

        cpu_physical_memory_unmap(elem->in_sg[i].iov_base,
                                  elem->in_sg[i].iov_len,
                                  1, size);

        offset += size;
    }

    for (i = 0; i < elem->out_num; i++)
        cpu_physical_memory_unmap(elem->out_sg[i].iov_base,
                                  elem->out_sg[i].iov_len,
                                  0, elem->out_sg[i].iov_len);

    idx = (idx + vring_used_idx(vq)) % vq->vring.num;

    /* Get a pointer to the next entry in the used ring. */
    vring_used_ring_id(vq, idx, elem->index);
    vring_used_ring_len(vq, idx, len);
}