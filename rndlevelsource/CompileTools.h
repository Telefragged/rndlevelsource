#pragma once

class CompileTools
{
public:

	static std::string getabspath(std::string partial);
	static std::string removeext(std::string file);

	static void exec(std::string exec, std::string param, bool output = true);
	static void copyfile(std::string original, std::string copy, bool confirm = false);
	static void cleanup(std::string);

	static void runtools(std::string vmffile, std::string gamedir);

	CompileTools(void);
	~CompileTools(void);
};

