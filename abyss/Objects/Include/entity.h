#ifndef ENTITY_HPP_INCLUDED
#define ENTITY_HPP_INCLUDED

#include "entity_types.h"

class CEntity
{
public:
	virtual ~CEntity() {}
	virtual unsigned long GetDomain() const = 0;
	virtual unsigned long GetType() const = 0;
};

#endif




