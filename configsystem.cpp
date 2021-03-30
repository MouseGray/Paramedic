#include "configsystem.h"

ConfigSystem::ConfigSystem() :
    m_host("ADMIN-PC\\SQLEXPRESS"),
    m_database("Paramedic10"),
    m_login("Owner"),
    m_password("ow1251")
{

}

QString ConfigSystem::host() const
{
    return m_host;
}

QString ConfigSystem::database() const
{
    return m_database;
}

QString ConfigSystem::login() const
{
    return m_login;
}

QString ConfigSystem::password() const
{
    return m_password;
}

void ConfigSystem::Load()
{
    if(QFile::exists(this->cfgName)) Read();
    else Create();
}

void ConfigSystem::Read()
{
    QFile file(this->cfgName);
    if(file.open(QIODevice::ReadOnly)){
        QTextStream stream(&file);
        QString line;
        QStringList list;
        while(!(line = stream.readLine()).isEmpty()){
            list = line.split('=');
            if(list.size() != 2) continue;
            list[0] = list[0].trimmed();
            list[1] = list[1].trimmed();
                 if(list[0] == "host")      this->m_host        = list[1];
            else if(list[0] == "database")  this->m_database    = list[1];
            else if(list[0] == "login")     this->m_login       = list[1];
            else if(list[0] == "password")  this->m_password    = list[1];
        }
        file.close();
    }
}

void ConfigSystem::Create()
{
    QFile file(this->cfgName);
    if(file.open(QIODevice::WriteOnly)){
        QTextStream stream(&file);
        stream << "host = "     << this->m_host     << "\r\n";
        stream << "database = " << this->m_database << "\r\n";
        stream << "login = "    << this->m_login    << "\r\n";
        stream << "password = " << this->m_password << "\r\n";
        stream.flush();
        file.close();
    }
}
