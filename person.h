#ifndef PERSON_H
#define PERSON_H

#include <QString>
#include <QDate>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QtGlobal>

#include "Declaration.h"

class Person
{
public:
    Person() :
        m_id(0), m_fName(""), m_sName(""), m_pName(""), m_number("") {}

    QString fullName();

    void clear() {
        m_id         = 0;
        m_fName      = "";
        m_sName      = "";
        m_pName      = "";
        m_number     = "";
    }

    inline int ID() const { return m_id; }
    inline void setID(const int &id) { m_id = id; }

    inline QString name() const { return m_fName; }
    inline void setName(const QString &name) { m_fName = name; }

    inline QString lastName() const { return m_sName; }
    inline void setLastName(const QString &lastName) { m_sName = lastName; }

    inline QString patronymic() const { return m_pName; }
    inline void setPatronymic(const QString &patronymic) { m_pName = patronymic; }

    inline QString number() const { return m_number; }
    inline void setNumber(const QString &number) { m_number = number; }

    virtual bool selectNSP(QSqlDatabase&) = 0;
    virtual bool selectNSPN(QSqlDatabase&) = 0;
    virtual bool selectAll(QSqlDatabase&) = 0;

protected:
    int m_id;
    QString m_fName;
    QString m_sName;
    QString m_pName;
    QString m_number;
};

#endif // PERSON_H
