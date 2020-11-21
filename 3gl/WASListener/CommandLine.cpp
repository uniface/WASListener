#include "CommandLine.h"
#include <filesystem>
#include <iostream>
#include <boost/program_options.hpp>

using namespace std::string_literals;

// Success: true , Failure: false
bool CCommandLine::_initConsole() noexcept
{
	if (!AllocConsole()) { return false; }
	if (freopen_s(&_g_ic_file_cout_stream, "CONOUT$", "w", stdout) != 0) { return false; } // For std::cout 
	if (freopen_s(&_g_ic_file_cin_stream, "CONIN$", "w+", stdin) != 0) { return false; } // For std::cin
	return true;
}

std::optional<std::string> CCommandLine::_convertToShortName(std::string const& pathToMakeShort)
{
	std::optional<std::string> result;
	// First obtain the size needed by passing NULL and 0.
	auto dwLength = GetShortPathNameA(pathToMakeShort.c_str(), nullptr, 0);
	if (dwLength)
	{
		// Dynamically allocate the correct size 
		// (terminating null char was included in length)
		std::string szShortPathName;
		szShortPathName.reserve(dwLength);
		
		// Now simply call again using same long path
		dwLength = GetShortPathNameA(pathToMakeShort.c_str(), szShortPathName.data(), dwLength);
		if (dwLength)
		{
			result = szShortPathName.data();
		}
	}
	return result;
}

void CCommandLine::_setUnifaceCommandOptions(std::string const& admFolder, std::string const& iniFile)
{
	std::string cmdLine;
	if (!admFolder.empty())
	{
		auto const shortAdmFile{ _convertToShortName(admFolder) };
		if (!shortAdmFile.has_value())
		{
			throw std::invalid_argument{ "Adm folder not found: "s + admFolder };
		}
		cmdLine = "/adm="s + shortAdmFile.value();
	}

	if (!iniFile.empty())
	{
		auto const shortInitFile{ _convertToShortName(iniFile) };
		if (!shortInitFile.has_value())
		{
			throw std::invalid_argument{ "Initialisation file not found: "s + iniFile };
		}
		if (!cmdLine.empty())
		{
			cmdLine += " "s;
		}
		cmdLine += "/ini="s + shortInitFile.value();
	}

	m_unifaceCommandOptions = cmdLine;
}

void CCommandLine::_setUnifaceAsnFile(std::string const& asnFile)
{
	if (asnFile.empty())
		return;
	auto const optionalAsnFile{ _convertToShortName(asnFile) };
	if (!optionalAsnFile.has_value())
	{
		throw std::invalid_argument{ "Assignment file not found: "s + asnFile };
	}
	m_unifaceAsnFile = optionalAsnFile.value();
}

bool CCommandLine::parse(LPCSTR lpCommandLine)
{
	std::filesystem::path exePath;
	std::filesystem::path projectFolder;
	std::filesystem::path admFolder;
	std::filesystem::path asnFile;
	std::filesystem::path iniFile;

	boost::program_options::options_description desc{ "Options" };
	desc.add_options()
		("uniface-adm-folder,a", boost::program_options::value<std::string>(), "Uniface ADM folder")
		("uniface-ini-file,i", boost::program_options::value<std::string>(), "Uniface ini file")
		("uniface-asn-file,f", boost::program_options::value<std::string>(), "Uniface asn file")
		("project-folder,p", boost::program_options::value<std::string>(), "Start In folder")
		("help,h", "produce help message");

	boost::program_options::variables_map vm;
	auto args = boost::program_options::split_winmain(lpCommandLine);
	store(boost::program_options::command_line_parser(args).options(desc).run(), vm);

	notify(vm);

	if (vm.count("help"))
	{
		if (_initConsole())
		{
			std::cout << desc << "\n";
			system("pause");
		}

		return false;
	}

	// The working folder is either set using the --dir flag or the current folder
	if (vm.count("project-folder"))
	{
		projectFolder = std::filesystem::path(vm["project-folder"].as<std::string>());
		// Does the path set via --project-folder exist?
		if (!exists(projectFolder) || !is_directory(projectFolder))
			throw std::runtime_error{ "(--project-folder) Unable to find folder "s + projectFolder.string() };

		// The path set by --project-folder does exist so set it as the working folder
		current_path(projectFolder);
	}

	// The adm folder is either specified on the commend line or relative to the exe
	if (vm.count("uniface-adm-folder"))
	{
		admFolder = std::filesystem::path(vm["uniface-adm-folder"].as<std::string>());
		// Does the path set via --uniface-adm-folder exist?
		if (!is_directory(admFolder))
			throw std::runtime_error{ "(--uniface-adm-folder) Unable to find folder "s + admFolder.string() };
	}
	else
	{
		// No ADM folder supplied on the command line
		// 1) look for an adm folder under uniface (relative to exe in the bin folder)
		char filename[MAX_PATH + 1];
		GetModuleFileNameA(nullptr, filename, MAX_PATH);

		exePath = std::filesystem::absolute(filename).remove_filename();
		admFolder = weakly_canonical(exePath / ".." / ".." / "uniface" / "adm").make_preferred();
		if (!is_directory(admFolder))
		{
			admFolder.clear();
		}
	}

	if (vm.count("uniface-ini-file"))
	{
		iniFile = std::filesystem::path(vm["uniface-ini-file"].as<std::string>());
		// Does the ini file set via --uniface-ini-file exist?
		if (exists(iniFile))
			iniFile = canonical(iniFile);
		else if (exists(admFolder / iniFile)) // (could be in the ADM folder)
			iniFile = canonical(admFolder / iniFile);
		else
			throw std::runtime_error{ "(--uniface-ini-file) Unable to find file "s + iniFile.string() };
	}

	_setUnifaceCommandOptions(admFolder.string(), iniFile.string());

	// look for an asn file
	if (vm.count("uniface-asn-file"))
		asnFile = std::filesystem::path(vm["uniface-asn-file"].as<std::string>());
	else
	{
		// 1) look for an asn file in the working folder
		asnFile = (std::filesystem::current_path() / "waslistener.asn").make_preferred();
		if (!exists(asnFile))
		{
			// 2) look for an asn file in the adm folder
			asnFile = (admFolder / "waslistener.asn").make_preferred();
			if (!exists(asnFile))
				asnFile.clear(); // Use the standard Uniface handling
		}
	}
	try
	{
		_setUnifaceAsnFile(asnFile.string());
	}
	catch (std::string const& msg)
	{
		MessageBoxA(nullptr, msg.c_str(), "Error", MB_OK);
		return false;
	}
	return true;
}

USTRING* CCommandLine::getUnifaceCommandLine() noexcept
{
	return reinterpret_cast<USTRING*>(m_unifaceCommandOptions.data());
}

USTRING* CCommandLine::getUnifaceAsnFile() noexcept
{
	return reinterpret_cast<USTRING*>(m_unifaceAsnFile.data());
}
