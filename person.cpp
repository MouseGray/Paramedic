#include "person.h"




QString Person::fullName()
{
    if(m_sName.isEmpty()) return "";
    return m_sName + " " + m_fName + " " + m_pName;
}
