module;
#include <ostream>
#include <string>
#include <array>
#include <cassert>
export module cover_gen;
import image;
import file_reader;

[[nodiscard]]
std::string get_group_name(image_size_t const x, image_size_t const y) {
	static constexpr std::string prefix("CoverGen");
	return prefix + std::to_string(x) + "_" + std::to_string(y);
}

[[nodiscard]]
char present_half_pass(pass_value_t const half_v) {
	static_assert(sizeof(half_v) == 1);
	assert(("Invalid half pass value.", half_v < 16));
	if (half_v < 10) {
		return '0' + half_v;
	}
	return 'A' - 10 + half_v;
}

void present_pass(char * const to, pass_value_t const pass_v) {
	*to = present_half_pass(pass_v / 16);
	to[1] = present_half_pass(pass_v % 16);
}

[[nodiscard]]
std::array<char, 6> present_color(fragment const frag) {
	std::array<char, 6> result;
	present_pass(result.data(), frag.red);
	present_pass(result.data() + 2, frag.green);
	present_pass(result.data() + 4, frag.blue);
	return result;
}

export void to_alpha(std::ostream & out, image const & im) {
	static constexpr std::string ns_name("cover_gen");

	out << "-- This file was generated by cover_gen.\n"
		"-- It will be used as a image for alpha theme.\n\n"

		"local M = {}\n"
		"M.namespace = vim.api.nvim_create_namespace(\"" << ns_name << "\")\n\n"
		;
	
	for (image_size_t x = 0; x < im.width(); ++x) {
		for (image_size_t y = 0; y < im.height(); ++y) {
			out << "vim.api.nvim_set_hl(namespace, \""
				<< get_group_name(x, y)
				<< "\", {\n"
					"\tbg = \"#"
				;
			out.write(present_color(im.at(x, y)).data(), 6);
			out <<"\",\n"
					"})\n"
				;
		}
	}

	out << "\n"
		"M.text = {\n"
		;
	for (image_size_t row_count = 0; row_count < im.height(); ++row_count) {
		out << "\t\"";
		for (image_size_t col_count = 0; col_count < im.width(); ++col_count) {
			out << ' ';
		}
		out << "\",\n";
	}
	out << "}\n";

	out << "\n"
		"M.hl = {\n"
		;
	for (image_size_t row_count = 0; row_count < im.height(); ++row_count) {
		out << "\t{\n";
		for (image_size_t col_count = 0; col_count < im.width(); ++col_count) {
			out << "\t\t\""
				<< get_group_name(col_count, row_count)
				<< "\",\n"
				;
		}
		out << "\t},\n";
	}
	out << "}\n";

	out << "\n"
		"return M";
}
