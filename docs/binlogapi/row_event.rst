RowEvent
========


Introduction
------------

This class parse the row event.

.. autoclass:: RowEvent
  
   :members: column_type,table_id,flag,column_count,column_null_bitmap, column_bitmap,null_bitmap,rows,row_event_type

Constructor
-----------

It will initialize all its data members to NULL/-1

Getter Functions
----------------

.. cc:function::  void initWithData(const unsigned char * data)

   Get the raw data and call all the setter functions with appropriate value

   :param data: Raw data from binlog 

.. cc:function::uint32_t  getTimestamp()


   Returns the timestamp for given event

.. cc:function:: uint8_t getType()


   Returns the Type for given event
    
.. cc:function:: uint32_t  getServerId()
    
    
   Returns the timestamp for given event
    
.. cc:function:: uint32_t  getEventSize()


   Returns the Event Size for given event

.. cc:function:: uint32_t  getLogPos()


   Returns the Log Position for given event

.. cc:function:: uint32_t  getFlagH()


   Returns the flag header for given event



Getter Functions
----------------

.. cc:function::  void initWithData(const unsigned char * data)

   Get the raw data and call all the setter functions with appropriate value

   :param data: Raw data from binlog 

.. cc:function::uint32_t  getTimestamp()


   Returns the timestamp for given event

.. cc:function:: uint8_t getType()


   Returns the Type for given event
    
.. cc:function:: uint32_t  getServerId()
    
    
   Returns the timestamp for given event
    
.. cc:function:: uint32_t  getEventSize()


   Returns the Event Size for given event

.. cc:function:: uint32_t  getLogPos()


   Returns the Log Position for given event

.. cc:function:: uint32_t  getFlagH()


   Returns the flag header for given event


Getter Functions
----------------

.. cc:function::  void initWithData(const unsigned char * data)

   Get the raw data and call all the setter functions with appropriate value

   :param data: Raw data from binlog 

.. cc:function::uint32_t  getTimestamp()


   Returns the timestamp for given event

.. cc:function:: uint8_t getType()


   Returns the Type for given event
    
.. cc:function:: uint32_t  getServerId()

    
   Returns the timestamp for given event
    
.. cc:function:: uint32_t  getEventSize()


   Returns the Event Size for given event

.. cc:function:: uint32_t  getLogPos()


   Returns the Log Position for given event

.. cc:function:: uint32_t  getFlagH()


   Returns the flag header for given event

.. cc:function::  uint64_t getTableId()

  Return the table id for given event

.. cc:function:: uint16_t getFlagPh()

  Return the flag of post header for given event

.. cc:function:: uint64_t getColumnCount()

  Return the number of columns for given event

.. cc:function:: bool * getColumnPresentBitmap()

  Return the column present bitmap for given event

.. cc:function:: RowVector getRows()

  Return the rowvector for given event

.. cc:function:: int getRowEventType()

  Return the event type for given event. There are three kind of evens in row event - Insert,Delete,Update


Setter Functions
----------------

.. cc:function:: void setTableId(uint64_t value)

   Set the parsed value of table_id  into given object.

.. cc:function:: void setFlagPh(uint16_t value)

   Set the parsed value of flag into given object.

.. cc:function:: void setColumnCount(uint64_t value)

   Set the parsed value of column_count into given object.
   


.. cc:function:: TODO 

   (Update Event)Set the parsed value of column_null_bitmap into given object.

.. cc:function:: void setColumnPresentBitmap(bool * value) 

   Set the parsed value of column_bitmap into given object.

.. cc:function:: void setNullBitmap(bool * value)

   Set the parsed value of null_bitmap into given object.


.. cc:function:: void RowEvent::setRowEventType(int value)

   Set the parsed value of row_event_type into given object.
