module;
#include <cstdint>
export module image;

export using image_size_t = std::uint16_t;
// to present a color pass (8 bits)
export using pass_value_t = unsigned char;

static_assert(sizeof(pass_value_t)==1);

/**
 * @brief RGBA form of a fragment
 */
export struct fragment {
	pass_value_t red, green, blue;
};

/**
 * @brief RGBA image
 */
export class image {
	public:
		virtual ~image() = default;

		virtual image_size_t width() const = 0;
		virtual image_size_t height() const = 0;
		
		/**
		 * @brief get color of a fragment
		 *
		 * @param x x pos
		 * @param y y pos
		 * @return color
		 */
		virtual fragment at(image_size_t const x, image_size_t const y) const = 0;
};
