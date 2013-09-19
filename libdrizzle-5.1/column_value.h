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
#include<string>

#ifndef COLUMN_VALUE
#define COLUMN_VALUE
namespace binlogevent
{
	class ColumnValue
	{
		public:
			ColumnValue(std::string st,bool _bool,int col_n)
			{
				value=st;
				is_null = _bool;
				col_num = col_n;
			}
				
			std::string getValue() {return value;  } // Check: str.empty() is true so int value otherwise string value.
			bool isNull() { return is_null;}
			int getColumnNum() {return col_num;}

		private:
			std::string value;
			bool is_null;
			int col_num;
	};
}
#endif
