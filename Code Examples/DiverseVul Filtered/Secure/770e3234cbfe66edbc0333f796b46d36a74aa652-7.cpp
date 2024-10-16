std::string DxfData::dump() const
{
	std::ostringstream out;
	out << "DxfData"
	  << "\n num points: " << points.size()
	  << "\n num paths: " << paths.size()
	  << "\n num dims: " << dims.size()
	  << "\n points: ";
	for (size_t k = 0; k < points.size(); ++k ) {
		out << "\n  x y: " << points[k].transpose();
	}
	out << "\n paths: ";
	for (size_t i = 0; i < paths.size(); ++i) {
		out << "\n  path:" << i
		  << "\n  is_closed: " << paths[i].is_closed
		  << "\n  is_inner: " << paths[i].is_inner ;
		DxfData::Path path = paths[i];
		for (size_t j = 0; j < path.indices.size(); ++j) {
			out << "\n  index[" << j << "]==" << path.indices[j];
		}
	}
	out << "\nDxfData end";
	return out.str();
}