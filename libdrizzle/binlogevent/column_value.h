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
namespace binlogevent
{
	class ColumnValue
	{
		public:
			ColumnValue(std::string st,unsigned int val,bool _bool,int col_n)
			{
				str=st;
				value=val;
				is_null = _bool;
				col_num = col_n;
			}
				
			std::string getString() {return str;  } // Check: str.empty() is true so int value otherwise string value.
			int    getInt()    {return value;}
			bool isNull() { return is_null;}
			int getColumnNum() {return col_num;}

		private:
			std::string str;
			unsigned int value;
			bool is_null;
			int col_num;
	};
}
