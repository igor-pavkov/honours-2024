int md_in_flight(struct mapped_device *md)
{
	return atomic_read(&md->pending[READ]) +
	       atomic_read(&md->pending[WRITE]);
}