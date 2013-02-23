#ifndef SECTION_HPP

#define SECTION_HPP

#include "atomic.h"
#include "system_commands.h"
#include "queue.h"
#include <hash_map>
#include <map>
#include <boost/shared_ptr.hpp>


class CIDSource
{
public:
	virtual long GetThisID() const = 0;
};

struct abstract_scheduler 
{
	virtual void schedule( const message& m) =0;
};

struct thread;

struct abstract_thread_owner
{
	virtual void setThread(thread* p) = 0;
protected:
	virtual thread*     get_thread()  = 0; 
};


typedef std::hash_map< long , ConformFunc > reaction_map;


/*
	optional class which provides requirements for 
	startup section 
*/
struct startup_section_config
{
	int m_thread_count;
};

class section : public virtual CIDSource
	,public virtual abstract_thread_owner
	,public virtual abstract_scheduler
	//,public CTCommandSender<CCommand_SectionQuit>
{
public:
	
	virtual startup_section_config configure()
	{
		startup_section_config cfg;
		cfg.m_thread_count = 8;
		return cfg;

	}
	section(long id ) : m_busy_flag(0),m_pthread(NULL), m_pbound_thread(NULL)
	{
		m_id = id;
		m_running_flag.set(1);
		m_CurrentMagic = 0;
		m_psection_queue = boost::shared_ptr< MSQueue<message> >( new MSQueue<message>() );
	}
	virtual ~section();
	ConformFunc GetReaction(long in_CommandID)
	{
		reaction_map::iterator it = m_reactions.find(in_CommandID);
		if (it == m_reactions.end() ) 
		{
			return NULL;
		}
		return it->second;
	}

	void Quit();
	virtual void schedule( const message& m);
	void setThread(thread* pThread);
	long GetThisID() const 
	{
		return m_id;
	}
	virtual std::string GetName()
	{
		return "Default";
	}

	void InitSubSections();
	//virtual void schedule( const message& m, int thread = -1);

	virtual void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand ) = 0;
	long m_flags;
	volatile long m_busy_flag;
	volatile thread * m_pbound_thread;
	atomic_long m_running_flag;
	boost::shared_ptr< MSQueue<message> > m_psection_queue;
	typedef std::hash_map<long, section*> sub_section_type;
	
	const sub_section_type& GetSubSections() const 
	{
		return m_sub_sections;
	}
protected:
	void InstallThreadProcs(IdleProc idleProc, KickProc kickProc);
	void RegisterSubSection(long in_CLSID, section* pSection);
	void RegisterReaction( long in_CommandID, ConformFunc in_pFunc );	
	thread * get_thread() 
	{
		return m_pthread;
	}
	thread* m_pthread;

	
	struct CReply{
				CReply() : m_Magic(0), m_OrigSection(-1)
				{
				}
				long m_Magic;
				long m_OrigSection;
	};
	long m_CurrentMagic;
			std::map<long, CReply> m_Reply;
			long CreateReply(long magic, long srcSection)
			{
				CReply reply;
				reply.m_Magic =magic;
				reply.m_OrigSection = srcSection;
				m_CurrentMagic++;
				m_Reply[m_CurrentMagic] = reply;
				
				return m_CurrentMagic;
			}
			CReply GetReply(long magic)
			{
				CReply reply;
				std::map<long, CReply>::iterator it = m_Reply.find(magic);
				if (it == m_Reply.end() ) 
				{
					return reply;
				}
				return it->second;
			}
			void RemoveReply(long magic) 
			{
				m_Reply.erase(magic);
			}

	
protected:
	sub_section_type m_sub_sections;
	reaction_map m_reactions;
	long m_id;
};

typedef section CSection;

class CSubSection : public CSection
{
public:
	CSubSection() : section(-1) 
	{
	}
	virtual void Reaction( long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
	{
	}
};

template <class T> 
struct CTCommandSender : public virtual CIDSource
	, public virtual abstract_scheduler
	, public virtual abstract_thread_owner
{
	
	CTCommandSender()
	{
		m_type_id  = GetID( (T*) NULL);
	};
	void SendCommand(long dest, const T& val)
	{
		CLog::Print( "Sending Command %s \n", typeid(T).name() );
		const section_map* map = get_thread()->m_section_map_subscriber.get_property();
		if (dest == SECTION_ID_BROADCAST) 
		{
			//if (true ) return;
			//const section_map* map = get_thread()->m_section_map_subscriber.get_property();
			section_map::const_iterator it = map->begin();
			while (it != map->end() ) 
			{
				if (it->second->GetReaction(m_type_id) ) 
				{
					long newDest = it->second->GetThisID();
					message m;
					m.m_clock = clock();
					m.m_destination = newDest;
					m.m_source = GetThisID();
					m.m_type = m_type_id;
					T* tPtr = static_cast<T*>(get_thread()->m_thread_allocator.allocate( sizeof(T)));
					m.pCommand = new (tPtr) T(val);
					m.pAllocator = get_thread()->m_thread_allocator.get_chunk();
					
					schedule(m);
					//it->second->m_psection_queue->enqueue( m , get_thread()->m_thread_allocator);
					
					
				}
				it++;
			}

		}
		else
		{
			section_map::const_iterator iTarget =  map->find(dest);
			if (iTarget != map->end()) 
			{
				message m;
				m.m_clock = clock();
				m.m_destination = dest;
				m.m_source = GetThisID();
				m.m_type = m_type_id;
				T* tPtr = static_cast<T*>(get_thread()->m_thread_allocator.allocate( sizeof(T)));
				m.pCommand = new (tPtr) T(val);
				m.pAllocator = get_thread()->m_thread_allocator.get_chunk();
			//;	iTarget->second->m_psection_queue->enqueue( m , get_thread()->m_thread_allocator);
				schedule(m);
			}
			else
			{
				assert(0&& "No dest found");
			}
		}
		
	}

private:
	int m_type_id;
};


#define REGISTER_REACTION(S,R) RegisterReaction( GetID((R*)NULL), CTConform<S,R >::Conform )

#endif