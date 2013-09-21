QueryEvent
==========

Introduction
------------

This class parse the query event.

.. autoclass:: QueryEvent

   :members: proxy_id, execution_time, schema_length, error_code

Constructor
-----------

It will initialize all its data members to -1

Getter Functions
----------------


.. cc:function::uint32_t  getTimestamp()


   Returns the timestamp for given event

.. cc:function:: uint8_t getType()


   Returns the Type for given event

.. cc:function:: uint32_t  getServerId()


   Returns the timestamp for given event

.. cc:function:: uint32_t  getEventSize()


   Returns the Event Size for given event

.. cc:function:: uint32_t  getLogPos()


   Returns the Log Position for next event

.. cc:function:: uint32_t  getFlagH()


   Returns the binlog Event Flag from header


.. cc:function:: uint32_t  getProxyId()


   Returns the proxy id of master 

.. cc:function:: uint32_t  getExecutionTime()


   Returns the execution time for the event

.. cc:function:: uint32_t  getSchemaLength()


   Returns the schema length for query

.. cc:function:: uint32_t  getErrorCode()


   Returns the binlog error code for query event


Setter Functions
----------------

.. cc:function::  void initWithData(const unsigned char * data)

   Get the raw data and call all the setter functions with appropriate value

   :param data: Raw data from binlog 

.. cc:function:: void  setProxyId(uint32_t value)


   sets the parsed proxy id of master into its object

   :param value: parsed proxy id

.. cc:function:: void  setExecutionTime(uint32_t value)


   sets the execution time for the event into its object

   :param value: parsed Execution time

.. cc:function:: void  setSchemaLength(uint32_t value)


   sets the Schema length into its object
   
   :param value: parsed Schema length 

.. cc:function:: void  setErrorCode(uint32_t value)


   sets the error code  for the event into its object

   :param value: parsed Error Code 

