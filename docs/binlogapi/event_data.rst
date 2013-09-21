Event Data
==========

Introduction
------------

This class is capable to return the objects for different events with their corresponding header and data parsed.

.. warnings::
	   This API will only work for Row Based Logs

.. autoclass:: EventData

   :members: _data 

Constructor
-----------

It will initialize the EventData object with given MySQL binary log data. we can update this later with setData(below)


Getter Functions
----------------

.. cc:function::  void getTableMap(TableMapEvent&  tableMap)

   This function will fill the tablemap object with event raw data	
   :param tableMap: reference to TableMapEvent object

.. cc:function::  void getWriteEvent(RowEvent&  writeEvent)

   This function will fill the writeEvent object with event raw data

   :param writeEvent: reference to RowEvent object

.. cc:function::  void getUpdateEvent(RowEvent&  updateEvent)

   This function will fill the updateEvent object with event raw data

   :param updateEvent: reference to RowEvent object

.. cc:function::  void getDeleteEvent(RowEvent&  deleteEvent)

   This function will fill the deleteEvent object with event raw data

   :param deleteEvent: reference to RowEvent object

.. cc:function::  void getQueryEvent(RowEvent&  queryEvent)

   This function will fill the queryEvent object with event raw data

   :param queryEvent: reference to RowEvent object

.. cc:function::  void getXidEvent(XidEvent&  xidEvent)

   This function will fill the xidEvent object with event raw data

   :param xidEvent: reference to XidEvent object


Setter Function
---------------

.. cc:function:: void setdata(const unsigned char* data)

   This function will update the MySQL binary log data in event data object whenever we read a new binlog

   :param data: binary Data for a MySQL binlog

