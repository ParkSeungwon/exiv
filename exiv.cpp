#include <exiv2/exiv2.hpp>
#include <iostream>
#include <cassert>
#include<string>
#include<sstream>
using namespace std;

void sstm(string command) 
{
	system(command.c_str());
}

int main(int argc, char* const argv[])
try {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " [directory to move] [files...]\n";
        return 1;
    }
	for(int n=2; n<argc; n++) {
		auto image = Exiv2::ImageFactory::open(argv[n]);
		assert(image.get() != 0);
		image->readMetadata();
		auto& exifData = image->exifData();
		if (exifData.empty()) {
			std::string error(argv[n]);
			error += ": No Exif data found in the file";
			throw Exiv2::Error(1, error);
		}

		double lat = 1, lon = 1;
		char dr;
		int degree, minute, second, divisor[3];
		for(auto& a : exifData) {
			if(a.key() == "Exif.GPSInfo.GPSLatitudeRef") {
				stringstream ss;
				ss << a.value();
				ss >> dr;
				lat *= dr == 'N' ? 1 : -1;
			} else if(a.key() == "Exif.GPSInfo.GPSLatitude") {
				stringstream ss;
				ss << a.value();
				ss >> degree >> dr >> divisor[0] >> minute >> dr >> divisor[1];
				ss >> second >> dr >> divisor[2];
				lat *= (float)degree/divisor[0] + (float)minute/60/divisor[1] +
					(float)second/3600/divisor[2];
			} else if(a.key() == "Exif.GPSInfo.GPSLongitudeRef") {
				stringstream ss;
				ss << a.value();
				ss >> dr;
				lon *= dr == 'E' ? 1 : -1;
			} else if(a.key() == "Exif.GPSInfo.GPSLongitude") {
				stringstream ss;
				ss << a.value();
				cout << a.value();
				ss >> degree >> dr >> divisor[0] >> minute >> dr >> divisor[1];
				ss >> second >> dr >> divisor[2];
				lon *= (float)degree/divisor[0] + (float)minute/60/divisor[1] + 
					(float)second/3600/divisor[2];
			}
		}
		stringstream ss;
		ss << " \"" << lat << ' ' << lon << "\" ";
		string gps = ss.str();
		for (auto& a : exifData) {
			if(a.key() == "Exif.Image.DateTime") {
				stringstream ss;
				ss << a.value();
				string s = ss.str();
				int sp = s.find(' ');
				string date = s.substr(0, sp);
				for(char& c : date) if(c == ':') c = '-';
				string time = s.substr(sp + 1);
				for(char& c : time) if(c == ':') c = '-';

				string direc{argv[1]};
				direc += '/';
				direc += date + '/';
				string command = "mkdir ";
				command += direc;
				cout << command << endl;
				string filename = argv[n];
				if(!(lat == 1 && lon == 1)) sstm("exiv2 mo -c" + gps + argv[n]);
				int dot = filename.find_last_of('.');
				string ext;
				if(dot != string::npos) ext = filename.substr(dot);
				
				system(command.c_str());
				command = "mv ";
				command += argv[n];
				command += ' ';
				command += direc + time + ext;
				system(command.c_str());
				cout << command << endl;

				sstm("mkdir " + direc + "/thumbnails");
				sstm("exiv2 -et " + direc + time + ext);
				sstm("mv " + direc + time + "-thumb.jpg " + direc + "/thumbnails/");
			}
		}
	}
    return 0;
} catch (Exiv2::Error& e) {
    std::cout << "Caught Exiv2 exception '" << e.what() << "'\n";
    return -1;
}
