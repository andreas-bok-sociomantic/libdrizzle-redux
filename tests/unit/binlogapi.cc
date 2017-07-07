/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (C) 2012-2013 Drizzle Developer Group
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 *     * The names of its contributors may not be used to endorse or
 * promote products derived from this software without specific prior
 * written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <yatl/lite.h>

#include <iostream>
#include <libdrizzle-5.1/libdrizzle.h>
#include <libdrizzle-5.1/binlogapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>

using namespace binlogevent;

TableMapEvent tableMap;
XidEvent xevent;

void binlog_error(drizzle_return_t ret, drizzle_st *con, void *context)
{
	(void) context;
	if (ret != DRIZZLE_RETURN_EOF)
	{
		printf("Error retrieving binlog: %s\n", drizzle_error(con));
	}
}

void binlog_event(drizzle_binlog_event_st *event, void *context)
{
	(void) context;
	uint32_t length;
	uint8_t type;

	const unsigned char* data;
	uint32_t ts = drizzle_binlog_event_timestamp(event);
	length= drizzle_binlog_event_length(event);
	type= drizzle_binlog_event_type(event);
	cout<< "type: " <<  (int)type<< " length: " << length << endl;

	if(type==16)
	{
		data= drizzle_binlog_event_raw_data(event);
		EventData event_raw_data(data);
		event_raw_data.getXidEvent(xevent);
		printf("AS->%d\n",xevent.getTimestamp());
	}

	if((int)type==19) //TABLE_MAP_EVENT)
	{
		data= drizzle_binlog_event_raw_data(event);
		EventData event_raw_data(data);
		event_raw_data.getTableMap(tableMap);
		printf("Timestamp %d\n", ts);
		printf("Logpos %d\n", tableMap.getLogPos());
		printf("Schemaname: %s\n", tableMap.getSchemaName());
		printf("Tablename: %s\n", tableMap.getTableName());

		printf("cols: %" PRIu64 "\n", tableMap.getColumnCount());
		// uint8_t *val=  tableMap.getColumnTypeDef();
		// for(uint i=0;i< tableMap.getColumnCount();i++)
		// {
		// 	cout<< (int)val[i]<<endl;
		// }
	}
	if(type==23) //WRITE_ROW_EVENTv1
	{
		data= drizzle_binlog_event_raw_data(event);
		EventData event_raw_data(data);
		RowEvent writeEvent(tableMap.getColumnTypeDef());
		event_raw_data.getWriteEvent(writeEvent);
		cout << "col count by data and raw_data :" << writeEvent.getTimestamp() << ":  " <<  writeEvent.getColumnCount() << endl;
		cout << "QUIT event type: 23" << endl;
	}

}

int main(void)
{
	drizzle_st *con;
	drizzle_binlog_st *binlog;
	drizzle_return_t ret;

	  con = drizzle_create(getenv("MYSQL_SOCK"),
	                       0, getenv("MYSQL_USER"),
	                       getenv("MYSQL_PASSWORD"),
	                       getenv("MYSQL_SCHEMA"), 0);

	ret= drizzle_connect(con);
	if (ret != DRIZZLE_RETURN_OK)
	{
		printf("Could not connect to server: %s\n", drizzle_error(con));
		return EXIT_FAILURE;
	}

	binlog= drizzle_binlog_init(con, binlog_event, binlog_error, NULL, true);

  	char *binlog_file;
  	drizzle_binlog_get_filename(con, &binlog_file, -1);
	ret= drizzle_binlog_start(binlog, 0, binlog_file, 0);

	ASSERT_EQ_(DRIZZLE_RETURN_EOF, ret, "Drizzle binlog start failure: %s(%s)",
		drizzle_error(con), drizzle_strerror(ret));

	ret= drizzle_quit(con);
  	ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "Closing drizzle connection");

	return EXIT_SUCCESS;
}
