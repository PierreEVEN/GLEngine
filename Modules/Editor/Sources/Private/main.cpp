#include <Editor/EditorEngine.h>
#include <Property/GSerialisable.h>
#include <Property/GProperty.h>

int main()
{

	char* data;
	unsigned int Length;
	GSerialisableTest* test = new GSerialisableTest();
	test->RegisterProperties();
	test->Serialize(data, Length);

	std::cout << test->prop1 << " | " << test->prop2 << " | " << test->prop3 << std::endl;

	test->prop1 = 0;
	test->prop2 = 0;
	test->prop3 = 0;

	test->Deserialize(data);

	std::cout << test->prop1 << " | " << test->prop2 << " | " << test->prop3 << std::endl;


	new EditorEngine();
	Engine::Start();
	exit(0);
}