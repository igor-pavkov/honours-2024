static void sdhci_resume_pending_transfer(SDHCIState *s)
{
    timer_del(s->transfer_timer);
    sdhci_data_transfer(s);
}