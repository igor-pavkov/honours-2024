static int iscsi_open(BlockDriverState *bs, QDict *options, int flags,
                      Error **errp)
{
    IscsiLun *iscsilun = bs->opaque;
    struct iscsi_context *iscsi = NULL;
    struct scsi_task *task = NULL;
    struct scsi_inquiry_standard *inq = NULL;
    struct scsi_inquiry_supported_pages *inq_vpd;
    char *initiator_name = NULL;
    QemuOpts *opts;
    Error *local_err = NULL;
    const char *transport_name, *portal, *target;
#if LIBISCSI_API_VERSION >= (20160603)
    enum iscsi_transport_type transport;
#endif
    int i, ret = 0, timeout = 0, lun;

    opts = qemu_opts_create(&runtime_opts, NULL, 0, &error_abort);
    qemu_opts_absorb_qdict(opts, options, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        ret = -EINVAL;
        goto out;
    }

    transport_name = qemu_opt_get(opts, "transport");
    portal = qemu_opt_get(opts, "portal");
    target = qemu_opt_get(opts, "target");
    lun = qemu_opt_get_number(opts, "lun", 0);

    if (!transport_name || !portal || !target) {
        error_setg(errp, "Need all of transport, portal and target options");
        ret = -EINVAL;
        goto out;
    }

    if (!strcmp(transport_name, "tcp")) {
#if LIBISCSI_API_VERSION >= (20160603)
        transport = TCP_TRANSPORT;
    } else if (!strcmp(transport_name, "iser")) {
        transport = ISER_TRANSPORT;
#else
        /* TCP is what older libiscsi versions always use */
#endif
    } else {
        error_setg(errp, "Unknown transport: %s", transport_name);
        ret = -EINVAL;
        goto out;
    }

    memset(iscsilun, 0, sizeof(IscsiLun));

    initiator_name = get_initiator_name(opts);

    iscsi = iscsi_create_context(initiator_name);
    if (iscsi == NULL) {
        error_setg(errp, "iSCSI: Failed to create iSCSI context.");
        ret = -ENOMEM;
        goto out;
    }
#if LIBISCSI_API_VERSION >= (20160603)
    if (iscsi_init_transport(iscsi, transport)) {
        error_setg(errp, ("Error initializing transport."));
        ret = -EINVAL;
        goto out;
    }
#endif
    if (iscsi_set_targetname(iscsi, target)) {
        error_setg(errp, "iSCSI: Failed to set target name.");
        ret = -EINVAL;
        goto out;
    }

    /* check if we got CHAP username/password via the options */
    apply_chap(iscsi, opts, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        ret = -EINVAL;
        goto out;
    }

    if (iscsi_set_session_type(iscsi, ISCSI_SESSION_NORMAL) != 0) {
        error_setg(errp, "iSCSI: Failed to set session type to normal.");
        ret = -EINVAL;
        goto out;
    }

    /* check if we got HEADER_DIGEST via the options */
    apply_header_digest(iscsi, opts, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        ret = -EINVAL;
        goto out;
    }

    /* timeout handling is broken in libiscsi before 1.15.0 */
    timeout = qemu_opt_get_number(opts, "timeout", 0);
#if LIBISCSI_API_VERSION >= 20150621
    iscsi_set_timeout(iscsi, timeout);
#else
    if (timeout) {
        warn_report("iSCSI: ignoring timeout value for libiscsi <1.15.0");
    }
#endif

    if (iscsi_full_connect_sync(iscsi, portal, lun) != 0) {
        error_setg(errp, "iSCSI: Failed to connect to LUN : %s",
            iscsi_get_error(iscsi));
        ret = -EINVAL;
        goto out;
    }

    iscsilun->iscsi = iscsi;
    iscsilun->aio_context = bdrv_get_aio_context(bs);
    iscsilun->lun = lun;
    iscsilun->has_write_same = true;

    task = iscsi_do_inquiry(iscsilun->iscsi, iscsilun->lun, 0, 0,
                            (void **) &inq, errp);
    if (task == NULL) {
        ret = -EINVAL;
        goto out;
    }
    iscsilun->type = inq->periperal_device_type;
    scsi_free_scsi_task(task);
    task = NULL;

    iscsi_modesense_sync(iscsilun);
    if (iscsilun->dpofua) {
        bs->supported_write_flags = BDRV_REQ_FUA;
    }

    /* Check the write protect flag of the LUN if we want to write */
    if (iscsilun->type == TYPE_DISK && (flags & BDRV_O_RDWR) &&
        iscsilun->write_protected) {
        ret = bdrv_apply_auto_read_only(bs, "LUN is write protected", errp);
        if (ret < 0) {
            goto out;
        }
        flags &= ~BDRV_O_RDWR;
    }

    iscsi_readcapacity_sync(iscsilun, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        ret = -EINVAL;
        goto out;
    }
    bs->total_sectors = sector_lun2qemu(iscsilun->num_blocks, iscsilun);

    /* We don't have any emulation for devices other than disks and CD-ROMs, so
     * this must be sg ioctl compatible. We force it to be sg, otherwise qemu
     * will try to read from the device to guess the image format.
     */
    if (iscsilun->type != TYPE_DISK && iscsilun->type != TYPE_ROM) {
        bs->sg = true;
    }

    task = iscsi_do_inquiry(iscsilun->iscsi, iscsilun->lun, 1,
                            SCSI_INQUIRY_PAGECODE_SUPPORTED_VPD_PAGES,
                            (void **) &inq_vpd, errp);
    if (task == NULL) {
        ret = -EINVAL;
        goto out;
    }
    for (i = 0; i < inq_vpd->num_pages; i++) {
        struct scsi_task *inq_task;
        struct scsi_inquiry_logical_block_provisioning *inq_lbp;
        struct scsi_inquiry_block_limits *inq_bl;
        struct scsi_inquiry_device_identification *inq_di;
        switch (inq_vpd->pages[i]) {
        case SCSI_INQUIRY_PAGECODE_LOGICAL_BLOCK_PROVISIONING:
            inq_task = iscsi_do_inquiry(iscsilun->iscsi, iscsilun->lun, 1,
                                        SCSI_INQUIRY_PAGECODE_LOGICAL_BLOCK_PROVISIONING,
                                        (void **) &inq_lbp, errp);
            if (inq_task == NULL) {
                ret = -EINVAL;
                goto out;
            }
            memcpy(&iscsilun->lbp, inq_lbp,
                   sizeof(struct scsi_inquiry_logical_block_provisioning));
            scsi_free_scsi_task(inq_task);
            break;
        case SCSI_INQUIRY_PAGECODE_BLOCK_LIMITS:
            inq_task = iscsi_do_inquiry(iscsilun->iscsi, iscsilun->lun, 1,
                                    SCSI_INQUIRY_PAGECODE_BLOCK_LIMITS,
                                    (void **) &inq_bl, errp);
            if (inq_task == NULL) {
                ret = -EINVAL;
                goto out;
            }
            memcpy(&iscsilun->bl, inq_bl,
                   sizeof(struct scsi_inquiry_block_limits));
            scsi_free_scsi_task(inq_task);
            break;
        case SCSI_INQUIRY_PAGECODE_DEVICE_IDENTIFICATION:
            inq_task = iscsi_do_inquiry(iscsilun->iscsi, iscsilun->lun, 1,
                                    SCSI_INQUIRY_PAGECODE_DEVICE_IDENTIFICATION,
                                    (void **) &inq_di, errp);
            if (inq_task == NULL) {
                ret = -EINVAL;
                goto out;
            }
            iscsi_save_designator(iscsilun, inq_di);
            scsi_free_scsi_task(inq_task);
            break;
        default:
            break;
        }
    }
    scsi_free_scsi_task(task);
    task = NULL;

    qemu_mutex_init(&iscsilun->mutex);
    iscsi_attach_aio_context(bs, iscsilun->aio_context);

    /* Guess the internal cluster (page) size of the iscsi target by the means
     * of opt_unmap_gran. Transfer the unmap granularity only if it has a
     * reasonable size */
    if (iscsilun->bl.opt_unmap_gran * iscsilun->block_size >= 4 * 1024 &&
        iscsilun->bl.opt_unmap_gran * iscsilun->block_size <= 16 * 1024 * 1024) {
        iscsilun->cluster_size = iscsilun->bl.opt_unmap_gran *
            iscsilun->block_size;
        if (iscsilun->lbprz) {
            ret = iscsi_allocmap_init(iscsilun, flags);
        }
    }

    if (iscsilun->lbprz && iscsilun->lbp.lbpws) {
        bs->supported_zero_flags = BDRV_REQ_MAY_UNMAP;
    }

out:
    qemu_opts_del(opts);
    g_free(initiator_name);
    if (task != NULL) {
        scsi_free_scsi_task(task);
    }

    if (ret) {
        if (iscsi != NULL) {
            if (iscsi_is_logged_in(iscsi)) {
                iscsi_logout_sync(iscsi);
            }
            iscsi_destroy_context(iscsi);
        }
        memset(iscsilun, 0, sizeof(IscsiLun));
    }

    return ret;
}