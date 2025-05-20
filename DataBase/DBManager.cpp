#include <sstream>
#include <iostream>
#include "DBManager.h"
#include "../ModBus/SmartHandler.h"
#include "../HandledData.h"



extern HandledData out_data_holder;

DBManager::DBManager(const database_params& db_params)
{
    //error handling. Where located if service
    int rc = sqlite3_open(db_params.db_name.c_str(), &db);
    if( rc )
    {
		std::cout<<"Can't open database: "<< sqlite3_errmsg(db)<<"\n";
        throw std::runtime_error("Can't open database");
    }
	/*
	else
		std::cout<<"Open database successfully\n\n";
	*/
    table_name = db_params.table_name;    
    mode = db_params.mode;
	days_of_storage = db_params.days_of_storage;

}

DBManager::~DBManager()
{
    sqlite3_close(db);
}

bool DBManager::CreateTable(const PARAMS& out_params)
{
    std::string create_request = make_create_request(out_params);
    //std::cout << create_request << std::endl;
    int rc = sqlite3_exec(db, create_request.c_str(), NULL, 0, &zErrMsg);
	if( rc!=SQLITE_OK )
	{
			std::cout<<"create SQL error: "<< sqlite3_errmsg(db) <<"\n";
			sqlite3_free(zErrMsg);
			return false;
	}
	std::cout<<"Table created successfully\n\n";
    return true;
}

bool DBManager::InsertData(const PARAMS& out_params)
{
    std::string insert_request = make_insert_request(out_params);
	//std::cout << insert_request <<std::endl;
    int rc = sqlite3_exec(db, insert_request.c_str(), NULL, 0, &zErrMsg);
	if( rc!=SQLITE_OK )
	{
			std::cout<<"insert SQL error: "<< sqlite3_errmsg(db) <<"\n";
			sqlite3_free(zErrMsg);
			return false;
	}
	std::cout<<"Data inserted successfully\n\n";
    return true;
}



static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	std::cout<<"callback : \n";
	for(i=0; i< argc; i++)
	{
		std::cout<< azColName[i] <<" = " << (argv[i] ? argv[i] : "NULL")<<"\n";
	}
	std::cout<<"\n";
	return 0;
}



bool DBManager::PrintAllData()
{
    std::string select_all_request = make_select_all_request();
    int rc = sqlite3_exec(db, select_all_request.c_str(), callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK )
	{
			std::cout<<"select SQL error: "<< sqlite3_errmsg(db) <<"\n";
			sqlite3_free(zErrMsg);
			return false;
	}
	std::cout<<"Selwctedted successfully\n\n";
    return true;
}

bool DBManager::PrintData(const last_data_selector& sel)
{
    std::string select_request = make_select_last_request(sel);
    int rc = sqlite3_exec(db, select_request.c_str(), callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK )
	{
			std::cout<<"select SQL error: "<< sqlite3_errmsg(db) <<"\n";
			sqlite3_free(zErrMsg);
			return false;
	}
	std::cout<<"Selwctedted successfully\n\n";
    return true;   
}

bool DBManager::ClearTable()
{
    std::string delete_all_request = make_delete_all_request();
    int rc = sqlite3_exec(db, delete_all_request.c_str(), NULL, 0, &zErrMsg);
	if( rc!=SQLITE_OK )
	{
			std::cout<<"delete SQL error: "<< sqlite3_errmsg(db) <<"\n";
			sqlite3_free(zErrMsg);
			return false;
	}
	std::cout <<"Delete all successfully\n\n";
    return true;
}


bool DBManager::ClearOldRecords()
{
    std::string delete_olds_request = make_delete_olds_request();
	int rc = sqlite3_exec(db, delete_olds_request.c_str(), NULL, 0, &zErrMsg);
	if( rc!=SQLITE_OK )
	{
			std::cout<<"delete SQL error: "<< sqlite3_errmsg(db) <<"\n";
			sqlite3_free(zErrMsg);
			return false;
	}
	std::cout<<"Delete olds successfully\n\n";
    return true;
}


void DBManager::GetCurrentData()
{ 
    previous = current;
    current = SmartHandlerBase::data;
    //compare current and previous and write full data if diff
    CheckEvent();
}

void DBManager::CheckEvent()
{ 
    //for example
    if(current.AC_FAIL != previous.AC_FAIL)
        RaiseRecordWriting();

}

void DBManager::RaiseRecordWriting()
{
    for (auto &&param : out_data_holder.GetOutParams())
    {
        //TODO : create insert request from params

    }
}


std::string DBManager::make_create_request(const PARAMS& out_params)
{
	std::string create_request{"create table if not exists"};
	create_request += " ";
	create_request += table_name;
	create_request += " (";
	create_request += " rowid integer, timestamp varchar(30), ";

	for(auto& par : out_params)
	{
		//std::cout << par.first << std::endl;
		if(par.first != "Updated")
		{
			if(par.second.size() == 1)
			{
				create_request += " ";
				create_request += par.first;
				create_request += " varchar(50),";
			}
			else
			{
				for(auto sp : par.second )
				{
					create_request += " ";
					create_request += sp.first;
					create_request += " varchar(50),";
				}
			}
		}		
	}
	//remove last comma
	if (!create_request.empty())
		create_request.pop_back();

	create_request += " )";	

	return create_request;
}

#include <iomanip>
#include <ctime>

std::string DBManager::make_insert_request(const PARAMS& out_params)
{
	std::string insert_request{"insert into"};
	insert_request += " ";
	insert_request += table_name;
	insert_request += " ( rowid, timestamp, ";

	for(auto& par : out_params)
	{
		//std::cout << par.first << std::endl; 
		if(par.first != "Updated")
		{		
			if(par.second.size() == 1)
			{
				insert_request += " ";
				insert_request += par.first;
				insert_request += " ,";

			}
			else
			{
				for(auto sp : par.second )
				{
					insert_request += " ";
					insert_request += sp.first;
					insert_request += " ,";
				}
			}		
		}
	}
	//remove last comma
	if (!insert_request.empty())
		insert_request.pop_back();

	insert_request += " ) values (";

    std::time_t result = std::time(nullptr);
	std::stringstream ss;
	ss << result;
	std::string rowid = ss.str();

/*
1)  Формат не подходит для datetime в sqlite 
	std::string timestamp{std::asctime(std::localtime(&result))};	
	insert_request += rowid;
	insert_request += ", '";
	insert_request += timestamp;
	insert_request += "', ";
*/
/*
2)
	MSK in desktop and UTC in FireBird 
    std::string timestamp{"datetime('now', '+3 hours')"};
	//std::cout << timestamp <<std::endl;
*/
	//3) Manualy format date-time as ISO8601 string
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
	ss.str(std::string());
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
	std::string timestamp = ss.str();
	

	insert_request += rowid;
	insert_request += ", '";
	insert_request += timestamp;
	insert_request += "', ";

	for(auto& par : out_params)
	{
		if(par.first != "Updated")
		{
			if(par.second.size() == 1)
			{
				insert_request += " '";
				insert_request += par.second.begin()->second;
				insert_request += "' ,";

			}
			else
			{
				for(auto sp : par.second )
				{
					insert_request += " '";
					insert_request += sp.second;
					insert_request += "' ,";
				}
			}
		}		
	}
	//remove last comma
	if (!insert_request.empty())
		insert_request.pop_back();

	insert_request += " )";			

	return insert_request;
}

//"select * from myTable";
std::string DBManager::make_select_all_request()
{
	std::string select_all_request{"select * from "};
	select_all_request += " ";
	select_all_request += table_name;
	return select_all_request;
}

std::string DBManager::make_select_last_request(const last_data_selector& sel)
{
    std::time_t result = std::time(nullptr);
    //std::cout << std::asctime(std::localtime(&result)) << result << " seconds since the Epoch\n";
    if(sel == last_data_selector::last_hour)
        result -= 3600;
    if(sel == last_data_selector::last_day)
        result -= (3600*24);
    if(sel == last_data_selector::last_week)
        result -= 3600*24*7;
    if(sel == last_data_selector::last_month)
        result -= 3600*24*30;
  
	std::stringstream ss;
	ss << result;
	std::string thresh = ss.str();

    std::string select_request{"select * from "};
    select_request += table_name;
	select_request += " where rowid>";
	select_request += thresh;
	return select_request;
}

//"delete from myTable";
std::string DBManager::make_delete_all_request()
{
	std::string delete_all_request{"delete from "};
	delete_all_request += " ";
	delete_all_request += table_name;
	return delete_all_request;
}

std::string DBManager::make_delete_olds_request()
{
	std::string delete_olds_request{"delete from "};
	delete_olds_request += " ";
	delete_olds_request += table_name;
    delete_olds_request += " where rowid<";
    std::time_t result = std::time(nullptr);
	int64_t now = result;
	//std::cout << "now " <<  now << std::endl;
	int64_t then = now - 3600*24*days_of_storage;
	//std::cout << "then " <<  then << std::endl;
  
	std::stringstream ss;
	ss << then;
	std::string thresh = ss.str();
    delete_olds_request += thresh;

	std::cout << delete_olds_request << std::endl;

	return delete_olds_request;
}


