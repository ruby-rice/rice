#include <BitmapPlusPlus.hpp>
#include "BitmapPlusPlus-rb.hpp"

using namespace Rice;

Rice::Class rb_cBmpBitmap;
Rice::Class rb_cBmpBitmapHeader;
Rice::Class rb_cBmpException;
Rice::Class rb_cBmpPixel;

void initialize()
{
  Module rb_mBmp = define_module("Bmp");
  
  rb_mBmp.define_constant("BITMAP_BUFFER_MAGIC", bmp::BITMAP_BUFFER_MAGIC);
  
  rb_cBmpBitmapHeader = define_class_under<bmp::BitmapHeader>(rb_mBmp, "BitmapHeader").
    define_constructor(Constructor<bmp::BitmapHeader>()).
    define_attr("magic", &bmp::BitmapHeader::magic).
    define_attr("file_size", &bmp::BitmapHeader::file_size).
    define_attr("reserved1", &bmp::BitmapHeader::reserved1).
    define_attr("reserved2", &bmp::BitmapHeader::reserved2).
    define_attr("offset_bits", &bmp::BitmapHeader::offset_bits).
    define_attr("size", &bmp::BitmapHeader::size).
    define_attr("width", &bmp::BitmapHeader::width).
    define_attr("height", &bmp::BitmapHeader::height).
    define_attr("planes", &bmp::BitmapHeader::planes).
    define_attr("bits_per_pixel", &bmp::BitmapHeader::bits_per_pixel).
    define_attr("compression", &bmp::BitmapHeader::compression).
    define_attr("size_image", &bmp::BitmapHeader::size_image).
    define_attr("x_pixels_per_meter", &bmp::BitmapHeader::x_pixels_per_meter).
    define_attr("y_pixels_per_meter", &bmp::BitmapHeader::y_pixels_per_meter).
    define_attr("clr_used", &bmp::BitmapHeader::clr_used).
    define_attr("clr_important", &bmp::BitmapHeader::clr_important);
  
  rb_cBmpPixel = define_class_under<bmp::Pixel>(rb_mBmp, "Pixel").
    define_attr("r", &bmp::Pixel::r).
    define_attr("g", &bmp::Pixel::g).
    define_attr("b", &bmp::Pixel::b).
    define_constructor(Constructor<bmp::Pixel>()).
    define_constructor(Constructor<bmp::Pixel, const std::int32_t>(),
      Arg("rgb")).
    define_constructor(Constructor<bmp::Pixel, const std::uint8_t, const std::uint8_t, const std::uint8_t>(),
      Arg("red"), Arg("green"), Arg("blue")).
    define_method("==", &bmp::Pixel::operator==,
      Arg("other")).
    define_method("!=", &bmp::Pixel::operator!=,
      Arg("other")).
    define_method("assign", [](bmp::Pixel& self, bmp::Pixel& other)
    {
      self.r = other.r;
      self.g = other.g;
      self.b = other.b;
    });

  
  rb_mBmp.define_constant("Aqua", bmp::Aqua);
  
  rb_mBmp.define_constant("Beige", bmp::Beige);
  
  rb_mBmp.define_constant("Black", bmp::Black);
  
  rb_mBmp.define_constant("Blue", bmp::Blue);
  
  rb_mBmp.define_constant("Brown", bmp::Brown);
  
  rb_mBmp.define_constant("Chocolate", bmp::Chocolate);
  
  rb_mBmp.define_constant("Coral", bmp::Coral);
  
  rb_mBmp.define_constant("Crimson", bmp::Crimson);
  
  rb_mBmp.define_constant("Cyan", bmp::Cyan);
  
  rb_mBmp.define_constant("Firebrick", bmp::Firebrick);
  
  rb_mBmp.define_constant("Gold", bmp::Gold);
  
  rb_mBmp.define_constant("Gray", bmp::Gray);
  
  rb_mBmp.define_constant("Green", bmp::Green);
  
  rb_mBmp.define_constant("Indigo", bmp::Indigo);
  
  rb_mBmp.define_constant("Lavender", bmp::Lavender);
  
  rb_mBmp.define_constant("Lime", bmp::Lime);
  
  rb_mBmp.define_constant("Magenta", bmp::Magenta);
  
  rb_mBmp.define_constant("Maroon", bmp::Maroon);
  
  rb_mBmp.define_constant("Navy", bmp::Navy);
  
  rb_mBmp.define_constant("Olive", bmp::Olive);
  
  rb_mBmp.define_constant("Orange", bmp::Orange);
  
  rb_mBmp.define_constant("Pink", bmp::Pink);
  
  rb_mBmp.define_constant("Purple", bmp::Purple);
  
  rb_mBmp.define_constant("Red", bmp::Red);
  
  rb_mBmp.define_constant("Salmon", bmp::Salmon);
  
  rb_mBmp.define_constant("Silver", bmp::Silver);
  
  rb_mBmp.define_constant("Snow", bmp::Snow);
  
  rb_mBmp.define_constant("Teal", bmp::Teal);
  
  rb_mBmp.define_constant("Tomato", bmp::Tomato);
  
  rb_mBmp.define_constant("Turquoise", bmp::Turquoise);
  
  rb_mBmp.define_constant("Violet", bmp::Violet);
  
  rb_mBmp.define_constant("White", bmp::White);
  
  rb_mBmp.define_constant("Wheat", bmp::Wheat);
  
  rb_mBmp.define_constant("Yellow", bmp::Yellow);
  
  rb_cBmpException = define_class_under<bmp::Exception, std::runtime_error>(rb_mBmp, "Exception").
    define_constructor(Constructor<bmp::Exception, const std::string&>(),
      Arg("message"));
  
  rb_cBmpBitmap = define_class_under<bmp::Bitmap>(rb_mBmp, "Bitmap").
    define_constructor(Constructor<bmp::Bitmap>()).
    define_constructor(Constructor<bmp::Bitmap, const std::string&>(),
      Arg("filename")).
    define_constructor(Constructor<bmp::Bitmap, const std::int32_t, const std::int32_t>(),
      Arg("width"), Arg("height")).
    define_constructor(Constructor<bmp::Bitmap, const bmp::Bitmap&>(),
      Arg("other")).
    define_method("draw_line", &bmp::Bitmap::draw_line,
      Arg("x1"), Arg("y1"), Arg("x2"), Arg("y2"), Arg("color")).
    define_method("fill_rect", &bmp::Bitmap::fill_rect,
      Arg("x"), Arg("y"), Arg("width"), Arg("height"), Arg("color")).
    define_method("draw_rect", &bmp::Bitmap::draw_rect,
      Arg("x"), Arg("y"), Arg("width"), Arg("height"), Arg("color")).
    define_method("draw_triangle", &bmp::Bitmap::draw_triangle,
      Arg("x1"), Arg("y1"), Arg("x2"), Arg("y2"), Arg("x3"), Arg("y3"), Arg("color")).
    define_method("fill_triangle", &bmp::Bitmap::fill_triangle,
      Arg("x1"), Arg("y1"), Arg("x2"), Arg("y2"), Arg("x3"), Arg("y3"), Arg("color")).
    define_method("draw_circle", &bmp::Bitmap::draw_circle,
      Arg("center_x"), Arg("center_y"), Arg("radius"), Arg("color")).
    define_method("fill_circle", &bmp::Bitmap::fill_circle,
      Arg("center_x"), Arg("center_y"), Arg("radius"), Arg("color")).
    define_method<bmp::Pixel&(bmp::Bitmap::*)(const std::int32_t, const std::int32_t)>("get", &bmp::Bitmap::get,
      Arg("x"), Arg("y")).
    define_method<const bmp::Pixel&(bmp::Bitmap::*)(const std::int32_t, const std::int32_t) const>("get", &bmp::Bitmap::get,
      Arg("x"), Arg("y")).
    define_method("width", &bmp::Bitmap::width).
    define_method("height", &bmp::Bitmap::height).
    define_method("clear", &bmp::Bitmap::clear,
      Arg("pixel") = static_cast<const bmp::Pixel>(bmp::Black)).
    define_method<const bmp::Pixel&(bmp::Bitmap::*)(const std::size_t) const>("[]", &bmp::Bitmap::operator[],
      Arg("i")).
    define_method<bmp::Pixel&(bmp::Bitmap::*)(const std::size_t)>("[]", &bmp::Bitmap::operator[],
      Arg("i")).
    define_method("[]=", [](bmp::Bitmap&self, int index, bmp::Pixel & value)
    {
        self[index] = value;
    }).
    define_method("!", &bmp::Bitmap::operator!).
   /* define_method("to_bool", [](const bmp::Bitmap& self) -> bool
    {
      return self;
    }).*/
    define_method("==", &bmp::Bitmap::operator==,
      Arg("image")).
    define_method("!=", &bmp::Bitmap::operator!=,
      Arg("image")).
    define_method<bmp::Bitmap&(bmp::Bitmap::*)(const bmp::Bitmap&)>("assign", &bmp::Bitmap::operator=,
      Arg("image")).
    define_method<bmp::Bitmap&(bmp::Bitmap::*)(bmp::Bitmap&&) noexcept>("assign", &bmp::Bitmap::operator=,
      Arg("image")).
    define_iterator<std::vector<bmp::Pixel>::iterator(bmp::Bitmap::*)() noexcept>(&bmp::Bitmap::begin, &bmp::Bitmap::end, "each").
    define_method("cbegin", &bmp::Bitmap::cbegin).
    define_method("cend", &bmp::Bitmap::cend).
    define_iterator<std::vector<bmp::Pixel>::reverse_iterator(bmp::Bitmap::*)() noexcept>(&bmp::Bitmap::rbegin, &bmp::Bitmap::rend, "each_reverse").
    define_method("crbegin", &bmp::Bitmap::crbegin).
    define_method("crend", &bmp::Bitmap::crend).
    define_method("set", &bmp::Bitmap::set,
      Arg("x"), Arg("y"), Arg("color")).
    define_method("flip_v", &bmp::Bitmap::flip_v).
    define_method("flip_h", &bmp::Bitmap::flip_h).
    define_method("rotate_90_left", &bmp::Bitmap::rotate_90_left).
    define_method("rotate_90_right", &bmp::Bitmap::rotate_90_right).
    define_method("save", &bmp::Bitmap::save,
      Arg("filename")).
    define_method("load", &bmp::Bitmap::load,
      Arg("filename"));
}

extern "C"
void Init_BitmapPlusPlus()
{
  return Rice::detail::cpp_protect([]
  {
      initialize();
      Init_Ruby_Api();
  });
}
