module;
#include <png.h>
#include <cstdio>
#include <cassert>
#include <string_view>
#include <memory>
#include <stdexcept>
#include <utility>
export module file_reader;
import image;

export struct read_failure: std::runtime_error {
	read_failure(std::string_view desc)
		: std::runtime_error { std::string { desc } }
	{}
};

export class file final {
	public:
		file(std::string_view path) {
			m_file = std::fopen(path.data(), "rb");
			if (!m_file) {
				throw read_failure("failed to open file");
			}
		}
		file(file && afile) {
			m_file = std::exchange(afile.m_file, nullptr);
		}

		~file() {
			std::fclose(m_file);
		}

		[[nodiscard]]
		decltype(auto) native_handle() {
			return m_file;
		}
	private:
		std::FILE * m_file = nullptr;
};

export class image_reader {
	public:
		virtual ~image_reader() = default;

		virtual std::unique_ptr<image> read_file(std::string_view path) = 0;
};

export class png_reader: public image_reader {
	public:
		virtual std::unique_ptr<image> read_file(std::string_view path) override {
			auto png_file = prepare_file(path);

			auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
			if (!png_ptr) {
				throw read_failure("png_create_read_struct failed.");
			}

			auto info_ptr = png_create_info_struct(png_ptr);
			if (!info_ptr) {
				png_destroy_read_struct(&png_ptr, nullptr, nullptr);
				throw read_failure("png_create_info_struct failed");
			}

			if (setjmp(png_jmpbuf(png_ptr))) {
				png_destroy_read_struct(&png_ptr, info_ptr, nullptr);
			}

			png_init_io(png_ptr, png_file.native_handle());
			png_set_sig_bytes(png_ptr, 8);
			png_read_info(png_ptr, info_ptr);
		}
	private:
		[[nodiscard]]
		file prepare_file(std::string_view path) {
			file png_file(path);

			png_byte header[8];
			auto result = std::fread(static_cast<void*>(header), 1, 8, png_file.native_handle())

			if (result != 8) {
				throw read_failure("Cannot read png header.");
			}
			if (png_sig_cmp(header, 8)) {
				throw read_failure("File is not a valid PNG file.");
			}
		}
};

