require_relative '../cpp/BitmapPlusPlus'

def draw_primitives
  pixel = Bmp::Pixel.new(0x25292e)
  image = Bmp::Bitmap.new(512, 240)
  image.clear(pixel)

  # Draw a yellow line from position (250, 50) to position (500, 50)
  image.draw_line(250, 50, 500, 50, Bmp::Yellow)

  # Draw a red rectangle in position (10, 10) with size 100x100
  image.draw_rect(10, 10, 100, 100, Bmp::Red)
  # Draw a white filled rectangle in position (120, 10) with size 100x100
  image.fill_rect(120, 10, 100, 100, Bmp::White)

  # Triangle
  image.draw_triangle(60, 120, 10, 220, 120, 220, Bmp::Cyan)
  image.fill_triangle(180, 120, 130, 220, 245, 220, Bmp::Magenta)

  # Draw a non-filled Gray circle in position (300, 170) with 50 pixels radius
  image.draw_circle(300, 170, 50, Bmp::Gray)
  # Draw a filled Lime circle in position (300, 170) with 50 pixels radius
  image.fill_circle(420, 170, 50, Bmp::Lime)

  # Save bitmap
  path = File.join(__dir__, "..", "images", "primitives.bmp")
  path = File.expand_path(path)
  std_path = Std::Filesystem::Path.new(path)
  image.save(std_path)
end

draw_primitives