#include "readingExcel.h"

static void processTransitionMapSheet(const xlnt::worksheet& transitionMapSheet, TransitionMap& transitionMap)
{
	auto max_row = transitionMapSheet.highest_row();
	auto max_col = transitionMapSheet.highest_column().index;
	for (int col = 2; col <= max_col;++col)
	{
		for (int row=2;row <=max_row;++row)
		{
			std::string cellString;
			try
			{
				cellString = transitionMapSheet.cell(col, row).value<std::string>();
			}
			catch(std::out_of_range)
			{
				continue;
			}
			std::string statesName;
			try
			{
				statesName = transitionMapSheet.cell(1, row).value<StatesName>();
			}
			catch(std::exception)
			{
				std::cerr << "not find corresponding states" << std::endl;
				std::exit(EXIT_FAILURE);
			}
			InputName inputName;
			try
			{
				inputName = transitionMapSheet.cell(col, 1).value<StatesName>();
			}
			catch (std::out_of_range)
			{
				inputName = Epsilon;
			}

			std::stringstream ss(cellString);
			StatesName token;
			std::set<StatesName> statesSet;
			while (std::getline(ss, token, ','))
			{
				token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
				if (!token.empty())
				{
					statesSet.insert(token);
				}
			}

			transitionMap[std::make_pair(statesName, inputName)] = statesSet;
		}
	}
}

static void processStartAndEnd(const xlnt::worksheet& startAndEndSheet, StatesName& start, std::set<StatesName>& end)
{
	const int startRow = 1;
	const int endRow = 2;
	const int statesColumn = 2;
	StatesName startCellString = startAndEndSheet.cell(statesColumn,startRow ).value<StatesName>();
	StatesName endCellString = startAndEndSheet.cell(statesColumn,endRow ).value<StatesName>();
	std::stringstream ss(endCellString);
	StatesName token;
	std::set<StatesName> statesSet;
	while (std::getline(ss, token, ','))
	{
		token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
		if (!token.empty())
		{
			statesSet.insert(token);
		}
	}
	start = startCellString;
	end = statesSet;
}

std::wstring openExcelFile()
{
	// Define constants for file dialog settings
	const int MAX_FILE_PATH = 260;
	wchar_t szFile[MAX_FILE_PATH] = L"";

	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;               // Handle to the owner window
	ofn.lpstrFile = szFile;             // Pointer to the file name buffer
	ofn.nMaxFile = MAX_FILE_PATH;       // Size of the file buffer
	ofn.lpstrFilter = L"Excel Files\0*.XLS;*.XLSX\0All Files\0*.*\0";
	ofn.nFilterIndex = 1;               // Default filter index (Excel Files)
	ofn.lpstrFileTitle = NULL;          // File title (not used here)
	ofn.nMaxFileTitle = 0;              // File title size (not used here)
	ofn.lpstrInitialDir = NULL;         // Initial directory (NULL means start at last visited location)
	ofn.lpstrTitle = L"Select an Excel File";   // Title of the dialog
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;  // Flags
	if (GetOpenFileNameW(&ofn))
	{
		return std::wstring(szFile); // 返回选中的文件路径
	}
	else
	{
		DWORD dwError = CommDlgExtendedError();
		if (dwError == 0)
		{
			// User pressed cancel
			MessageBoxW(NULL, L"No file selected", L"Info", MB_OK);
		}
		else
		{
			// An error occurred
			std::wstringstream ss;
			ss << L"An error occurred in GetOpenFileName: " << dwError;
			MessageBoxW(NULL, ss.str().c_str(), L"Error", MB_OK);
		}
		return L""; // 返回空字符串表示未选择文件
	}
}

void processFile(const std::wstring& filePath, TransitionMap& transitionMap, StatesName& start, std::set<StatesName>& end)
{
	if (filePath.empty())
	{
		std::wcout << L"No file selected." << std::endl;
		return;
	}
	xlnt::workbook wb;
	try
	{
		wb.load(filePath);
	}
	catch (const std::exception& e)
	{
		std::wcerr << L"Error processing file: " << e.what() << std::endl;
		//
	}
	xlnt::worksheet transitionMapSheet;
	xlnt::worksheet startAndEndSheet;
	bool findT = false;//find TransitionMap
	bool findSE = false;//find start and End
	for (const auto& sheet : wb)
	{
		if (sheet.cell("A1").value<std::string>() == "T" && !findT)
		{
			transitionMapSheet = sheet;
			findT = true;
			continue;
		}

		if (sheet.cell("A1").value<std::string>() == "start" && sheet.cell("A2").value<std::string>() == "end" && !findSE)
		{
			startAndEndSheet = sheet;
			findSE = true;
		}
	}
	if (!findT || !findSE)
	{
		std::cerr << "not find correct format excel file,please check the file format" << std::endl;
		system("pause");//
	}
	processTransitionMapSheet(transitionMapSheet, transitionMap);
	processStartAndEnd(startAndEndSheet, start, end);
}

