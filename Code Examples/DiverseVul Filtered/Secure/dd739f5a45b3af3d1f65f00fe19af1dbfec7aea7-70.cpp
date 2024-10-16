R_API int r_core_cmd_foreach3(RCore *core, const char *cmd, char *each) { // "@@@"
	RDebug *dbg = core->dbg;
	RList *list, *head;
	RListIter *iter;
	int i;
	const char *filter = NULL;

	if (each[1] == ':') {
		filter = each + 2;
	}

	switch (each[0]) {
	case '=':
		foreach_pairs (core, cmd, each + 1);
		break;
	case '?':
		r_core_cmd_help (core, help_msg_at_at_at);
		break;
	case 'c':
		if (filter) {
			char *arg = r_core_cmd_str (core, filter);
			foreach_pairs (core, cmd, arg);
			free (arg);
		} else {
			eprintf ("Usage: @@@c:command   # same as @@@=`command`\n");
		}
		break;
	case 'C':
		r_meta_list_cb (core->anal, R_META_TYPE_COMMENT, 0, foreach_comment, (void*)cmd, UT64_MAX);
		break;
	case 'm':
		{
			int fd = r_io_fd_get_current (core->io);
			// only iterate maps of current fd
			RList *maps = r_io_map_get_for_fd (core->io, fd);
			RIOMap *map;
			if (maps) {
				RListIter *iter;
				r_list_foreach (maps, iter, map) {
					r_core_seek (core, map->itv.addr, 1);
					r_core_block_size (core, map->itv.size);
					r_core_cmd0 (core, cmd);
				}
				r_list_free (maps);
			}
		}
		break;
	case 'M':
		if (dbg && dbg->h && dbg->maps) {
			RDebugMap *map;
			r_list_foreach (dbg->maps, iter, map) {
				r_core_seek (core, map->addr, 1);
				//r_core_block_size (core, map->size);
				r_core_cmd0 (core, cmd);
			}
		}
		break;
	case 't':
		// iterate over all threads
		if (dbg && dbg->h && dbg->h->threads) {
			int origpid = dbg->pid;
			RDebugPid *p;
			list = dbg->h->threads (dbg, dbg->pid);
			if (!list) {
				return false;
			}
			r_list_foreach (list, iter, p) {
				r_core_cmdf (core, "dp %d", p->pid);
				r_cons_printf ("PID %d\n", p->pid);
				r_core_cmd0 (core, cmd);
			}
			r_core_cmdf (core, "dp %d", origpid);
			r_list_free (list);
		}
		break;
	case 'r':
		// registers
		{
			ut64 offorig = core->offset;
			for (i = 0; i < 128; i++) {
				RRegItem *item;
				ut64 value;
				head = r_reg_get_list (dbg->reg, i);
				if (!head) {
					continue;
				}
				r_list_foreach (head, iter, item) {
					if (item->size != core->anal->bits) {
						continue;
					}
					value = r_reg_get_value (dbg->reg, item);
					r_core_seek (core, value, 1);
					r_cons_printf ("%s: ", item->name);
					r_core_cmd0 (core, cmd);
				}
			}
			r_core_seek (core, offorig, 1);
		}
		break;
	case 'i': // @@@i
		// imports
		{
			RBinImport *imp;
			ut64 offorig = core->offset;
			list = r_bin_get_imports (core->bin);
			r_list_foreach (list, iter, imp) {
				char *impflag = r_str_newf ("sym.imp.%s", imp->name);
				ut64 addr = r_num_math (core->num, impflag);
				free (impflag);
				if (addr && addr != UT64_MAX) {
					r_core_seek (core, addr, 1);
					r_core_cmd0 (core, cmd);
				}
			}
			r_core_seek (core, offorig, 1);
		}
		break;
	case 'S': // "@@@S"
		{
			RBinObject *obj = r_bin_cur_object (core->bin);
			if (obj) {
				ut64 offorig = core->offset;
				ut64 bszorig = core->blocksize;
				RBinSection *sec;
				RListIter *iter;
				r_list_foreach (obj->sections, iter, sec) {
					r_core_seek (core, sec->vaddr, 1);
					r_core_block_size (core, sec->vsize);
					r_core_cmd0 (core, cmd);
				}
				r_core_block_size (core, bszorig);
				r_core_seek (core, offorig, 1);
			}
		}
#if ATTIC
		if (each[1] == 'S') {
			RListIter *it;
			RBinSection *sec;
			RBinObject *obj = r_bin_cur_object (core->bin);
			int cbsz = core->blocksize;
			r_list_foreach (obj->sections, it, sec){
				ut64 addr = sec->vaddr;
				ut64 size = sec->vsize;
				// TODO:
				//if (R_BIN_SCN_EXECUTABLE & sec->perm) {
				//	continue;
				//}
				r_core_seek_size (core, addr, size);
				r_core_cmd (core, cmd, 0);
			}
			r_core_block_size (core, cbsz);
		}
#endif
		break;
	case 's':
		if (each[1] == 't') { // strings
			list = r_bin_get_strings (core->bin);
			RBinString *s;
			if (list) {
				ut64 offorig = core->offset;
				ut64 obs = core->blocksize;
				r_list_foreach (list, iter, s) {
					r_core_block_size (core, s->size);
					r_core_seek (core, s->vaddr, 1);
					r_core_cmd0 (core, cmd);
				}
				r_core_block_size (core, obs);
				r_core_seek (core, offorig, 1);
			}
		} else {
			// symbols
			RBinSymbol *sym;
			ut64 offorig = core->offset;
			ut64 obs = core->blocksize;
			list = r_bin_get_symbols (core->bin);
			r_cons_break_push (NULL, NULL);
			r_list_foreach (list, iter, sym) {
				if (r_cons_is_breaked ()) {
					break;
				}
				r_core_block_size (core, sym->size);
				r_core_seek (core, sym->vaddr, 1);
				r_core_cmd0 (core, cmd);
			}
			r_cons_break_pop ();
			r_core_block_size (core, obs);
			r_core_seek (core, offorig, 1);
		}
		break;
	case 'f': // flags
		{
		// TODO: honor ^C
			char *glob = filter? r_str_trim_dup (filter): NULL;
			ut64 off = core->offset;
			ut64 obs = core->blocksize;
			struct exec_command_t u = { .core = core, .cmd = cmd };
			r_flag_foreach_glob (core->flags, glob, exec_command_on_flag, &u);
			r_core_seek (core, off, 0);
			r_core_block_size (core, obs);
			free (glob);
		}
		break;
	case 'F': // functions
		{
			ut64 obs = core->blocksize;
			ut64 offorig = core->offset;
			RAnalFunction *fcn;
			list = core->anal->fcns;
			r_cons_break_push (NULL, NULL);
			r_list_foreach (list, iter, fcn) {
				if (r_cons_is_breaked ()) {
					break;
				}
				if (!filter || r_str_glob (fcn->name, filter)) {
					r_core_seek (core, fcn->addr, 1);
					r_core_block_size (core, r_anal_fcn_size (fcn));
					r_core_cmd0 (core, cmd);
				}
			}
			r_cons_break_pop ();
			r_core_block_size (core, obs);
			r_core_seek (core, offorig, 1);
		}
		break;
	case 'b':
		{
			RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, 0);
			ut64 offorig = core->offset;
			ut64 obs = core->blocksize;
			if (fcn) {
				RListIter *iter;
				RAnalBlock *bb;
				r_list_foreach (fcn->bbs, iter, bb) {
					r_core_seek (core, bb->addr, 1);
					r_core_block_size (core, bb->size);
					r_core_cmd0 (core, cmd);
				}
				r_core_block_size (core, obs);
				r_core_seek (core, offorig, 1);
			}
		}
		break;
	}
	return 0;
}