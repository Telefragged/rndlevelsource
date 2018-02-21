#pragma once

#include <string>

class CompileTools
{
public:

	static std::string getabspath(std::string partialpath);
	static std::string removeext(std::string file);

	static void exec(const std::string& binarypath, const std::string& param, bool showoutput = true);
	static void copyfile(const std::string& existing, const std::string& newfile, bool confirm = false);
	static void cleanup(const std::string& vmfpath);

	static void runtools(std::string file, std::string gamedir);

	CompileTools() = default;
	~CompileTools() = default;
};

