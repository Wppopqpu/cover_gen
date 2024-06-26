module;
#include <iostream>
#include <string>
export module main;
import cover_gen;
import file_reader;
import pixelize;

int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "Invalid arg number" << std::endl;
		return -3;
	}

	std::string fpath = argv[1];

	try {
		png_reader reader;

		auto image_p = reader.read_file(fpath);
		auto final_image = pixelized_image(*image_p, 10, 10);
		to_alpha(std::cout, final_image);
	} catch (read_failure & e) {
		std::cerr << "Cannot read:\n"
			<< e.what()
			<< std::endl
			;
		return -2;
	}

    return 0;
}
