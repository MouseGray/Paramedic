#ifndef CONFIGSYSTEM_H
#define CONFIGSYSTEM_H

#include <QString>
#include <QFile>
#include <QTextStream>

class ConfigSystem
{
public:
    ConfigSystem();
    void Load();

    QString host() const;

    QString database() const;

    QString login() const;

    QString password() const;

private:
    void Create();
    void Read();

    QString m_host;
    QString m_database;
    QString m_login;
    QString m_password;

    const QString cfgName = "config.cfg";
};

#endif // CONFIGSYSTEM_H
