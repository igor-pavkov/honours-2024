static bool sdhci_pending_insert_vmstate_needed(void *opaque)
{
    SDHCIState *s = opaque;

    return s->pending_insert_state;
}