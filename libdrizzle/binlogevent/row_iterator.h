/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 *
 */
#include<iostream>
#include"column_value.h"
#include<vector>

using namespace std;
namespace binlogevent
{
	typedef vector<vector<ColumnValue> > RowVector;

	class RowIterator
	{
		public:
			void next();
			bool hasNext();
			void setCount(int value);

			vector<vector<ColumnValue> > vec_rows;
			vector<ColumnValue> vec_col_val;
			void setCurrentPos(int value);
		private:	
			int count;
			int current_pos;
	};
}
