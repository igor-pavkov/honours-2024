static void manager_notify_finished(Manager *m) {
        char userspace[FORMAT_TIMESPAN_MAX], initrd[FORMAT_TIMESPAN_MAX], kernel[FORMAT_TIMESPAN_MAX], sum[FORMAT_TIMESPAN_MAX];
        usec_t firmware_usec, loader_usec, kernel_usec, initrd_usec, userspace_usec, total_usec;

        if (m->test_run)
                return;

        if (MANAGER_IS_SYSTEM(m) && detect_container() <= 0) {

                /* Note that m->kernel_usec.monotonic is always at 0,
                 * and m->firmware_usec.monotonic and
                 * m->loader_usec.monotonic should be considered
                 * negative values. */

                firmware_usec = m->firmware_timestamp.monotonic - m->loader_timestamp.monotonic;
                loader_usec = m->loader_timestamp.monotonic - m->kernel_timestamp.monotonic;
                userspace_usec = m->finish_timestamp.monotonic - m->userspace_timestamp.monotonic;
                total_usec = m->firmware_timestamp.monotonic + m->finish_timestamp.monotonic;

                if (dual_timestamp_is_set(&m->initrd_timestamp)) {

                        kernel_usec = m->initrd_timestamp.monotonic - m->kernel_timestamp.monotonic;
                        initrd_usec = m->userspace_timestamp.monotonic - m->initrd_timestamp.monotonic;

                        log_struct(LOG_INFO,
                                   LOG_MESSAGE_ID(SD_MESSAGE_STARTUP_FINISHED),
                                   "KERNEL_USEC="USEC_FMT, kernel_usec,
                                   "INITRD_USEC="USEC_FMT, initrd_usec,
                                   "USERSPACE_USEC="USEC_FMT, userspace_usec,
                                   LOG_MESSAGE("Startup finished in %s (kernel) + %s (initrd) + %s (userspace) = %s.",
                                               format_timespan(kernel, sizeof(kernel), kernel_usec, USEC_PER_MSEC),
                                               format_timespan(initrd, sizeof(initrd), initrd_usec, USEC_PER_MSEC),
                                               format_timespan(userspace, sizeof(userspace), userspace_usec, USEC_PER_MSEC),
                                               format_timespan(sum, sizeof(sum), total_usec, USEC_PER_MSEC)),
                                   NULL);
                } else {
                        kernel_usec = m->userspace_timestamp.monotonic - m->kernel_timestamp.monotonic;
                        initrd_usec = 0;

                        log_struct(LOG_INFO,
                                   LOG_MESSAGE_ID(SD_MESSAGE_STARTUP_FINISHED),
                                   "KERNEL_USEC="USEC_FMT, kernel_usec,
                                   "USERSPACE_USEC="USEC_FMT, userspace_usec,
                                   LOG_MESSAGE("Startup finished in %s (kernel) + %s (userspace) = %s.",
                                               format_timespan(kernel, sizeof(kernel), kernel_usec, USEC_PER_MSEC),
                                               format_timespan(userspace, sizeof(userspace), userspace_usec, USEC_PER_MSEC),
                                               format_timespan(sum, sizeof(sum), total_usec, USEC_PER_MSEC)),
                                   NULL);
                }
        } else {
                firmware_usec = loader_usec = initrd_usec = kernel_usec = 0;
                total_usec = userspace_usec = m->finish_timestamp.monotonic - m->userspace_timestamp.monotonic;

                log_struct(LOG_INFO,
                           LOG_MESSAGE_ID(SD_MESSAGE_STARTUP_FINISHED),
                           "USERSPACE_USEC="USEC_FMT, userspace_usec,
                           LOG_MESSAGE("Startup finished in %s.",
                                       format_timespan(sum, sizeof(sum), total_usec, USEC_PER_MSEC)),
                           NULL);
        }

        bus_manager_send_finished(m, firmware_usec, loader_usec, kernel_usec, initrd_usec, userspace_usec, total_usec);

        sd_notifyf(false,
                   "READY=1\n"
                   "STATUS=Startup finished in %s.",
                   format_timespan(sum, sizeof(sum), total_usec, USEC_PER_MSEC));
}