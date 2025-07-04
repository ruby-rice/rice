require_relative '../cpp/BitmapPlusPlus'

def random_pixel
  color = Bmp::Pixel.new
  color.r = rand(255)
  color.g = rand(255)
  color.b = rand(255)
  color
end

def random_colors
  image = Bmp::Bitmap.new(512, 512)

  # Assign a random color to each pixel in the image
  image.each do |pixel|
    pixel.assign(random_pixel)
  end

  # Save bitmap
  path = File.join(__dir__, "..", "images", "random_colors.bmp")
  path = File.expand_path(path)
  std_path = Std::Filesystem::Path.new(path)
  image.save(std_path)
rescue Bmp::Exception => e
  puts "[BMP ERROR]: #{e.message}"
  exit(1)
end

random_colors