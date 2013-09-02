/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * summary: column value info
 *
 */
#include<iostream>
namespace binlogevent
{
	class ColumnValue
	{
		public:
			ColumnValue(int p_start,int p_end)
			{
				start=p_start;
				end=p_end;
			}
			int start;
			int end;
	};
}
