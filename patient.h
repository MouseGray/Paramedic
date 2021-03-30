#ifndef PATIENT_H
#define PATIENT_H

#include "person.h"

class Patient : public Person
{
public:
    enum Gender { Male, Female };

    Patient() :
        Person(), m_gender(Male), m_dateOfBirth(QDate(2000, 0, 0)) {}

    Gender gender() const { return m_gender; }
    void setGender(const Gender &gender) { m_gender = gender; }

    QDate dateOfBirth() const { return m_dateOfBirth; }
    void setDateOfBirth(const QDate &dateOfBirth) { m_dateOfBirth = dateOfBirth; }

    bool selectNSP(QSqlDatabase &datebase) override;
    bool selectNSPN(QSqlDatabase &datebase) override;
    bool selectAll(QSqlDatabase &datebase) override;

    bool updateAll(QSqlDatabase &datebase);
private:
    Gender m_gender;
    QDate m_dateOfBirth;
};

#endif // PATIENT_H
