static MemTxResult gic_thiscpu_hyp_read(void *opaque, hwaddr addr, uint64_t *data,
                                    unsigned size, MemTxAttrs attrs)
{
    GICState *s = (GICState *)opaque;

    return gic_hyp_read(s, gic_get_current_cpu(s), addr, data, attrs);
}