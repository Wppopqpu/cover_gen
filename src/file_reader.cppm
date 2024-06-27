module;
#include <png.h>
#include <cstdio>
#include <cassert>
#include <string_view>
#include <string>
#include <memory>
#include <stdexcept>
#include <utility>
#include <csetjmp>
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
		decltype(auto) native_handle() const {
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
		class image: public ::image {
			public:
				image(file const & png_file) {
					m_png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
					if (!m_png_ptr) {
						throw read_failure("png_create_read_struct failed.");
					}

					m_info_ptr = png_create_info_struct(m_png_ptr);
					if (!m_info_ptr) {
						png_destroy_read_struct(&m_png_ptr, nullptr, nullptr);
						throw read_failure("png_create_info_struct failed");
					}

					if (setjmp(png_jmpbuf(m_png_ptr))) {
						png_destroy_read_struct(&m_png_ptr, &m_info_ptr, nullptr);
						throw read_failure("failed to set error handling");
					}

					png_init_io(m_png_ptr, png_file.native_handle());
					png_set_sig_bytes(m_png_ptr, 8);
					png_read_png(m_png_ptr, m_info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_ALPHA | PNG_TRANSFORM_GRAY_TO_RGB, nullptr);

					assert(("Invalid colortype.", PNG_COLOR_TYPE_RGB == png_get_color_type(m_png_ptr, m_info_ptr)));

					m_width = static_cast<image_size_t>(png_get_image_width(m_png_ptr, m_info_ptr));
					m_height = static_cast<image_size_t>(png_get_image_height(m_png_ptr, m_info_ptr));
					m_data = static_cast<unsigned char **>(png_get_rows(m_png_ptr, m_info_ptr));

				}
				virtual ~image() override {
					png_destroy_read_struct(&m_png_ptr, &m_info_ptr, nullptr);
				}

				[[nodiscard]]
				virtual image_size_t width() const override {
					return m_width;
				}
				[[nodiscard]]
				virtual image_size_t height() const override {
					return m_height;
				}

				[[nodiscard]]
				virtual fragment at(image_size_t const x, image_size_t const y) const override {
					return {
						.red = m_data[y][3*x],
						.green = m_data[y][3*x+1],
						.blue = m_data[y][3*x+2],
					};
				}

			private:
				png_structp m_png_ptr = nullptr;
				png_infop m_info_ptr = nullptr;
				unsigned char ** m_data;
				image_size_t m_width, m_height;
		};

		virtual std::unique_ptr<::image> read_file(std::string_view path) override {
			auto png_file = prepare_file(path);
			return std::make_unique<image>(png_file);
		}
	private:
		[[nodiscard]]
		file prepare_file(std::string_view path) {
			file png_file(path);

			png_byte header[8];
			auto result = std::fread(static_cast<void*>(header), 1, 8, png_file.native_handle());

			if (result != 8) {
				throw read_failure("Cannot read png header.");
			}
			if (png_sig_cmp(header, 0, 8)) {
				throw read_failure("File is not a valid PNG file.");
			}

			return png_file;
		}
};

