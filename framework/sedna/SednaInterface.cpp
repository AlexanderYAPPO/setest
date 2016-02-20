/*
    Copyright 2012 Ilya Taranov <ilya@taranoff.net>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#include "SednaInterface.h"

#include <iostream>
#include <cstring>
#include <glib.h>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <libsedna.h>

using namespace sedna;

const std::string Session::url = "localhost:5050";
const std::string Session::login = "SYSTEM";
const std::string Session::password = "MANAGER";

const std::string Database::CDB_EXECUTABLE = "se_cdb";
const std::string Database::SM_EXECUTABLE = "se_sm";
const std::string Database::SMSD_EXECUTABLE = "se_smsd";
const std::string Database::DROP_EXECUTABLE = "se_ddb";

const std::string SednaInterface::GOV_EXECUTABLE = "se_gov";
const std::string SednaInterface::STOP_EXECUTABLE = "se_stop";

//Static buffer for result retrieval
static char resultBuf[Session::resultBufferSize + 10];

inline static
bool spawnProcess(std::string executableName) {
    int resultCode;

    GError *error;
    bool isOk = g_spawn_command_line_sync(executableName.c_str(), NULL, NULL, &resultCode, &error);

    if (!isOk) {
        throw std::runtime_error(error->message);
        return false;
    }
    return true;
}

/**************************************************
 * Query implementation
 * ************************************************/

Query::Query(const Query::Body &_body)
    : body(_body.body)
{
}

Query::Query(const Query::Filename &filename)
{
    boost::filesystem::path queryFile(filename.filename);
    body.resize(boost::filesystem::file_size(queryFile));
    boost::filesystem::ifstream fstream(queryFile);
    fstream.read(&body[0], body.size());
}


/**************************************************
 * Transaction implementation
 * ************************************************/

void Transaction::commit()
{
    active = false;
}

void Transaction::execute(Query &query)
{
    session->execute(query);
}

void Transaction::rollback()
{
    active = false;
}

Transaction::Transaction(Session *_session)
  : active(false), session(_session)
{
}

Transaction::~Transaction()
{
    if (active) {
        rollback();
    };
}

/**************************************************
 * Session implementation
 * ************************************************/

Session::Session(const std::string &dbName): conn()
{
    std::cout << "\nCreating session for database " << dbName << "\n";
    int resultCode = SEconnect(&conn, url.c_str(), dbName.c_str(), login.c_str(), password.c_str());
    
    if (resultCode != SEDNA_SESSION_OPEN) {
        throw std::runtime_error("Cannot open session");
    }
    std::cout << "\nSession created succesfully!\n";

    //Setting autocommit mode, although it should be set automatically
    int value = SEDNA_AUTOCOMMIT_ON;
    resultCode = SEsetConnectionAttr(&conn, SEDNA_ATTR_AUTOCOMMIT, &value, sizeof(int));
    if (resultCode != SEDNA_SET_ATTRIBUTE_SUCCEEDED) {
        throw std::runtime_error("Cannot set autocommit");
    }
}

Session::~Session()
{
    std::cout << "\nClosing session\n";
    int resultCode = SEclose(&conn);
    
    if (resultCode != SEDNA_SESSION_CLOSED) {
        throw std::runtime_error("Cannot close session");
    }
}

void Session::execute(Query &query)
{
    /*  After execution, query.resultCode either contains a positive status and 
    *   query.resultSeq contains result, if provided,
    *   or query.resultCode is negative and query.resultSeq contains error message */
    query.resultCode = SEexecute(&conn, query.body.c_str());

    query.resultString = "";
    if (query.resultCode == SEDNA_QUERY_SUCCEEDED) {
        //We can try to obtain result
        int nextItemCode, getDataCode;
        nextItemCode = SEnext(&conn);

        while (nextItemCode == SEDNA_NEXT_ITEM_SUCCEEDED) {
            getDataCode = SEgetData(&conn, resultBuf, Session::resultBufferSize);
            if (getDataCode < 0) {
                break;
            }

            query.resultSequence.push_back(std::string(resultBuf, getDataCode));
            query.resultString += std::string(resultBuf, getDataCode);
            nextItemCode = SEnext(&conn);
        }

        if (nextItemCode != SEDNA_RESULT_END) {
            query.resultCode = SEgetLastErrorCode(&conn);
        }
    }

    if (query.resultCode < 0) {
        query.resultSequence.clear();
        query.resultSequence.push_back(std::string(SEgetLastErrorMsg(&conn)));
        query.resultString = std::string(SEgetLastErrorMsg(&conn));
    }
}

/**************************************************
 * Database implementation
 * ************************************************/

Database::Database(const std::string& _name, const std::string& _sednaBin)
    : name(_name), sednaBin(_sednaBin)
{
}

Database::~Database()
{
    stop();
}

Session *Database::createSession()
{
    Session *tmp = new Session(name);
    sessions.push_back(tmp);
    return tmp;
}

void Database::freeSessions()
{
    for (int i = 0; i < sessions.size(); i++) {
        delete sessions[i];
        sessions[i] = 0;
    }
}

void Database::create() {
    std::cout << "\nCreating database " << name << "\n";
    spawnProcess(sednaBin + CDB_EXECUTABLE + " " + name);
}

void Database::drop()
{
    stop();
    std::cout << "\nDropping database " << name << "\n";
    spawnProcess(sednaBin + DROP_EXECUTABLE + " " + name);
}

void Database::start()
{
    std::cout << "\nStarting database " << name << "\n";
    spawnProcess(sednaBin + SM_EXECUTABLE + " " + name);
}

void Database::stop()
{
    freeSessions();
    std::cout << "\nStopping database " << name << "\n";
    spawnProcess(sednaBin + SMSD_EXECUTABLE + " " + name);
}

/**************************************************
 * SednaInterface implementation
 * ************************************************/

SednaInterface::SednaInterface(const std::string& _sednaBin)
{
    sednaBin = _sednaBin;
}

void SednaInterface::startSedna()
{
    std::cout << "\nStarting governor\n";
    spawnProcess(sednaBin + GOV_EXECUTABLE);
}

Database * SednaInterface::getDatabase(const std::string& name)
{
    Database *tmp = new Database(name, sednaBin);
    databases.push_back(tmp);
    return tmp;
}

void SednaInterface::freeDatabases()
{
    for (int i = 0; i < databases.size(); i++) {
        delete databases[i];
        databases[i] = 0;
    }
}

void SednaInterface::stopSedna()
{
    freeDatabases();

    std::cout << "\nStoping governor\n";
    spawnProcess(sednaBin + STOP_EXECUTABLE);
}

SednaInterface::~SednaInterface()
{
    stopSedna();
}
