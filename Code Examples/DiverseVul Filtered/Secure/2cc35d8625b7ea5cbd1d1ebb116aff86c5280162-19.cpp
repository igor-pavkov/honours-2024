void ModuleSQL::OnUnloadModule(Module* mod)
{
	SQLerror err(SQL_BAD_DBID);
	Dispatcher->LockQueue();
	unsigned int i = qq.size();
	while (i > 0)
	{
		i--;
		if (qq[i].q->creator == mod)
		{
			if (i == 0)
			{
				// need to wait until the query is done
				// (the result will be discarded)
				qq[i].c->lock.Lock();
				qq[i].c->lock.Unlock();
			}
			qq[i].q->OnError(err);
			delete qq[i].q;
			qq.erase(qq.begin() + i);
		}
	}
	Dispatcher->UnlockQueue();
	// clean up any result queue entries
	Dispatcher->OnNotify();
}