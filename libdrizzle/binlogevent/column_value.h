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
