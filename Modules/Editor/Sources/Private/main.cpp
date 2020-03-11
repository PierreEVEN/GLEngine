#include <Editor/EditorEngine.h>
#include "Asset/GProperty.h"
#include "EngineLog/engineLog.h"

int main()
{
	char* result = nullptr;

	double VariableTest = 12.5;

	GProperty prop = GPROPERTY(VariableTest);
	
	prop.Serialize(result);
	GProperty resultProp = GProperty(result); // Deserialize


	std::cout << "avant serialisation : " << prop.ToString() << "value = " << *prop.Get<double>() << std::endl;
	std::cout << "apres serialisation : " << resultProp.ToString() << "value = " << *resultProp.Get<double>() << std::endl;





	while(true) {}

	return 0;

	new EditorEngine();
	Engine::Start();
	exit(0);



}