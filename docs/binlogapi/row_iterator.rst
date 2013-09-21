RowIterator
===========

Introduction
------------

This class creates the iterator datatypes to iterate on different rows for a RowEvent. 

.. warnings::
	This will work only in case of Row Based Logging
	

.. autoclass:: RowIterator 

   :members: vec_rows, vec_col_val 

Constructor
-----------

It will initialize all its data members to NULL 

DataTypes
---------

.. c:type:: RowVector

   Its a vector of ColumnValue vector i.e vector<vector<ColumnValue> >
 
.. c:type:: Row   

   Its a vector of ColumnValue i.e  vector<ColumnValue>






