#include "CompileTools.h"

#include <string>
#include <stdlib.h>

#if defined _WIN32
#include <Windows.h>
#endif

std::string CompileTools::getabspath(std::string partialpath)
{
	std::string ret;
	//char buf[256];
	//ret = buf;
	
	return ret;
}

std::string CompileTools::removeext(std::string file)
{
	return file.substr(0, file.find_last_of('.'));
}

void CompileTools::runtools(std::string file, std::string game)
{
#if defined (_WIN32)
	std::string outfileabs = getabspath(file);
	std::string outfilebsp = removeext(outfileabs) + ".bsp";
	exec("F:\\Program Files (x86)\\Steam\\steamapps\\common\\Half-Life 2\\bin\\vbsp.exe",
		std::string() + "-game \"" + game + "\" \"" + outfileabs + "\"");
	exec("F:\\Program Files (x86)\\Steam\\steamapps\\common\\Half-Life 2\\bin\\vvis.exe",
		std::string() + "\"" + outfilebsp + "\"");
	exec("F:\\Program Files (x86)\\Steam\\steamapps\\common\\Half-Life 2\\bin\\vrad.exe",
		std::string() + "-game \"" + game + "\" \"" + outfilebsp + "\"");
	copyfile(outfilebsp,
		"F:\\Program Files (x86)\\Steam\\steamapps\\common\\Half-Life 2\\ep2\\maps\\roomwrite.bsp",
		false);
#endif
}

void CompileTools::exec(std::string binpath, std::string param, bool showoutput)
{
#if defined (_WIN32)
	STARTUPINFO info={sizeof(info)};
	PROCESS_INFORMATION processInfo;
	char params[8192];
	param = std::string("\"") + binpath + "\"" + ' ' + param;
	printf("Executing %s\n\n", param.c_str());
	strcpy_s(params, param.c_str());
	DWORD creationflags = 0;
	if(!showoutput) creationflags = CREATE_NO_WINDOW;
	BOOL result = CreateProcess(
		nullptr,
		params,
		nullptr,
		nullptr,
		TRUE,
		creationflags,
		nullptr,
		nullptr,
		&info,
		&processInfo);
	if(result) {
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	} else {
		printf("Could not open %s\n", binpath.c_str());
	}
#endif
}

void CompileTools::cleanup(std::string vmfpath)
{
#if defined (_WIN32)
	std::string outfilevmf = getabspath(vmfpath);
	std::string outfilenoext = removeext(outfilevmf);
	printf("Removing files %s.*\n", outfilenoext.c_str());
	std::string outfilebsp = outfilenoext + ".bsp";
	std::string outfilelog = outfilenoext + ".log";
	std::string outfileprt = outfilenoext + ".prt";
	if(!DeleteFile(outfilevmf.c_str())) printf("Error deleting file %s\n", outfilevmf.c_str());
	if(!DeleteFile(outfilebsp.c_str())) printf("Error deleting file %s\n", outfilebsp.c_str());
	if(!DeleteFile(outfilelog.c_str())) printf("Error deleting file %s\n", outfilelog.c_str());
	if(!DeleteFile(outfileprt.c_str())) printf("Error deleting file %s\n", outfileprt.c_str());
#endif
}

void CompileTools::copyfile(std::string existing, std::string newfile, bool confirm)
{
#if defined (_WIN32)
	printf("Copy: %s -> %s\n", existing.c_str(), newfile.c_str());
	BOOL result = CopyFile(existing.c_str(), newfile.c_str(), confirm);
	if(!result && confirm) {
		char res = 0;
		while(res != 'y' && res != 'n' && res != 'Y' && res != 'N') {
			printf("Overwrite %s? (y/n): ", newfile.c_str());
			scanf_s("%c", &res, (int)sizeof(char));
		}
		switch(res) {
		case('y'):
		case('Y'):
			CopyFile(existing.c_str(), newfile.c_str(), FALSE);
			break;
		}
	}
#endif
}

CompileTools::CompileTools(void)
{
}

CompileTools::~CompileTools(void)
{
}

