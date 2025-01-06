#include "Util.hpp"

#include <shlobj.h>

namespace Windows::Util
{
	std::filesystem::path documentsPath()
	{
		std::filesystem::path path;
		wchar_t buffer[MAX_PATH];

		if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, 0, buffer)))
		{
			path = buffer;
		}

		return path;
	}
}