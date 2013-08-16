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
