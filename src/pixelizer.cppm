module;
#include <memory>
#include <concepts>
#include <cassert>
#include <iostream>
export module pixelize;
import image;

[[nodiscard]]
fragment operator+(fragment const & a, fragment const & b) {
	return {
		.red = a.red + b.red,
		.green = a.green + b.green,
		.blue = a.blue + b.blue,
	};
}

template <std::integral int_t>
[[nodiscard]]
fragment operator/(fragment const & a, int_t devider) {
	return {
		.red = a.red / devider,
		.green = a.green / devider,
		.blue = a.blue / devider,
	};
}


export class pixelized_image: public image{
	private:
		image const & m_raw;
		image_size_t const m_width, m_height;

	public:
		pixelized_image(image const & raw, image_size_t width, image_size_t height)
			: m_raw(raw)
			, m_width(width)
			, m_height(height)
			{
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
			fragment sum { 0, 0, 0 };
			image_size_t count = 0;

			for (image_size_t row = x * m_raw.width() / m_width; row < (x+1) * m_raw.width() / m_width; ++row) {
				for (image_size_t col = y * m_raw.height() / m_height; col < (y+1) * m_raw.height() / m_height; ++col) {
					if (row >= 0 && row < m_raw.width() && col >= 0 && col < m_raw.height()) {
						++count;
						sum = sum + m_raw.at(row, col);
					}
				}
			}

			sum = sum / count;
			// To make sure.
			sum.red = sum.red > 255? 255: sum.red;
			sum.green = sum.green > 255? 255: sum.green;
			sum.blue = sum.blue > 255? 255: sum.blue;
			return sum;
		}
};
