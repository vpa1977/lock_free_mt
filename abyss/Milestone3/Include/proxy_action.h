#ifndef PROXY_ACTION_HPP

#define PROXY_ACTION_HPP

#include <map>


template< typename K, typename T >
class ProxyAction
{
public:
	void RegisterProxiedAction( const K& key , const T& user )
	{
		std::map<K,T>::iterator it = m_ProxiedActions.find(key);
		assert( it==m_ProxiedActions.end() );
		m_ProxiedActions.insert( std::map<K,T>::value_type(key,user) );
	}
	T PopProxiedAction( const K& key ) 
	{
		std::map<K,T>::iterator it = m_ProxiedActions.find(key);
		if (it != m_ProxiedActions.end() ) 
		{
			T result = it->second;
			m_ProxiedActions.erase(key);
			return result;
		}
		return T();
	}

private:
	std::map<K,T> m_ProxiedActions;
};


#endif