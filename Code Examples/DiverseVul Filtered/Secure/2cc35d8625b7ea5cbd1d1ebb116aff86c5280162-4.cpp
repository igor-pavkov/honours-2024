void ModuleSQL::OnRehash(User* user)
{
	ConnMap conns;
	ConfigTagList tags = ServerInstance->Config->ConfTags("database");
	for(ConfigIter i = tags.first; i != tags.second; i++)
	{
		if (i->second->getString("module", "mysql") != "mysql")
			continue;
		std::string id = i->second->getString("id");
		ConnMap::iterator curr = connections.find(id);
		if (curr == connections.end())
		{
			SQLConnection* conn = new SQLConnection(this, i->second);
			conns.insert(std::make_pair(id, conn));
			ServerInstance->Modules->AddService(*conn);
		}
		else
		{
			conns.insert(*curr);
			connections.erase(curr);
		}
	}

	// now clean up the deleted databases
	Dispatcher->LockQueue();
	SQLerror err(SQL_BAD_DBID);
	for(ConnMap::iterator i = connections.begin(); i != connections.end(); i++)
	{
		ServerInstance->Modules->DelService(*i->second);
		// it might be running a query on this database. Wait for that to complete
		i->second->lock.Lock();
		i->second->lock.Unlock();
		// now remove all active queries to this DB
		for (size_t j = qq.size(); j > 0; j--)
		{
			size_t k = j - 1;
			if (qq[k].c == i->second)
			{
				qq[k].q->OnError(err);
				delete qq[k].q;
				qq.erase(qq.begin() + k);
			}
		}
		// finally, nuke the connection
		delete i->second;
	}
	Dispatcher->UnlockQueue();
	connections.swap(conns);
}