#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fmt/base.h>
#include <fmt/format.h>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>
#include <functional>
#include <optional>

#include <random>

#include <CLI/CLI.hpp>
#include <ftxui/component/component.hpp>// for Slider
#include <ftxui/component/screen_interactive.hpp>// for ScreenInteractive
#include <spdlog/spdlog.h>

#include <lefticus/tools/non_promoting_ints.hpp>

// This file will be generated automatically when cur_you run the CMake
// configuration step. It creates a namespace called `Clock`. You can modify
// the source template at `configured_files/config.hpp.in`.
#include <internal_use_only/config.hpp>
#include <string>
#include <thread>
#include <utility>
#include <vector>


/**
 * Game board representation.
 */
template<std::size_t Width, std::size_t Height> struct GameBoard
{
  /**
   * The width of the game board.
   */
  static constexpr std::size_t WIDTH = Width;
  /**
   * The height of the game board.
   */
  static constexpr std::size_t HEIGHT = Height;

  /**
   * The game board is represented as a 2D array of strings and values.
   * The strings are used to display the state of each cell, and the values
   * are used to store the actual state of each cell.
   */
  std::array<std::array<std::string, HEIGHT>, WIDTH> strings;

  /**
   * The values are used to store the actual state of each cell.
   */
  std::array<std::array<bool, HEIGHT>, WIDTH> values{};

  /**
   * The number of moves made in the game.
   */
  std::size_t move_count{ 0 };

  /**
   * Accessor for the string at the given coordinates.
   * @param cur_x The x-coordinate of the string.
   * @param cur_y The y-coordinate of the string.
   */
  std::string &getString(std::size_t cur_x, std::size_t cur_y)
  {
    return strings.at(cur_x).at(cur_y);
  }


  /**
   * Sets the value at the given coordinates and updates the string
   * representation.
   * @param cur_x The x-coordinate of the value.
   * @param cur_y The y-coordinate of the value.
   * @param new_value The new value to set.
   */
  void set(std::size_t cur_x, std::size_t cur_y, bool new_value)
  {
    get(cur_x, cur_y) = new_value;

    if (new_value) {
      getString(cur_x, cur_y) = " ON";
    } else {
      getString(cur_x, cur_y) = "OFF";
    }
  }

  /**
   * Visits each cell in the game board and applies the given visitor function.
   * @param visitor The function to apply to each cell.
   */
  void visit(auto visitor)
  {
    for (std::size_t cur_x = 0; cur_x < WIDTH; ++cur_x) {
      for (std::size_t cur_y = 0; cur_y < HEIGHT; ++cur_y) {
        visitor(cur_x, cur_y, *this);
      }
    }
  }

  /**
   * Accessor for the value at the given coordinates.
   * @param cur_x The x-coordinate of the value.
   * @param cur_y The y-coordinate of the value.
   * @return A reference to the value at the given coordinates.
   */
  [[nodiscard]] bool get(std::size_t cur_x, std::size_t cur_y) const
  {
    return values.at(cur_x).at(cur_y);
  }

  /**
   * Accessor for the value at the given coordinates.
   * @param cur_x The x-coordinate of the value.
   * @param cur_y The y-coordinate of the value.
   * @return A reference to the value at the given coordinates.
   */
  [[nodiscard]] bool &get(std::size_t cur_x, std::size_t cur_y)
  {
    return values.at(cur_x).at(cur_y);
  }

  GameBoard()
  {
    visit([](const auto cur_x, const auto cur_y, auto &gameboard) {
      gameboard.set(cur_x, cur_y, true);
    });
  }

  /**
   * Updates the string representation of the game board.
   */
  void updateStrings()
  {
    for (std::size_t cur_x = 0; cur_x < WIDTH; ++cur_x) {
      for (std::size_t cur_y = 0; cur_y < HEIGHT; ++cur_y) {
        set(cur_x, cur_y, get(cur_x, cur_y));
      }
    }
  }

  /**
   * Toggles the value at the given coordinates and updates the surrounding
   * values.
   * @param cur_x The x-coordinate of the value.
   * @param cur_y The y-coordinate of the value.
   */
  void toggle(std::size_t cur_x, std::size_t cur_y)
  {
    set(cur_x, cur_y, !get(cur_x, cur_y));
  }

  /**
   * Presses the button at the given coordinates and toggles the surrounding
   * values.
   * @param cur_x The x-coordinate of the button.
   * @param cur_y The y-coordinate of the button.
   */
  void press(std::size_t cur_x, std::size_t cur_y)
  {
    ++move_count;
    toggle(cur_x, cur_y);
    if (cur_x > 0) { toggle(cur_x - 1, cur_y); }
    if (cur_y > 0) { toggle(cur_x, cur_y - 1); }
    if (cur_x < WIDTH - 1) { toggle(cur_x + 1, cur_y); }
    if (cur_y < HEIGHT - 1) { toggle(cur_x, cur_y + 1); }
  }

  /**
   * Checks if the game board is solved.
   * @return True if all values are true, false otherwise.
   */
  [[nodiscard]] bool solved() const
  {
    for (std::size_t cur_x = 0; cur_x < WIDTH; ++cur_x) {
      for (std::size_t cur_y = 0; cur_y < HEIGHT; ++cur_y) {
        if (!get(cur_x, cur_y)) { return false; }
      }
    }

    return true;
  }
};

namespace {
void consequenceGame()
{
  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  GameBoard<3, 3> game_board;

  std::string quit_text;

  const auto update_quit_text = [&quit_text](const auto &game_board_param) {
    quit_text = fmt::format("Quit ({} moves)", game_board_param.move_count);
    if (game_board_param.solved()) { quit_text += " Solved!"; }
  };

  const auto make_buttons = [&] {
    std::vector<ftxui::Component> buttons;
    for (std::size_t cur_x = 0; cur_x < game_board.WIDTH; ++cur_x) {
      for (std::size_t cur_y = 0; cur_y < game_board.HEIGHT; ++cur_y) {
        buttons.push_back(
          ftxui::Button(&game_board.getString(cur_x, cur_y), [=, &game_board] {
            if (!game_board.solved()) { game_board.press(cur_x, cur_y); }
            update_quit_text(game_board);
          }));
      }
    }
    return buttons;
  };

  auto buttons = make_buttons();

  auto quit_button = ftxui::Button(&quit_text, screen.ExitLoopClosure());

  auto make_layout = [&] {
    std::vector<ftxui::Element> rows;

    std::size_t idx = 0;

    for (std::size_t cur_x = 0; cur_x < game_board.WIDTH; ++cur_x) {
      std::vector<ftxui::Element> row;
      for (std::size_t cur_y = 0; cur_y < game_board.HEIGHT; ++cur_y) {
        row.push_back(buttons[idx]->Render());
        ++idx;
      }
      rows.push_back(ftxui::hbox(std::move(row)));
    }

    rows.push_back(ftxui::hbox({ quit_button->Render() }));

    return ftxui::vbox(std::move(rows));
  };


  static constexpr int RANDOMIZATION_ITERATIONS = 100;
  static constexpr int RANDOM_SEED = 42;

  std::mt19937 gen32{ RANDOM_SEED };// NOLINT fixed seed

  // NOLINTNEXTLINE This cannot be const
  std::uniform_int_distribution<std::size_t> cur_x(
    static_cast<std::size_t>(0), game_board.WIDTH - 1);
  // NOLINTNEXTLINE This cannot be const
  std::uniform_int_distribution<std::size_t> cur_y(
    static_cast<std::size_t>(0), game_board.HEIGHT - 1);

  for (int i = 0; i < RANDOMIZATION_ITERATIONS; ++i) {
    game_board.press(cur_x(gen32), cur_y(gen32));
  }
  game_board.move_count = 0;
  update_quit_text(game_board);

  auto all_buttons = buttons;
  all_buttons.push_back(quit_button);
  auto container = ftxui::Container::Horizontal(all_buttons);

  auto renderer = ftxui::Renderer(container, make_layout);

  screen.Loop(renderer);
}
}// namespace

/**
 * RGB color representation.
 */
struct Color
{
  /// Represents the red component of the color.
  lefticus::tools::uint_np8_t r{ static_cast<std::uint8_t>(0) };
  /// Represents the green component of the color.
  lefticus::tools::uint_np8_t g{ static_cast<std::uint8_t>(0) };
  /// Represents the blue component of the color.
  lefticus::tools::uint_np8_t b{ static_cast<std::uint8_t>(0) };
};

// A simple way of representing a bitmap on screen using only characters
/**
 * This is a custom Node that can be used to render a bitmap on the screen.
 */
struct Bitmap : ftxui::Node
{
  /**
   * Constructor for Bitmap.
   *
   * @param width The width of the bitmap.
   * @param height The height of the bitmap.
   */
  Bitmap(std::size_t width,// NOLINT
    std::size_t height)// NOLINT same typed parameters adjacent to each other
    : width_(width), height_(height)
  {}

  /**
   * Accessor for the pixel at the given coordinates.
   *
   * @param cur_x The x-coordinate of the pixel.
   * @param cur_y The y-coordinate of the pixel.
   * @return A reference to the pixel at the given coordinates.
   */
  Color &at(std::size_t cur_x, std::size_t cur_y)
  {
    return pixels_.at((width_ * cur_y) + cur_x);
  }

  /**
   * Computes the requirement for the bitmap.
   * This is used to determine how much space the bitmap needs on the screen.
   * It sets the minimum width and height of the bitmap.
   */
  void ComputeRequirement() override
  {
    requirement_.min_x = static_cast<int>(width_);
    requirement_.min_y = static_cast<int>(height_ / 2);
  }

  /**
   * Renders the bitmap on the given screen.
   *
   * @param screen The screen to render the bitmap on.
   */
  void Render(ftxui::Screen &screen) override
  {
    for (std::size_t cur_x = 0; cur_x < width_; ++cur_x) {
      for (std::size_t cur_y = 0; cur_y < height_ / 2; ++cur_y) {
        auto &pixel = screen.PixelAt(box_.x_min + static_cast<int>(cur_x),
          box_.y_min + static_cast<int>(cur_y));
        pixel.character = "▄";
        const auto &top_color = at(cur_x, cur_y * 2);
        const auto &bottom_color = at(cur_x, (cur_y * 2) + 1);
        pixel.background_color = ftxui::Color{
          top_color.r.get(), top_color.g.get(), top_color.b.get()
        };
        pixel.foreground_color = ftxui::Color{
          bottom_color.r.get(), bottom_color.g.get(), bottom_color.b.get()
        };
      }
    }
  }

  /**
   * Accessor for the width of the bitmap.
   *
   * @return The width of the bitmap.
   */
  [[nodiscard]] auto width() const noexcept { return width_; }

  /**
   * Accessor for the height of the bitmap.
   *
   * @return The height of the bitmap.
   */
  [[nodiscard]] auto height() const noexcept { return height_; }

  /**
   * Accessor for the pixel data of the bitmap.
   *
   * @return A reference to the pixel data.
   */
  [[nodiscard]] const auto &data() const noexcept { return pixels_; }

  /**
   * Accessor for the pixel data of the bitmap.
   *
   * @return A reference to the pixel data.
   */
  [[nodiscard]] auto &data() noexcept { return pixels_; }

private:
  std::size_t width_;
  std::size_t height_;

  std::vector<Color> pixels_ = std::vector<Color>(width_ * height_, Color{});
};

namespace {
void gameIterationCanvas()
{
  // this should probably have a `bitmap` helper function that does what cur_you
  // expect similar to the other parts of FTXUI
  auto bm = std::make_shared<Bitmap>(50, 50);// NOLINT magic numbers
  auto small_bm = std::make_shared<Bitmap>(6, 6);// NOLINT magic numbers

  double fps = 0;

  std::size_t max_row = 0;
  std::size_t max_col = 0;

  // to do, add total game time clock also, not just current elapsed time
  auto game_iteration =
    [&](const std::chrono::steady_clock::duration elapsed_time) {
      // in here we simulate however much game time has elapsed. Update
      // animations, run character AI, whatever, update stats, etc

      // this isn't actually timing based for now, it's just updating the
      // display however fast it can
      fps =
        1.0
        / (static_cast<double>(
             std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time)
               .count())
           / 1'000'000.0);// NOLINT magic numbers

      for (std::size_t row = 0; row < max_row; ++row) {
        for (std::size_t col = 0; col < bm->width(); ++col) {
          ++(bm->at(col, row).r);
        }
      }

      for (std::size_t row = 0; row < bm->height(); ++row) {
        for (std::size_t col = 0; col < max_col; ++col) {
          ++(bm->at(col, row).g);
        }
      }

      // for the fun of it, let's have a second window doing interesting things
      auto &small_bm_pixel =
        small_bm->data().at(static_cast<std::size_t>(elapsed_time.count())
                            % small_bm->data().size());

      switch (elapsed_time.count() % 3) {
      case 0:
        small_bm_pixel.r += 11;// NOLINT Magic Number
        break;
      case 1:
        small_bm_pixel.g += 11;// NOLINT Magic Number
        break;
      case 2:
        small_bm_pixel.b += 11;// NOLINT Magic Number
        break;
      default:// literally impossible
#ifdef __GNUC__
        __builtin_unreachable();
#elif _MSC_VER
        __assume(false);
#else
        std::terminate();// Fallback for other compilers
#endif
      }


      ++max_row;
      if (max_row >= bm->height()) { max_row = 0; }
      ++max_col;
      if (max_col >= bm->width()) { max_col = 0; }
    };

  auto screen = ftxui::ScreenInteractive::TerminalOutput();

  int counter = 0;

  auto last_time = std::chrono::steady_clock::now();

  auto make_layout = [&] {
    // This code actually processes the draw event
    const auto new_time = std::chrono::steady_clock::now();

    ++counter;
    // we will dispatch to the game_iteration function, where the work happens
    game_iteration(new_time - last_time);
    last_time = new_time;

    // now actually draw the game elements
    return ftxui::hbox({ bm | ftxui::border,
      ftxui::vbox({ ftxui::text("Frame: " + std::to_string(counter)),
        ftxui::text("FPS: " + std::to_string(fps)),
        small_bm | ftxui::border }) });
  };

  auto renderer = ftxui::Renderer(make_layout);


  std::atomic<bool> refresh_ui_continue = true;

  // This thread exists to make sure that the event queue has an event to
  // process at approximately a rate of 30 FPS
  std::thread refresh_ui([&] {
    while (refresh_ui_continue) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(1.0s / 30.0);// NOLINT magic numbers
      screen.PostEvent(ftxui::Event::Custom);
    }
  });

  screen.Loop(renderer);

  refresh_ui_continue = false;
  refresh_ui.join();
}
}// namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, const char **argv)
{
  try {
    CLI::App app{ fmt::format("{} version {}",
      Clock::cmake::project_name,
      Clock::cmake::project_version) };

    std::optional<std::string> message;
    app.add_option("-m,--message", message, "A message to print back out");
    bool show_version = false;
    app.add_flag("--version", show_version, "Show version information");

    bool is_turn_based = false;
    auto *turn_based = app.add_flag("--turn_based", is_turn_based);

    bool is_loop_based = false;
    auto *loop_based = app.add_flag("--loop_based", is_loop_based);

    turn_based->excludes(loop_based);
    loop_based->excludes(turn_based);


    CLI11_PARSE(app, argc, argv);

    if (show_version) {
      fmt::print("{}\n", Clock::cmake::project_version);
      return EXIT_SUCCESS;
    }

    if (is_turn_based) {
      consequenceGame();
    } else {
      gameIterationCanvas();
    }

  } catch (const std::exception &e) {
    spdlog::error("Unhandled exception in main: {}", e.what());
  }
}
