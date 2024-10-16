scoped_ostream::~scoped_ostream()
{
	delete stream;
}