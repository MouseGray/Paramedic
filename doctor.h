#ifndef DOCTOR_H
#define DOCTOR_H

#include <QSqlQuery>
#include "person.h"

class Doctor : public Person
{
public:
    Doctor() :
        Person(), m_spec("") {}

    QString specialization() const { return m_spec; }
    void setSpecialization(const QString &specialization) { m_spec = specialization; }

    bool selectNSP(QSqlDatabase &datebase) override;
    bool selectNSPN(QSqlDatabase &datebase) override;
    bool selectAll(QSqlDatabase &datebase) override;

    bool updateAll(QSqlDatabase &datebase);
private:
    QString m_spec;
};

#endif // DOCTOR_H
