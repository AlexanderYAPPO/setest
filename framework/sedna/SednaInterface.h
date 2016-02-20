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


#ifndef SEDNAINTERFACE_H
#define SEDNAINTERFACE_H

#include <string>
#include <vector>
#include <libsedna.h>

namespace sedna {

struct Query
{
private:
    friend class Session;
    friend class Transaction;

public:
    std::string body;

    std::vector<std::string> resultSequence; //Stores either result or error message
    std::string resultString;
    int resultCode;

    struct Filename { const std::string & filename; Filename(const std::string & _filename) : filename(_filename) {}; };
    struct Body { const std::string & body; Body(const std::string & _body) : body(_body) {}; };

    Query(const Filename &filename);
    Query(const Body & body);
};

class Session;

class Transaction
{
    bool active;
    Session *session;
public:
    Transaction(Session *_session);
    ~Transaction();

    void execute(Query &query);
    void commit();
    void rollback();

    bool isActive() { return active; };
};

class Session
{
    static const std::string url;
    static const std::string login;
    static const std::string password;

    SednaConnection conn;

public:
    const static int resultBufferSize = 10000;
    
    Session(const std::string &dbName);
    ~Session();

    void execute(Query &query);
};

class Database
{
    static const std::string CDB_EXECUTABLE;
    static const std::string SM_EXECUTABLE;
    static const std::string SMSD_EXECUTABLE;
    static const std::string DROP_EXECUTABLE;

    std::string name;
    std::string sednaBin;
    
    std::vector <Session *> sessions;
public:
    Database(const std::string & _name, const std::string & _sednaBin);
    ~Database();
    
    void freeSessions();

    void create();
    void start();
    void stop();
    void drop();

    Session *createSession();
};

class SednaInterface
{
    static const std::string GOV_EXECUTABLE;
    static const std::string STOP_EXECUTABLE;
    
    std::string sednaBin;
    std::vector <Database *> databases;
    
public:
    SednaInterface(const std::string & _sednaBin);
    virtual ~SednaInterface();

    void freeDatabases();
    
    void startSedna();
    void stopSedna();

    Database *getDatabase(const std::string & name);
};

}

#endif // SEDNAINTERFACE_H
