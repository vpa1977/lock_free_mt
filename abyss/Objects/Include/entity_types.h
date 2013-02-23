#ifndef ENTITY_TYPES_HPP_INCLUDED
#define ENTITY_TYPES_HPP_INCLUDED

enum ENTITY_DOMAIN {
	ED_NONE		= 0,
	ED_RENDER	= 1,
	ED_SOUND	= 2
	// ED_PHYSICS, etc.
};

enum ENTITY_TYPE {

	// dummy
	ET_DUMMY					= 0,

	// render-related
	ET_RENDER_RELATED_BEGIN		= 1000,
	ET_STATIC_MESH				= 1001,
	ET_ANIMATION                = 1002,
	ET_RENDER_RELATED_END		= 1999,

	// sound-related
	ET_SOUND_RELATED_BEGIN		= 2000,
	ET_SOUND_RELATED_END		= 2999

};

#endif


