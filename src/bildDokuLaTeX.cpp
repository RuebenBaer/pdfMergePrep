#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <wchar.h>
#include <boost/filesystem.hpp>
#include <vector>
#include <ctime>
#include <new>

#define ae (char)0xE4
#define oe (char)0xf6
#define ue (char)0xfc

namespace fs = boost::filesystem;

typedef struct {
	std::string datei;
	std::string titel;
} bild;

void DateiVerarbeiten(fs::path pfad, std::string dirRoot, std::ofstream& os, int ebene, std::vector<bild>& bildVec);
void VerzeichnisVerarbeiten(fs::path pfad, std::string dirRoot, std::ofstream& os, int ebene, std::string& ordnerUeberschrift);
void BilderSchreiben (std::ofstream& os, std::vector<bild>& bildVec);
void GrossBuchstaben(std::string& str);
std::string EbenenName(int ebene);
void printLicense(void);

int main(int argc, char** argv)
{
	system("chcp 1252");
	system("cls");
	
	std::cout<<"\n.: (c)R"<<ue<<"bensoft 2026 - bildDokuLaTeX :.\n\n";

	if(argc <2)
	{
		std::cout<<"\nVerwendung: bildDokuLaTeX.exe Datei_1 Datei_2 ...\n\n";
		std::cout<<"Entfernt Punkte aus dem Dateinamen (Datei_1 Datei_2 ...)\n";
		std::cout<<"und schreibt den '\\includegraphics'-Block (LaTeX) f"<<ue<<"r jede Datei.\n\n";
		system("PAUSE");
		printLicense();
		system("PAUSE");
		return 1;
	}

	std::ofstream os("bildDoku.tex", std::ios::out);
	if(!os.good())
	{
		std::cout<<"Ausgabedatei 'bildDoku.tex' konnte nicht ge"<<oe<<"ffnet werden\n";
		system("PAUSE");
		return 1;
	}
	
	// Zuerst die Dateien verarbeiten
	std::vector<bild> bildVec;
	for(int datNr = 1; datNr < argc; datNr++)
	{
		std::cout<<"Argument "<<datNr<<": "<<argv[datNr]<<"\n";
		fs::path pfad(argv[datNr]);
		if(!fs::exists(pfad))continue;

		std::string ordnerUeberschrift("");
		std::string dirRoot = "";
		fs::path subPfad = pfad.parent_path();
		if(fs::is_regular_file(pfad))
		{
			DateiVerarbeiten(pfad, dirRoot, os, 0, bildVec);
		}
	}
	BilderSchreiben (os, bildVec);
	
	// Dann die Verzeichniss verarbeiten
	for(int datNr = 1; datNr < argc; datNr++)
	{
		std::cout<<"Argument "<<datNr<<": "<<argv[datNr]<<"\n";
		fs::path pfad(argv[datNr]);
		if(!fs::exists(pfad))continue;

		std::string ordnerUeberschrift("");
		std::string dirRoot = "";
		fs::path subPfad = pfad.parent_path();
		if(fs::is_directory(pfad))
      	{
			VerzeichnisVerarbeiten(pfad, dirRoot, os, 0, ordnerUeberschrift);
		}
	}

	os.close();
	std::cout<<"\n\n******************ACHTUNG!******************\n\n";
	std::cout<<"BildDoku.tex zuerst in einem Texteditor "<<oe<<"ffnen und im 'UTF-8'-Format speichern.\n";
	std::cout<<"LatexEditor in 'UTF-8'-Modus starten!\n\n";
	system("PAUSE");
	return 0;
}

void VerzeichnisVerarbeiten(fs::path pfad, std::string dirRoot, std::ofstream& os, int ebene, std::string& ordnerUeberschrift)
{
	dirRoot += pfad.filename().generic_string() + '/';
	std::cout<<"Verzeichnis "<<dirRoot<<" gefunden\n";
	fs::directory_iterator it(pfad);
	
	std::string ueberschrift = "\\" + EbenenName(ebene) + "{" + pfad.filename().generic_string() + "}\n";
	os << ueberschrift;

	std::vector<bild> bildVec;
	while(it != fs::directory_iterator())
	{
		fs::path subPfad = it->path();
		if(fs::is_regular_file(subPfad))
		{
			DateiVerarbeiten(subPfad, dirRoot, os, ebene + 1, bildVec);
			ordnerUeberschrift = "";
		}
		it++;
	}
	BilderSchreiben (os, bildVec);

	it = fs::directory_iterator(pfad);
	while(it != fs::directory_iterator())
	{
		fs::path subPfad = it->path();
		if(fs::is_directory(subPfad))
		{
			VerzeichnisVerarbeiten(subPfad, dirRoot, os, ebene + 1, ordnerUeberschrift);
		}
		it++;
	}
	return;
}

void DateiVerarbeiten(fs::path pfad, std::string dirRoot, std::ofstream& os, int ebene, std::vector<bild>& bildVec)
{
	std::string strPfad = pfad.filename().generic_string();
	std::string ext = pfad.extension().generic_string();
	GrossBuchstaben(ext);
	if(strcmp(ext.c_str(), ".PNG") && strcmp(ext.c_str(), ".JPG") && strcmp(ext.c_str(), ".JPEG"))
	{
		std::cout<<"\t"<<strPfad<<" ist kein Bild ("<<ext<<") ... Datei wird "<<ue<<"bersprungen\n";
		return;
	}

	std::string strDir = pfad.parent_path().generic_string();
	std::cout<<"\tVerarbeite "<<strPfad<<"\n";
	size_t letzteFundStelle = strPfad.find_last_of('.', std::string::npos);
	if(letzteFundStelle == std::string::npos)return;
	
	// Punkte und Kommas im Dateinamen tauschen, sonst findet LaTeX die Dateien nicht
	size_t fundStelle = 0;
	int gefunden = 0;
	do
	{
		fundStelle = strPfad.find('.', fundStelle+1);
		if(fundStelle != letzteFundStelle)
		{
			strPfad[fundStelle] = '+';
			gefunden++;
		}
	}while(fundStelle != letzteFundStelle);

	fundStelle = 0;
	do
	{
		fundStelle = strPfad.find(',', fundStelle+1);
		if(fundStelle == std::string::npos) break;
		strPfad[fundStelle] = ' ';
		gefunden++;
	}while(1);

	if(gefunden)
	{
		fs::rename(pfad, strDir + "/" + strPfad);
	}

	std::string dateiName = strPfad;//dateiname ist Eintrag in TOC => Unterstriche entfernen und Pluszeichen in Punkt zurücktauschen
	fundStelle = 0;
	do
	{
		fundStelle = dateiName.find('_', fundStelle+1);
		if(fundStelle == std::string::npos) break;
		dateiName[fundStelle] = ' ';
	}while(1);
	fundStelle = 0;
	do
	{
		fundStelle = dateiName.find('+', fundStelle+1);
		if(fundStelle == std::string::npos) break;
		dateiName[fundStelle] = '.';
	}while(1);

	fundStelle = strPfad.find('.', 0);
	dateiName.erase(fundStelle, std::string::npos);
	bild tBild;
	tBild.datei = dirRoot + strPfad;
	if (dateiName[0] == '+') {
		tBild.titel = strPfad.substr(1, strPfad.find_last_of('.') - 1);
	} else {
		tBild.titel = "";
	}
	bildVec.push_back(tBild);

	return;
}

void BilderSchreiben (std::ofstream& os, std::vector<bild>& bildVec)
{
	int bildCount = 0;
	for (std::vector<bild>::iterator it = bildVec.begin();
			it != bildVec.end(); it++) {
		if (bildCount == 0) {
			os << "\\begin{figure}[ht]\n"
				"\t\\centering\n";
		} else {
			os << "\t\\qquad\n";
		}
		os << "\t\\parbox{0.4\\textwidth}"
			"{\\includegraphics[width=\\linewidth]{" << (*it).datei << "}";
		if (!(*it).titel.empty()) {
			os << "\\\\" << (*it).titel;
		}
		os << "}\n";
		if (bildCount == 1) {
			os << "\\end{figure}\n\n";
		}
		bildCount ^= 1;
	}
	// Bei ungerader Anzahl Bilder, eine leere parbox anhängen
	if (bildCount == 1) {
		os << "\t\\qquad\n";
		os << "\t\\parbox{0.4\\textwidth}{\\qquad}\n";
		os << "\\end{figure}\n\n";
	}
	return;
}

void GrossBuchstaben(std::string& str)
{
	for(size_t i = 0; i < str.length(); i++)
	{
		if((str[i] >= 'a') && (str[i] <= 'z'))
			str[i] = str[i] + ('A' - 'a');
	}
	return;
}

std::string EbenenName(int ebene)
{
	switch(ebene)
	{
		case 0:
			return std::string("chapter");
		case 1:
			return std::string("section");
		case 2:
			return std::string("subsection");
		case 3:
			return std::string("subsubsection");
		case 4:
			return std::string("paragraph");
		case 5:
			return std::string("subparagraph");
		default:
			return std::string("part");
	}
	return std::string("");
}

void printLicense(void)
{
	std::cout<<"\n*\t*\t*\t*\t*\n";
	std::cout<<"GNU GENERAL PUBLIC LICENSE disclaimer:\n\n";
	std::cout<<"This program is free software: you can redistribute it and/or modify\n";
	std::cout<<"it under the terms of the GNU General Public License as published by\n";
	std::cout<<"the Free Software Foundation, either version 3 of the License, or\n";
	std::cout<<"(at your option) any later version.\n\n";
	std::cout<<"This program is distributed in the hope that it will be useful,\n";
	std::cout<<"but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
	std::cout<<"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n";
	std::cout<<"GNU General Public License for more details.\n\n";
	std::cout<<"You should have received a copy of the GNU General Public License\n";
	std::cout<<"along with this program.  If not, see <http://www.gnu.org/licenses/>\n\n";
	std::cout<<"\n*\t*\t*\t*\t*\n";

	std::cout<<"Boost Software License - Version 1.0 - August 17th, 2003\n";
	std::cout<<"\n";
	std::cout<<"Permission is hereby granted, free of charge, to any person or organization\n";
	std::cout<<"obtaining a copy of the software and accompanying documentation covered by\n";
	std::cout<<"this license (the \"Software\") to use, reproduce, display, distribute,\n";
	std::cout<<"execute, and transmit the Software, and to prepare derivative works of the\n";
	std::cout<<"Software, and to permit third-parties to whom the Software is furnished to\n";
	std::cout<<"do so, all subject to the following:\n";
	std::cout<<"\n";
	std::cout<<"The copyright notices in the Software and this entire statement, including\n";
	std::cout<<"the above license grant, this restriction and the following disclaimer,\n";
	std::cout<<"must be included in all copies of the Software, in whole or in part, and\n";
	std::cout<<"all derivative works of the Software, unless such copies or derivative\n";
	std::cout<<"works are solely in the form of machine-executable object code generated by\n";
	std::cout<<"a source language processor.\n";
	std::cout<<"\n";
	std::cout<<"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n";
	std::cout<<"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n";
	std::cout<<"FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT\n";
	std::cout<<"SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE\n";
	std::cout<<"FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,\n";
	std::cout<<"ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER\n";
	std::cout<<"DEALINGS IN THE SOFTWARE.\n";
	std::cout<<"\n*\t*\t*\t*\t*\n";
	
	std::cout<<"Please find the licenses in the \"lic\"-folder\n\n";
	return;
}
