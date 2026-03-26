require "minitest/autorun"

# Build the test extension if needed
ext_dir = File.expand_path("ext", __dir__)
bundle = File.join(ext_dir, "ractor_test_ext.bundle")
so = File.join(ext_dir, "ractor_test_ext.so")

unless File.exist?(bundle) || File.exist?(so)
  Dir.chdir(ext_dir) do
    system(RbConfig.ruby, "extconf.rb", exception: true)
    system("make", "clean", exception: true)
    system("make", exception: true)
  end
end

$LOAD_PATH.unshift(ext_dir)
require "ractor_test_ext"

class RactorSafetyTest < Minitest::Test
  def test_single_ractor
    r = Ractor.new do
      c = Counter.new(10)
      c.increment
      c.increment
      c.value
    end

    assert_equal 12, r.take
  end

  def test_sequential_ractors
    results = 5.times.map do |i|
      r = Ractor.new(i) do |start|
        c = Counter.new(start)
        c.increment
        c.value
      end
      r.take
    end

    assert_equal [1, 2, 3, 4, 5], results
  end

  def test_concurrent_ractors
    r1 = Ractor.new do
      c = Counter.new(100)
      c.increment
      c.value
    end

    r2 = Ractor.new do
      c = Counter.new(200)
      c.increment
      c.value
    end

    assert_equal 101, r1.take
    assert_equal 201, r2.take
  end

  def test_many_objects_in_ractor
    r = Ractor.new do
      counters = 50.times.map { |i| Counter.new(i) }
      counters.each(&:increment)
      counters.map(&:value)
    end

    result = r.take
    assert_equal 50, result.size
    assert_equal 50.times.map { |i| i + 1 }, result
  end

  def test_main_thread_and_ractor
    main_counter = Counter.new(0)
    main_counter.increment

    r = Ractor.new do
      c = Counter.new(1000)
      c.increment
      c.value
    end

    assert_equal 1, main_counter.value
    assert_equal 1001, r.take
  end

  # --- Heavy concurrency test ---
  #
  # Calibrates HeavyObject constructor to take ~1 second, then launches
  # two Ractors creating objects simultaneously. Each new() spends ~1s
  # inside Rice's InstanceRegistry, making concurrent collisions certain.
  # Without RICE_RACTOR_SAFE this would segfault.

  TARGET_SECONDS = 1.0
  OBJECTS_PER_RACTOR = 3

  def test_concurrent_heavy_objects
    size = calibrate_heavy_object_size

    r1 = Ractor.new(size) do |sz|
      OBJECTS_PER_RACTOR.times.map { HeavyObject.new(sz).sum }
    end

    r2 = Ractor.new(size) do |sz|
      OBJECTS_PER_RACTOR.times.map { HeavyObject.new(sz).sum }
    end

    results1 = r1.take
    results2 = r2.take

    assert_equal OBJECTS_PER_RACTOR, results1.size
    assert_equal OBJECTS_PER_RACTOR, results2.size

    # All sums must be identical (same size → same computation)
    expected_sum = results1.first
    (results1 + results2).each do |s|
      assert_equal expected_sum, s
    end
  end

  private

  # Find the size parameter that makes HeavyObject.new take ~TARGET_SECONDS.
  # Uses exponential search then binary refinement.
  def calibrate_heavy_object_size
    # Exponential search for upper bound
    size = 1_000
    loop do
      elapsed = measure_seconds { HeavyObject.new(size) }
      break if elapsed >= TARGET_SECONDS
      size *= 2
    end

    # Binary search between size/2 and size
    lo = size / 2
    hi = size
    while hi - lo > lo / 10  # ~10% precision is enough
      mid = (lo + hi) / 2
      elapsed = measure_seconds { HeavyObject.new(mid) }
      if elapsed < TARGET_SECONDS
        lo = mid
      else
        hi = mid
      end
    end

    (lo + hi) / 2
  end

  def measure_seconds
    t = Process.clock_gettime(Process::CLOCK_MONOTONIC)
    yield
    Process.clock_gettime(Process::CLOCK_MONOTONIC) - t
  end
end
