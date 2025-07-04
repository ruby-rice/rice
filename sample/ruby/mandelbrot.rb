require_relative '../cpp/BitmapPlusPlus'
require_relative './colormaps'

MAX_ITERATIONS = 500

def mandelbrot
  image = Bmp::Bitmap.new(640, 480)

  image.height.times do |y|
    if y % 10  == 0
      STDOUT << "Row: " << y << "/" << image.height << "\n"
    end

    image.width.times do |x|
      cr = 1.5 * (2.0 * x / image.width - 1.0) - 0.5
      ci = (2.0 * y / image.height - 1.0)

      nextr = nexti = 0
      prevr = previ = 0

      MAX_ITERATIONS.times do |i|
        prevr = nextr
        previ = nexti

        nextr = prevr * prevr - previ * previ + cr;
        nexti = 2 * prevr * previ + ci;

        if ((nextr * nextr) + (nexti * nexti)) > 4
          z = Math.sqrt(nextr * nextr + nexti * nexti)

          # https://en.wikipedia.org/wiki/Mandelbrot_set#Continuous_.28smooth.29_coloring
          index = 1000.0 * Math.log2(1.75 + i - Math.log2(Math.log2(z))) / Math.log2(MAX_ITERATIONS)

          pixel = JET_COLORMAP[index]
          image.set(x, y, pixel)
          break
        end
      end
    end
  end

  # Save bitmap
  path = File.join(__dir__, "..", "images", "mandelbrot.bmp")
  path = File.expand_path(path)
  std_path = Std::Filesystem::Path.new(path)
  image.save(std_path)
end

mandelbrot