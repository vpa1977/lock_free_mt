#ifndef ANIMATION_CONTROLLER_HPP_INCLUDED
#define ANIMATION_CONTROLLER_HPP_INCLUDED

#include "object.h"
#include "../../Common/Include/animation.h"
#include <deque>

class CAnimationController : public CController< CObject >
{
public:
	CAnimationController();
	CAnimationController( const std::deque< CAnimationDescription_Matrix >& in_rDesc );
	CAnimationController( const std::deque< CAnimationDescription_Quaternion >& in_rDesc );
	~CAnimationController();

////////////////////////////////////////
// virtual from CController
////////////////////////////////////////
	// Pri attach-e k objektu proverit` chto tam deistvitel`no skeletno animirovannij mesh.
	// Inache - vernut` false pri pervom Process()-e.
	void Added( CObject& in_rHost );
	//
	void Removed( CObject& in_rHost );
	// Podvigat` vse kosti soglasno aktivnim animacijam.
	bool Process( CObject& in_rHost );
	const std::string& GetName() const {
		static std::string Name("AnimationController");
		return Name;
	}
////////////////////////////////////////

	bool AddAnimation( const CAnimationDescription_Matrix& in_rAni );
	bool AddAnimation( const CAnimationDescription_Quaternion& in_rAni );
	long FindAnimationByName( const std::string& in_rName );
	bool Play( long in_AnimationID, bool in_bLoop = false, float in_StartPosition = 0.0f );
	bool Stop( long in_AnimationID );
	bool SeekTo( long in_AnimationID, float in_Where );
};


#endif


































