ColumnValue
===========

Introduction
------------

Binlog API contains class which gives structure for column value of any row event.

.. autoclass:: ColumnValue

   :members: value,is_null,col_num



Constructor
-----------

.. cc:function:: ColumnValue(std::string st,bool _bool,int col_n)

   Set's the data member respectively.


Getter Function
---------------

.. cc:function:: getValue()

   returns  Value of column as string

.. cc:function:: isNull()

   return column is null or not

.. cc:fucntion:: getColumnNum()

   return position of column in original schema.
	
