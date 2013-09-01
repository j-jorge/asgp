/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::misc_layer class.
 * \author Julien Jorge
 */
#include "rp/layer/misc_layer.hpp"
#include "rp/game_variables.hpp"

#include "engine/game.hpp"
#include "engine/level.hpp"
#include "engine/level_globals.hpp"
#include "engine/system/game_filesystem.hpp"
#include "input/keyboard.hpp"
#include "visual/font/font.hpp"
#include "visual/scene_sprite.hpp"

#include <sstream>
#include <iomanip>
#include <claw/bitmap.hpp>
#include <claw/logger.hpp>
#include <claw/png.hpp>

#include <boost/thread.hpp>

#ifdef RP_TRACE_FPS
typedef std::list<unsigned int> fps_list;
static fps_list g_fps;
#endif

/*----------------------------------------------------------------------------*/
/**
 * \brief Execute the function object to save the image.
 */
template<typename ImageWriter>
void rp::misc_layer::screenshot_file_save<ImageWriter>::operator()() const
{
  try
    {
      std::ofstream f( file_path.c_str() );
      ImageWriter writer(*image, f);
    }
  catch(std::exception& e)
    {
      claw::logger << claw::log_error << "Screenshot: " << e.what()
                   << std::endl;
    }
} // misc_layer::screenshot_file_save::operator()()




/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::misc_layer::misc_layer()
  : m_fps_text(NULL), m_fps_count(0), m_its_count(0),
#ifdef RP_TRACE_FPS
    m_show_fps(true),
#else
    m_show_fps(true),
#endif
    m_cursor_position(-1, -1), m_first_screenshot(0),
    m_fps_key(bear::input::keyboard::kc_F2),
    m_screenshot_key(bear::input::keyboard::kc_F5),
    m_fullscreen_key(bear::input::keyboard::kc_F12),
    m_screenshot_sequence_key(bear::input::keyboard::kc_F11),
    m_levelshot_key(bear::input::keyboard::kc_F10),
    m_overview_key(bear::input::keyboard::kc_F8)
{

} // misc_layer::misc_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Destructor.
 */
rp::misc_layer::~misc_layer()
{
  delete m_fps_text;

  if (m_first_screenshot != 0)
    stop_screenshot_sequence();

#ifdef RP_TRACE_FPS
  std::ofstream f("fps.txt");
  std::size_t i=0;
  for ( fps_list::const_iterator it=g_fps.begin(); it!=g_fps.end(); ++it, ++i )
    f << i << '\t' << *it << '\n';
#endif
} // misc_layer::~misc_layer()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialise the layer.
 */
void rp::misc_layer::build()
{
  m_fps_text =
    new bear::gui::static_text
    ( get_level_globals().get_font("font/fixed-10x20.fnt",12) );

  m_fps_text->set_auto_size(true);
  m_fps_text->set_text("0");
  m_fps_text->set_position( m_fps_text->height(), m_fps_text->height() );
  m_fps_text->get_rendering_attributes().set_intensity( 0, 0, 0 );

  m_last_fps_check = bear::systime::get_date_ms();

  m_cursor =
    get_level_globals().auto_sprite( "gfx/status/cursor.png", "default" );
} // misc_layer::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Do one step in the progression of the layer.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::misc_layer::progress( bear::universe::time_type elapsed_time )
{
  ++m_its_count;

  if ( m_first_screenshot != 0 )
    sequence_screenshot();
} // misc_layer::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the visibles components of the layer on a screen.
 * \param e (out) The scene elements.
 */
void rp::misc_layer::render( scene_element_list& e ) const
{
  ++m_fps_count;
  render_fps( e );

  if ( get_level().is_paused() || game_variables::get_ending_effect() )
    e.push_back
      ( bear::visual::scene_sprite
        ( m_cursor_position.x - m_cursor.width() / 2, 
          m_cursor_position.y - m_cursor.height() / 2,
          m_cursor) );
} // misc_layer::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform the layer that a key had been pressed.
 * \param key The value of the pressed key.
 */
bool rp::misc_layer::key_pressed( const bear::input::key_info& key )
{
  bool result = true;

  if ( key.get_code() == m_fps_key )
    m_show_fps = !m_show_fps;
  else if ( key.get_code() == m_screenshot_key )
    screenshot();
  else if ( key.get_code() == m_fullscreen_key )
    bear::engine::game::get_instance().toggle_fullscreen();
#ifdef RP_VIDEO_ENABLED
  else if ( key.get_code() == m_levelshot_key )
    levelshot();
  else if ( key.get_code() == m_screenshot_sequence_key )
    {
      if (m_first_screenshot != 0)
        stop_screenshot_sequence();
      else
        start_screenshot_sequence();
    }
  else if ( key.get_code() == m_overview_key )
    bear::engine::game::get_instance().toggle_overview_mode();
#endif
  else
    result = false;

  return result;
} // misc_layer::key_pressed()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform the layer that the mouse had been moved.
 * \param pos The new position of the mouse.
 */
bool rp::misc_layer::mouse_move
( const claw::math::coordinate_2d<unsigned int>& pos )
{
  m_cursor_position = pos;

  // let the sub components know the movement
  return false;
} // misc_layer::mouse_move()

/*----------------------------------------------------------------------------*/
/**
 * \brief Take a picture of the whole level.
 */
void rp::misc_layer::levelshot() const
{
  std::ostringstream name;
  name << "level-" << bear::systime::get_date_ms() << ".bmp";

  screenshot_file_save<claw::graphic::bitmap::writer> writer;
  writer.image = new claw::graphic::image;

  bear::engine::game::get_instance().levelshot(*writer.image);

  const bear::engine::game& g( bear::engine::game::get_instance() );

  writer.file_path =
    g.get_game_filesystem().get_custom_data_file_name( name.str() );

  boost::thread t(writer);
} // misc_layer::levelshot()

/*----------------------------------------------------------------------------*/
/**
 * \brief Take a picture of the screen.
 */
void rp::misc_layer::screenshot() const
{
  std::ostringstream name;

  name << game_variables::get_serial_number() << '-'
       << game_variables::get_level_number() << '-';

  name << bear::systime::get_date_ms() << ".png";

  screenshot<claw::graphic::png::writer>(name.str());
} // misc_layer::screenshot()

/*----------------------------------------------------------------------------*/
/**
 * \brief Take a picture of the screen.
 * \param name The name of the file to save the image into.
 */
template<typename ImageWriter>
void rp::misc_layer::screenshot( const std::string& name ) const
{
  screenshot_file_save<ImageWriter> writer;
  writer.image = new claw::graphic::image;

  bear::engine::game::get_instance().screenshot(*writer.image);

  const bear::engine::game& g( bear::engine::game::get_instance() );

  writer.file_path = 
    g.get_game_filesystem().get_custom_data_file_name( name );

  //boost::thread t(writer);
  writer();
} // misc_layer::screenshot()

/*----------------------------------------------------------------------------*/
/**
 * \brief Take a picture of the screen for the screenshot sequence.
 */
void rp::misc_layer::sequence_screenshot()
{
  //if ( bear::systime::get_date_ms() >= m_last_screenshot + 40 ) // 25 fps
    {
      std::ostringstream name;
      name << m_screenshot_prefix << '-' << std::setw(8)
           << std::setfill('0') << m_screenshots_count << ".bmp";
      ++m_screenshots_count;

      screenshot<claw::graphic::bitmap::writer>(name.str());
      m_last_screenshot = bear::systime::get_date_ms();
    }
} // misc_layer::sequence_screenshot()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the images in the sequence.
 */
void rp::misc_layer::start_screenshot_sequence()
{
  claw::logger << claw::log_verbose << "Starting screenshot sequence."
               << std::endl;

  m_first_screenshot = bear::systime::get_date_ms();
  m_last_screenshot = m_first_screenshot;
  m_screenshots_count = 0;

  std::ostringstream prefix;
  prefix << "s-" << bear::systime::get_date_ms();
  m_screenshot_prefix = prefix.str();
} // misc_layer::start_screenshot_sequence()

/*----------------------------------------------------------------------------*/
/**
 * \brief Save the data associated with the screenshots of the sequence.
 */
void rp::misc_layer::stop_screenshot_sequence()
{
  const bear::engine::game& g( bear::engine::game::get_instance() );

  std::string file_name
    ( g.get_game_filesystem().get_custom_data_file_name
      ( m_screenshot_prefix + ".seq" ) );

  std::ofstream f( file_name.c_str() );

  const bear::systime::milliseconds_type d = bear::systime::get_date_ms();
  const unsigned int fps =
    (unsigned int)
    ( (double)m_screenshots_count
      / ((double)(d - m_first_screenshot) / 1000.0)
      + 0.5 );

  f << fps << " # fps" << std::endl;

  claw::logger << claw::log_verbose << "Screenshot sequence stopped. " << fps
               << " fps during " << ((double)(d - m_first_screenshot) / 1000.0)
               << " seconds." << std::endl;

  m_first_screenshot = 0;
} // misc_layer::stop_screenshot_sequence()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the fps text component.
 * \param e (out) The scene elements.
 */
void rp::misc_layer::render_fps( scene_element_list& e ) const
{
  if ( m_show_fps )
    {
      const bear::systime::milliseconds_type current_time =
        bear::systime::get_date_ms();

      if ( current_time - m_last_fps_check >= 1000 )
        {
          std::ostringstream oss;
          oss << m_fps_count << " fps - " << m_its_count << " its";

#ifdef RP_TRACE_FPS
          g_fps.push_back(m_fps_count);
#endif

          m_fps_text->set_text( oss.str() );
          m_fps_count = 0;
          m_its_count = 0;
          m_last_fps_check = bear::systime::get_date_ms();
        }

      m_fps_text->render( e );
    }
} // misc_layer::render_fps()
