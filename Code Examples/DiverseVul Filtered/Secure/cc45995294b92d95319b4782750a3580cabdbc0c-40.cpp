static unsigned virtqueue_next_desc(hwaddr desc_pa,
                                    unsigned int i, unsigned int max)
{
    unsigned int next;

    /* If this descriptor says it doesn't chain, we're done. */
    if (!(vring_desc_flags(desc_pa, i) & VRING_DESC_F_NEXT))
        return max;

    /* Check they're not leading us off end of descriptors. */
    next = vring_desc_next(desc_pa, i);
    /* Make sure compiler knows to grab that: we don't want it changing! */
    smp_wmb();

    if (next >= max) {
        error_report("Desc next is %u", next);
        exit(1);
    }

    return next;
}