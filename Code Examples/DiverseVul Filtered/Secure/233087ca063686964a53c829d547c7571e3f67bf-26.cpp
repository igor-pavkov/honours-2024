static int raw_cmd_copyin(int cmd, void __user *param,
				 struct floppy_raw_cmd **rcmd)
{
	struct floppy_raw_cmd *ptr;
	int ret;

	*rcmd = NULL;

loop:
	ptr = kmalloc(sizeof(struct floppy_raw_cmd), GFP_KERNEL);
	if (!ptr)
		return -ENOMEM;
	*rcmd = ptr;
	ret = copy_from_user(ptr, param, sizeof(*ptr));
	ptr->next = NULL;
	ptr->buffer_length = 0;
	ptr->kernel_data = NULL;
	if (ret)
		return -EFAULT;
	param += sizeof(struct floppy_raw_cmd);
	if (ptr->cmd_count > FD_RAW_CMD_FULLSIZE)
		return -EINVAL;

	memset(ptr->reply, 0, FD_RAW_REPLY_SIZE);
	ptr->resultcode = 0;

	if (ptr->flags & (FD_RAW_READ | FD_RAW_WRITE)) {
		if (ptr->length <= 0 || ptr->length >= MAX_LEN)
			return -EINVAL;
		ptr->kernel_data = (char *)fd_dma_mem_alloc(ptr->length);
		fallback_on_nodma_alloc(&ptr->kernel_data, ptr->length);
		if (!ptr->kernel_data)
			return -ENOMEM;
		ptr->buffer_length = ptr->length;
	}
	if (ptr->flags & FD_RAW_WRITE) {
		ret = fd_copyin(ptr->data, ptr->kernel_data, ptr->length);
		if (ret)
			return ret;
	}

	if (ptr->flags & FD_RAW_MORE) {
		rcmd = &(ptr->next);
		ptr->rate &= 0x43;
		goto loop;
	}

	return 0;
}