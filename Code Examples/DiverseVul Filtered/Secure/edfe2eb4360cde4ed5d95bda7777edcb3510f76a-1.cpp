static inline int gic_get_current_cpu(GICState *s)
{
    if (!qtest_enabled() && s->num_cpu > 1) {
        return current_cpu->cpu_index;
    }
    return 0;
}