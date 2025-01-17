/* 
 * Copyright (C) 2005,2006,2007 MaNGOS <http://www.mangosproject.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "DatabaseEnv.h"
#include "Config/ConfigEnv.h"

#include <ctime>
#include <iostream>
#include <fstream>

bool Database::Initialize(const char *)
{
    // Enable logging of SQL commands (usally only GM commands)
    // (See method: PExecuteLog)
    m_logSQL = sConfig.GetIntDefault("LogSQL", 0);
    return true;
}

void Database::ThreadStart()
{
}

void Database::ThreadEnd()
{
}

void Database::escape_string(std::string& str)
{
    if(str.size()==0)
        return;

    char* buf = new char[str.size()*2+1];
    escape_string(buf,str.c_str(),str.size());
    str = buf;
    delete[] buf;
}

bool Database::PExecuteLog(const char * format,...)
{
    if (!format)
        return false;

    va_list ap;
    char szQuery [1024];
    va_start(ap, format);
    int res = vsnprintf( szQuery, 1024, format, ap );
    va_end(ap);

    if(res==-1)
    {
        sLog.outError("SQL Query truncated (and not execute) for format: %s",format);
        return false;
    }

    if( m_logSQL )
    {
        time_t curr;
        tm local;
        time(&curr);                                            // get current time_t value
        local=*(localtime(&curr));                              // dereference and assign
        char fName[128];
        sprintf( fName, "%04d-%02d-%02d_logSQL.sql", local.tm_year+1900, local.tm_mon+1, local.tm_mday );

        std::fstream log_file ( fName, std::ios::app );

        if ( !log_file.is_open() )
        {
            // The file could not be opened
            sLog.outError("SQL-Logging is disabled - Log file for the SQL commands could not be openend: %s",fName);
        }
        else
        {
            // Safely use the file stream
            log_file << szQuery << "\n";
            log_file.close();
        }
    }

    return Execute(szQuery);
}
