static void mig_throttle_guest_down(void)
{
    CPUState *cpu;

    qemu_mutex_lock_iothread();
    CPU_FOREACH(cpu) {
        async_run_on_cpu(cpu, mig_sleep_cpu, NULL);
    }
    qemu_mutex_unlock_iothread();
}