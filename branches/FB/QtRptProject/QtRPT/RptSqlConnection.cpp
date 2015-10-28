/*
Name: QtRptConnection
Version: 1.5.5
Web-site: http://www.qtrpt.tk
Programmer: Aleksey Osipov
E-mail: aliks-os@ukr.net
Web-site: http://www.aliks-os.tk

Copyright 2012-2015 Aleksey Osipov

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

#include "RptSqlConnection.h"

RptSqlConnection::RptSqlConnection()
    : m_bIsActive(false)
    , m_dbPort(0)
{
}

RptSqlConnection::RptSqlConnection(QString dsName, QString dbType, QString dbName, QString dbHost, QString dbUser, QString dbPassword, int dbPort, QString dbConnectionName, QString sqlQuery, QString dbCoding, QString charsetCoding)
    : m_bIsActive(true)
    , m_dsName(dsName)
    , m_dbType(dbType)
    , m_dbName(dbName)
    , m_dbHost(dbHost)
    , m_dbUser(dbUser)
    , m_dbPassword(dbPassword)
    , m_dbPort(dbPort)
    , m_dbConnectionName(dbConnectionName)
    , m_sqlQuery(sqlQuery)
    , m_dbCoding(dbCoding)
    , m_charsetCoding(charsetCoding)
{
}

void RptSqlConnection::reset()
{
    m_bIsActive = false;
    m_dsName.clear();
    m_dbType.clear();
    m_dbName.clear();
    m_dbHost.clear();
    m_dbUser.clear();
    m_dbPassword.clear();
    m_dbPort = 0;
    m_dbConnectionName.clear();
    m_sqlQuery.clear();
    m_dbCoding.clear();
    m_charsetCoding.clear();
}

RptSqlConnection & RptSqlConnection::operator=(const RptSqlConnection & other)
{
    if (this != &other)
    {
        m_bIsActive         = other.m_bIsActive;
        m_dsName            = other.m_dsName;
        m_dbType            = other.m_dbType;
        m_dbName            = other.m_dbName;
        m_dbHost            = other.m_dbHost;
        m_dbUser            = other.m_dbUser;
        m_dbPassword        = other.m_dbPassword;
        m_dbPort            = other.m_dbPort;
        m_dbConnectionName  = other.m_dbConnectionName;
        m_sqlQuery          = other.m_sqlQuery;
        m_dbCoding          = other.m_dbCoding;
        m_charsetCoding     = other.m_charsetCoding;
    }

    return *this;
}
