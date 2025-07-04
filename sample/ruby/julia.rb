require_relative '../cpp/BitMapPlusPlus'
require_relative './colormaps'

MAX_ITERATIONS = 300
CR = -0.70000
CI = 0.27015

def julia
  image = Bmp::Bitmap.new(640, 480)

  image.height.times do |y|
    if y % 10  == 0
      STDOUT << "Row: " << y << "/" << image.height << "\n"
    end

    image.width.times do |x|
      nextr = 1.5 * (2.0 * x / image.width() - 1.0)
      nexti = (2.0 * y / image.height() - 1.0)

      MAX_ITERATIONS.times do |i|
        prevr = nextr
        previ = nexti

        nextr = prevr * prevr - previ * previ + CR
        nexti = 2 * prevr * previ + CI

        if ((nextr * nextr) + (nexti * nexti)) > 4
          index = ((1000.0 * i) / MAX_ITERATIONS).floor
          pixel = HSV_COLORMAP[index]
          image.set(x, y, pixel)
          break
        end
      end
    end
  end

  # Save bitmap
  path = File.join(__dir__, "..", "images", "julia.bmp")
  path = File.expand_path(path)
  std_path = Std::Filesystem::Path.new(path)
  image.save(std_path)
end

julia