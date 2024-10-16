int DxfData::addPoint(double x, double y)
{
	this->points.emplace_back(x, y);
	return this->points.size() - 1;
}