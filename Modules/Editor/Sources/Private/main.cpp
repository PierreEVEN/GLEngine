#include <Editor/EditorEngine.h>
#include "Asset/GProperty.h"
#include "EngineLog/engineLog.h"
#include "Asset/GSerialisable.h"

int main()
{

// 	char* data;
// 	unsigned int Length;
// 	GSerialisableTest* test = new GSerialisableTest();
// 	test->RegisterProperties();
// 	test->Serialize(data, Length);
// 
// 
// 
// 	std::cout << test->prop1 << " | " << test->prop2 << " | " << test->prop3 << std::endl;
// 
// 	for (unsigned int i = 0; i < Length; ++i)
// 	{
// 		std::cout << (int)data[i] << ";";
// 	}
// 	std::cout << std::endl;
// 
// 	test->prop1 = 0;
// 	test->prop2 = 0;
// 	test->prop3 = 0;
// 
// 	test->Deserialize(data);
// 
// 	std::cout << test->prop1 << " | " << test->prop2 << " | " << test->prop3 << std::endl;


	new EditorEngine();
	Engine::Start();
	exit(0);
}