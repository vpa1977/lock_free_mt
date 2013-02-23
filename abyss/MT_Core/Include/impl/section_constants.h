#ifndef SECTION_FLAGS_H_INCLUDED
#define SECTION_FLAGS_H_INCLUDED


#define REGULAR_SECTION false
#define STARTUP_SECTION true

#define MULTIPLE_INSTANCES_ALLOWED	true
#define SINGLE_INSTANCE_ONLY		false


#define SECTION_ID_ERROR	-1
#define SECTION_ID_SYSTEM			0
#define SECTION_ID_BROADCAST		1
#define SECTION_ID_FIRST_AVAILABLE	1024

// Regular section.
// Sheduler can run it on any available thread.
#define SF_NONE				0x0

// Sheduler finds some thread X for this section,
//	and later runs this section only on thread X,
// However, user code can explicitly launch other "Bound" sections
//	to run together with this section. See SF_PARTNER.
#define SF_BOUND			0x1

// Requires SF_BOUND.
// Sheduler finds some thread X for this section,
//	and later does not run any other regular sections on thread X.
#define SF_EXCLUSIVE		0x2

// Requires SF_BOUND.
// Indicates that user code requests to run this section
//	on the same thread as specific section Y.
#define SF_PARTNER			0x4

// Requires SF_BOUND.
// Must not be used with SF_EXCLUSIVE or SF_PARTNER.
// Indicates that user code requests to run this section
//	on the thread without any SF_BOUND-flagged sections.
#define SF_SEPARATE			0x8


enum SECTION_LAUNCH_RESULT
{
	SLR_OK						= 0,
	SLR_ERROR					= 1, // unspecified error
	SLR_UNKNOWN_SECTION			= 2, // unknown CLSID or section ID
	SLR_FLAGS					= 3, // Conflicting flags. Either in launch flags themselves, or between launch flags and section registrator flags.
	SLR_FLAGS_ARE_TOO_HARD		= 4, // Flags are ok, but the system cannot launch the section in the way user code requests.
	SLR_PARTNER_IS_NOT_BOUND	= 5, // Partner section is not bound.
	SLR_INSTANCE				= 6, // Only one instance for this particular CLSID is allowed.
	SLR_FORCE_DWORD	= 0xffffffff
};


#define SYSTEM_CLSID 1 // 

#define SYSTEM_SECTION_ID 0



#endif