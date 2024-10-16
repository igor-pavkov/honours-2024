static void snd_pcm_oss_proc_write(struct snd_info_entry *entry,
				   struct snd_info_buffer *buffer)
{
	struct snd_pcm_str *pstr = entry->private_data;
	char line[128], str[32], task_name[32];
	const char *ptr;
	int idx1;
	struct snd_pcm_oss_setup *setup, *setup1, template;

	while (!snd_info_get_line(buffer, line, sizeof(line))) {
		mutex_lock(&pstr->oss.setup_mutex);
		memset(&template, 0, sizeof(template));
		ptr = snd_info_get_str(task_name, line, sizeof(task_name));
		if (!strcmp(task_name, "clear") || !strcmp(task_name, "erase")) {
			snd_pcm_oss_proc_free_setup_list(pstr);
			mutex_unlock(&pstr->oss.setup_mutex);
			continue;
		}
		for (setup = pstr->oss.setup_list; setup; setup = setup->next) {
			if (!strcmp(setup->task_name, task_name)) {
				template = *setup;
				break;
			}
		}
		ptr = snd_info_get_str(str, ptr, sizeof(str));
		template.periods = simple_strtoul(str, NULL, 10);
		ptr = snd_info_get_str(str, ptr, sizeof(str));
		template.period_size = simple_strtoul(str, NULL, 10);
		for (idx1 = 31; idx1 >= 0; idx1--)
			if (template.period_size & (1 << idx1))
				break;
		for (idx1--; idx1 >= 0; idx1--)
			template.period_size &= ~(1 << idx1);
		do {
			ptr = snd_info_get_str(str, ptr, sizeof(str));
			if (!strcmp(str, "disable")) {
				template.disable = 1;
			} else if (!strcmp(str, "direct")) {
				template.direct = 1;
			} else if (!strcmp(str, "block")) {
				template.block = 1;
			} else if (!strcmp(str, "non-block")) {
				template.nonblock = 1;
			} else if (!strcmp(str, "partial-frag")) {
				template.partialfrag = 1;
			} else if (!strcmp(str, "no-silence")) {
				template.nosilence = 1;
			} else if (!strcmp(str, "buggy-ptr")) {
				template.buggyptr = 1;
			}
		} while (*str);
		if (setup == NULL) {
			setup = kmalloc(sizeof(*setup), GFP_KERNEL);
			if (! setup) {
				buffer->error = -ENOMEM;
				mutex_unlock(&pstr->oss.setup_mutex);
				return;
			}
			if (pstr->oss.setup_list == NULL)
				pstr->oss.setup_list = setup;
			else {
				for (setup1 = pstr->oss.setup_list;
				     setup1->next; setup1 = setup1->next);
				setup1->next = setup;
			}
			template.task_name = kstrdup(task_name, GFP_KERNEL);
			if (! template.task_name) {
				kfree(setup);
				buffer->error = -ENOMEM;
				mutex_unlock(&pstr->oss.setup_mutex);
				return;
			}
		}
		*setup = template;
		mutex_unlock(&pstr->oss.setup_mutex);
	}
}