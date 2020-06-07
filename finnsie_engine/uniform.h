#ifndef UNIFORM_HG_
#define UNIFORM_HG_

#include <string>
#include <vector>

namespace finnsie {

	// NOTE(CK): Instead of having a templated class this Uniform just holds 
	// three types this way it can be changed on the fly and allows the vector
	// to just use a Uniform object and not be templated

	// TODO(CK): Possibly figure out a templated solution
	class Uniform
	{
	public:
		std::string guiType;
		std::string name;

		Uniform()
		{
			integer = 0;
			floating = 0.0f;
			boolean = false;
			guiType = "";
			name = "";
		}

		Uniform(int value, std::string name, std::string type)
		{
			this->integer = value;
			this->name = name;
			this->guiType = type;

			this->floating = 0.0f;
			this->boolean = false;
		}

		Uniform(float value, std::string name, std::string type)
		{
			this->floating = value;
			this->name = name;
			this->guiType = type;

			this->integer = 0.0;
			this->boolean = false;
		}

		Uniform(bool value, std::string name, std::string type)
		{
			this->boolean = value;
			this->name = name;
			this->guiType = type;

			this->floating = 0.0f;
			this->integer = 0.0;
		}


		Uniform(std::string name, std::string type)
		{
			this->name = name;
			this->guiType = type;
		}

		int GetInt()
		{
			return this->integer;
		}

		float GetFloat()
		{
			return this->floating;
		}

		bool GetBool()
		{
			return this->boolean;
		}

	private:
		int integer;
		float floating;
		bool boolean;
	};
}


#endif