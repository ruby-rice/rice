require "minitest/autorun"

# Build the UNSAFE test extension if needed (no RICE_RACTOR_SAFE)
ext_dir = File.expand_path("ext_unsafe", __dir__)
bundle = File.join(ext_dir, "ractor_test_unsafe_ext.bundle")
so = File.join(ext_dir, "ractor_test_unsafe_ext.so")

unless File.exist?(bundle) || File.exist?(so)
  Dir.chdir(ext_dir) do
    system(RbConfig.ruby, "extconf.rb", exception: true)
    system("make", "clean", exception: true)
    system("make", exception: true)
  end
end

class RactorUnsafeTest < Minitest::Test
  # Verify that WITHOUT RICE_RACTOR_SAFE, concurrent Ractor access to
  # Rice-wrapped objects fails — either by crashing (segfault/abort) or
  # by hanging (corrupted std::map causes infinite loop).
  #
  # This proves the mutex is necessary, not just defensive.
  #
  # Runs in a subprocess with a timeout so neither a crash nor a hang
  # kills the test runner.
  #
  # The number of objects is calibrated to fill ~1 second of rapid-fire
  # Counter.new calls, ensuring sufficient registry contention on any machine.

  TIMEOUT_SECONDS = 30

  SUBPROCESS_SCRIPT = <<~'RUBY'
    $LOAD_PATH.unshift(ARGV[0])
    require "ractor_test_unsafe_ext"

    # Calibrate: how many Counter.new(0) fit in ~1 second?
    count = 1_000
    loop do
      t = Process.clock_gettime(Process::CLOCK_MONOTONIC)
      count.times { Counter.new(0) }
      break if Process.clock_gettime(Process::CLOCK_MONOTONIC) - t >= 1.0
      count *= 2
    end

    # Launch concurrent Ractors with calibrated count
    r1 = Ractor.new(count) { |n| n.times { Counter.new(0) }; :ok }
    r2 = Ractor.new(count) { |n| n.times { Counter.new(0) }; :ok }
    r1.take
    r2.take
  RUBY

  def test_concurrent_access_without_mutex_fails
    ext_dir = File.expand_path("ext_unsafe", __dir__)

    pid = spawn(
      RbConfig.ruby, "-e", SUBPROCESS_SCRIPT, ext_dir,
      out: File::NULL, err: File::NULL
    )

    # Wait with timeout — hang is also a failure mode
    deadline = Process.clock_gettime(Process::CLOCK_MONOTONIC) + TIMEOUT_SECONDS
    status = nil

    loop do
      _, status = Process.wait2(pid, Process::WNOHANG)
      break if status
      if Process.clock_gettime(Process::CLOCK_MONOTONIC) > deadline
        Process.kill("KILL", pid)
        _, status = Process.wait2(pid)
        break
      end
      sleep 0.1
    end

    if status.success?
      skip "Race condition did not manifest in this run (non-deterministic)"
    elsif status.termsig == 9 && Process.clock_gettime(Process::CLOCK_MONOTONIC) >= deadline - 1
      # We killed it after timeout — it was hanging (corrupted map)
      pass # Hang confirms the corruption
    else
      # Crashed with a signal — segfault, abort, etc.
      assert status.signaled?,
        "Expected crash or hang without RICE_RACTOR_SAFE, got exit #{status.exitstatus}"
    end
  end
end
