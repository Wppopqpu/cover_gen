module;
#include <iostream>
#include <string>
export module main;
import image;
import cover_gen;
import file_reader;
import pixelize;

int main(int argc, char** argv)
{
	if (argc != 4) {
		std::cerr << "Invalid arguments.\n"
			"Usage:\n"
			"cover_gen {png_file} {output_width} {output_height}" << std::endl;
		return -3;
	}

	std::string fpath = argv[1];

	image_size_t width = (image_size_t)std::stoi(argv[2]), height = (image_size_t)std::stoi(argv[3]);

	try {
		png_reader reader;

		auto image_p = reader.read_file(fpath);
		auto final_image = pixelized_image(*image_p, width, height);
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
