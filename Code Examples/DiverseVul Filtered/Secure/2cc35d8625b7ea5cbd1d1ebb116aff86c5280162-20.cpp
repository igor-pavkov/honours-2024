	virtual SQLEntry GetValue(int row, int column)
	{
		if ((row >= 0) && (row < rows) && (column >= 0) && (column < (int)fieldlists[row].size()))
		{
			return fieldlists[row][column];
		}
		return SQLEntry();
	}