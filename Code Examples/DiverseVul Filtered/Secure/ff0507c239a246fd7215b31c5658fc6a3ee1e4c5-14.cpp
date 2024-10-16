static int iscsi_translate_sense(struct scsi_sense *sense)
{
    return scsi_sense_to_errno(sense->key,
                               (sense->ascq & 0xFF00) >> 8,
                               sense->ascq & 0xFF);
}