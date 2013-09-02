/* 
 * Copyright (C) 2013 Drizzle Developer Group
 * Copyright (C) 2013 Kuldeep Porwal
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * summary: utility functions to get each row of Row event
 * 
 */
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
