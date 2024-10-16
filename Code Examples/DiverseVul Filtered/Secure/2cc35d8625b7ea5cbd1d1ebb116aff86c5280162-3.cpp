	void submit(SQLQuery* q, const std::string& qs)
	{
		Parent()->Dispatcher->LockQueue();
		Parent()->qq.push_back(QQueueItem(q, qs, this));
		Parent()->Dispatcher->UnlockQueueWakeup();
	}