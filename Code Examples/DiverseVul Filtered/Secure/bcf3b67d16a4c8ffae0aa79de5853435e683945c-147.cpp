inline int megasas_cmd_type(struct scsi_cmnd *cmd)
{
	int ret;

	switch (cmd->cmnd[0]) {
	case READ_10:
	case WRITE_10:
	case READ_12:
	case WRITE_12:
	case READ_6:
	case WRITE_6:
	case READ_16:
	case WRITE_16:
		ret = (MEGASAS_IS_LOGICAL(cmd->device)) ?
			READ_WRITE_LDIO : READ_WRITE_SYSPDIO;
		break;
	default:
		ret = (MEGASAS_IS_LOGICAL(cmd->device)) ?
			NON_READ_WRITE_LDIO : NON_READ_WRITE_SYSPDIO;
	}
	return ret;
}