module;
#include <cstdint>
export module image;

using image_size_t = std::uint16_t;
// to present a color pass
using pass_value_t = std::uint16_t;

/**
 * @brief RGBA form of a fragment
 */
export struct fragment {
	pass_value_t red, green, blue, alpha;
};

/**
 * @brief RGBA image
 */
export class image {
	public:
		virtual ~image() = default;

		virtual image_size_t width() = 0;
		virtual image_size_t height() = 0;
		
		/**
		 * @brief get color of a fragment
		 *
		 * @param x x pos
		 * @param y y pos
		 * @return color
		 */
		virtual fragment at(image_size_t const x, image_size_t const y) = 0;
};
