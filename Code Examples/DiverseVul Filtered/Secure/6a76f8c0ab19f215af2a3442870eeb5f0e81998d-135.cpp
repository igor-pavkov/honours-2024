int ftrace_regex_release(struct inode *inode, struct file *file)
{
	struct seq_file *m = (struct seq_file *)file->private_data;
	struct ftrace_iterator *iter;
	struct ftrace_hash **orig_hash;
	struct trace_parser *parser;
	int filter_hash;
	int ret;

	mutex_lock(&ftrace_regex_lock);
	if (file->f_mode & FMODE_READ) {
		iter = m->private;

		seq_release(inode, file);
	} else
		iter = file->private_data;

	parser = &iter->parser;
	if (trace_parser_loaded(parser)) {
		parser->buffer[parser->idx] = 0;
		ftrace_match_records(iter->hash, parser->buffer, parser->idx);
	}

	trace_parser_put(parser);

	if (file->f_mode & FMODE_WRITE) {
		filter_hash = !!(iter->flags & FTRACE_ITER_FILTER);

		if (filter_hash)
			orig_hash = &iter->ops->filter_hash;
		else
			orig_hash = &iter->ops->notrace_hash;

		mutex_lock(&ftrace_lock);
		ret = ftrace_hash_move(iter->ops, filter_hash,
				       orig_hash, iter->hash);
		if (!ret && (iter->ops->flags & FTRACE_OPS_FL_ENABLED)
		    && ftrace_enabled)
			ftrace_run_update_code(FTRACE_UPDATE_CALLS);

		mutex_unlock(&ftrace_lock);
	}
	free_ftrace_hash(iter->hash);
	kfree(iter);

	mutex_unlock(&ftrace_regex_lock);
	return 0;
}