      static bool cut(const double val) { return val<(double)min()?min():val>(double)max()?max():(bool)val; }