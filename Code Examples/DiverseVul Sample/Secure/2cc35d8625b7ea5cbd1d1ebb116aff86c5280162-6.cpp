void DispatcherThread::OnNotify()
{
	// this could unlock during the dispatch, but OnResult isn't expected to take that long
	this->LockQueue();
	for(ResultQueue::iterator i = Parent->rq.begin(); i != Parent->rq.end(); i++)
	{
		MySQLresult* res = i->r;
		if (res->err.id == SQL_NO_ERROR)
			i->q->OnResult(*res);
		else
			i->q->OnError(res->err);
		delete i->q;
		delete i->r;
	}
	Parent->rq.clear();
	this->UnlockQueue();
}