static void virtio_scsi_set_config(VirtIODevice *vdev,
                                   const uint8_t *config)
{
    VirtIOSCSIConfig *scsiconf = (VirtIOSCSIConfig *)config;
    VirtIOSCSICommon *vs = VIRTIO_SCSI_COMMON(vdev);

    if ((uint32_t) ldl_raw(&scsiconf->sense_size) >= 65536 ||
        (uint32_t) ldl_raw(&scsiconf->cdb_size) >= 256) {
        error_report("bad data written to virtio-scsi configuration space");
        exit(1);
    }

    vs->sense_size = ldl_raw(&scsiconf->sense_size);
    vs->cdb_size = ldl_raw(&scsiconf->cdb_size);
}