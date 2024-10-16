void DxfData::fixup_path_direction()
{
	for (size_t i = 0; i < this->paths.size(); ++i) {
		if (!this->paths[i].is_closed) break;
		this->paths[i].is_inner = true;
		double min_x = this->points[this->paths[i].indices[0]][0];
		size_t min_x_point = 0;
		for (size_t j = 1; j < this->paths[i].indices.size(); ++j) {
			if (this->points[this->paths[i].indices[j]][0] < min_x) {
				min_x = this->points[this->paths[i].indices[j]][0];
				min_x_point = j;
			}
		}
		// rotate points if the path is in non-standard rotation
		size_t b = min_x_point;
		size_t a = b == 0 ? this->paths[i].indices.size() - 2 : b - 1;
		size_t c = b == this->paths[i].indices.size() - 1 ? 1 : b + 1;
		double ax = this->points[this->paths[i].indices[a]][0] - this->points[this->paths[i].indices[b]][0];
		double ay = this->points[this->paths[i].indices[a]][1] - this->points[this->paths[i].indices[b]][1];
		double cx = this->points[this->paths[i].indices[c]][0] - this->points[this->paths[i].indices[b]][0];
		double cy = this->points[this->paths[i].indices[c]][1] - this->points[this->paths[i].indices[b]][1];
#if 0
		printf("Rotate check:\n");
		printf("  a/b/c indices = %d %d %d\n", a, b, c);
		printf("  b->a vector = %f %f (%f)\n", ax, ay, atan2(ax, ay));
		printf("  b->c vector = %f %f (%f)\n", cx, cy, atan2(cx, cy));
#endif
		// FIXME: atan2() usually takes y,x. This variant probably makes the path clockwise..
		if (atan2(ax, ay) < atan2(cx, cy)) {
			std::reverse(this->paths[i].indices.begin(), this->paths[i].indices.end());
		}
	}
}