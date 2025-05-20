#pragma once

#include <time.h>
#include "sqlite3.h"
#include "../ModBus/SmartHandler.h"
#include "../HandledData.h"

enum class last_data_selector
{
    last_hour,
    last_day,
    last_week,
    last_month
};

struct database_params
{
    std::string db_name;
    std::string table_name;
    std::string mode;
    int days_of_storage;
};

class DBManager
{
    Data previous;
    Data current;
    sqlite3 *db;
    std::string db_name;
	std::string table_name;
    std::string mode;
    int days_of_storage;
    char *zErrMsg = 0;
    void CheckEvent();
    void RaiseRecordWriting();
    std::string make_create_request(const PARAMS& out_params);
    std::string make_insert_request(const PARAMS& out_params);
    std::string make_select_all_request();
    std::string make_select_last_request(const last_data_selector&);
    std::string make_delete_all_request();
    std::string make_delete_olds_request();//save last month only
    
public:
    DBManager(const database_params& db_params);
    ~DBManager();
    void GetCurrentData();
    bool CreateTable(const PARAMS& out_params);
    bool InsertData(const PARAMS& out_params);
    bool PrintAllData();
    bool PrintData(const last_data_selector&);
    bool ClearTable();
    bool ClearOldRecords();

};