static void mig_sleep_cpu(void *opq)
{
    qemu_mutex_unlock_iothread();
    g_usleep(30*1000);
    qemu_mutex_lock_iothread();
}