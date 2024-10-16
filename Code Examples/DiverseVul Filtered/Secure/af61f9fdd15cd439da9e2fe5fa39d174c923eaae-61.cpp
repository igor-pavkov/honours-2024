scoped_istream::~scoped_istream()
{
	delete stream;
}