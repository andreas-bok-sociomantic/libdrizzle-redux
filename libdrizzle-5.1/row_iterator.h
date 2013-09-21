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

#ifndef ROW_ITERATOR
#define ROW_ITERATOR
namespace binlogevent
{
	typedef vector<vector<ColumnValue> > RowVector;
	typedef vector<ColumnValue> Row;

	class RowIterator
	{
		public:
			RowIterator()
			{
			}
			vector<vector<ColumnValue> > vec_rows;
			vector<ColumnValue> vec_col_val;
//			RowVector
	};
}
#endif
