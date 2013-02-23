#ifndef JAVA_GATE_HPP

#define JAVA_GATE_HPP

#include "jni.h"

// will send commands to renderer to display our components. 
// will recieve events from input and dispatch it to our components.
class CJavaGate 
{
public:
	bool InitJVM();
	bool DestroyJVM();

	void Poll();
	

private:
	JavaVM *jvm; 
	JNIEnv *env; 
}

#endif 