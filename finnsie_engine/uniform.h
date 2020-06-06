#ifndef UNIFORM_HG_
#define UNIFORM_HG_

#include <string>
#include <vector>

namespace finnsie {

	// TODO(CK): Might change this to
	/*
		class Uniform
			int integer
			float floating
			bool boolean

		instead of using polymorphism just use the std::string type
		cause thats basically what its doing the type is pointless 
		in this heirarchy cause thats what is being determined i just 
		dont want to do type checking in the gui cause thats slow to build
		the widgets
	
	*/
	class Uniform
	{
	public:
		int value;
		std::string guiType;
		std::string name;

		Uniform()
		{
			value = 0;
			guiType = "";
			name = "";
		}

		Uniform(int value, std::string name, std::string type)
		{
			this->value = value;
			this->name = name;
			this->guiType = type;
		}

		Uniform(std::string name, std::string type)
		{
			this->name = name;
			this->guiType = type;
		}
	};

	class BoolUniform : public Uniform
	{
	public:
		bool value;
		BoolUniform(bool value, std::string name, std::string type) : Uniform(name, type)
		{
			this->value = value;
		}
	};

	class FloatUniform : public Uniform
	{
	public:
		float value;
		FloatUniform(float value, std::string name, std::string type) : Uniform(name, type)
		{
			this->value = value;
		}
	};
}


#endif