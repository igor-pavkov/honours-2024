Polygon2d *DxfData::toPolygon2d() const
{
	auto poly = new Polygon2d();
	for (size_t i = 0; i < this->paths.size(); ++i) {
		const auto &path = this->paths[i];
		Outline2d outline;
		size_t endidx = path.indices.size();
		// We don't support open paths; closing them to be compatible with existing behavior
		if (!path.is_closed) endidx++;
		for (size_t j = 1; j < endidx; ++j) {
			outline.vertices.push_back(this->points[path.indices[path.indices.size()-j]]);
		}
		poly->addOutline(outline);
	}
	return poly;
}