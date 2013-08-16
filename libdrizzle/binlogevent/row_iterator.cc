#include"row_iterator.h"

using namespace binlogevent;

void RowIterator::next()
{
	current_pos++;	
}

bool RowIterator::hasNext()
{
	return current_pos < count;
}

void RowIterator::setCount(int value)
{
	count= value;
}

void RowIterator::setCurrentPos(int value)
{
	current_pos= value;
}
