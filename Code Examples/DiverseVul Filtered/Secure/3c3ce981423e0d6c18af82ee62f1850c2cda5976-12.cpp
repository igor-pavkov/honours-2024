static void virtio_scsi_get_config(VirtIODevice *vdev,
                                   uint8_t *config)
{
    VirtIOSCSIConfig *scsiconf = (VirtIOSCSIConfig *)config;
    VirtIOSCSICommon *s = VIRTIO_SCSI_COMMON(vdev);

    stl_raw(&scsiconf->num_queues, s->conf.num_queues);
    stl_raw(&scsiconf->seg_max, 128 - 2);
    stl_raw(&scsiconf->max_sectors, s->conf.max_sectors);
    stl_raw(&scsiconf->cmd_per_lun, s->conf.cmd_per_lun);
    stl_raw(&scsiconf->event_info_size, sizeof(VirtIOSCSIEvent));
    stl_raw(&scsiconf->sense_size, s->sense_size);
    stl_raw(&scsiconf->cdb_size, s->cdb_size);
    stw_raw(&scsiconf->max_channel, VIRTIO_SCSI_MAX_CHANNEL);
    stw_raw(&scsiconf->max_target, VIRTIO_SCSI_MAX_TARGET);
    stl_raw(&scsiconf->max_lun, VIRTIO_SCSI_MAX_LUN);
}