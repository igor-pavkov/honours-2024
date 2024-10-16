  bool matches(const Ref *idA, double m11A, double m12A,
		double m21A, double m22A)
    { return fontID == *idA &&
	     m11 == m11A && m12 == m12A && m21 == m21A && m22 == m22A; }