#pragma once
#include <map>
#include <string>
#include "Shader.h"

class ShaderList
{
	public:
		ShaderList(void) {
		};
		ShaderList(std::string name, std::string vertexShader, std::string fragmentShader) {
			addShader(name, vertexShader, fragmentShader);
		};
		~ShaderList() {
			for (auto const& x : shaderMap)
			{
				if (x.second != NULL)
					delete x.second;
			}
		};
		Shader * getShader(std::string name) {
			return this->shaderMap[name];
		};
		void addShader(std::string name, std::string vertexShader, std::string fragmentShader) {
			this->shaderMap[name] = new Shader(vertexShader, fragmentShader);
		};
		void deleteShader(std::string name) {
			delete shaderMap[name];
			this->shaderMap.erase(name);
		};

	private:
		std::map<std::string, Shader *> shaderMap;	
};

