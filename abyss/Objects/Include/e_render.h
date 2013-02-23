#ifndef ENTITY_RENDER_HPP_INCLUDED
#define ENTITY_RENDER_HPP_INCLUDED

#include "../../Common/Include/box.h"
#include "entity_types.h"

class CEntity_RenderDomain : public CEntity
{
public:
	virtual ~CEntity_RenderDomain() {}
	unsigned long GetDomain() const { return ED_RENDER; }
	const CBox& GetBox() const { return vGetBox(); }
protected:
	virtual const CBox& vGetBox() const = 0;
};

#endif


































