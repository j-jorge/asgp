/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::level_ending_effect class.
 * \author Julien Jorge
 */
#include "rp/transition_effect/level_ending_effect.hpp"

#include "rp/game_variables.hpp"
#include "rp/defines.hpp"
#include "rp/rp_gettext.hpp"
#include "rp/cart.hpp"
#include "rp/util.hpp"
#include "rp/version.hpp"

#include "engine/game.hpp"
#include "engine/level.hpp"
#include "engine/level_globals.hpp"

#include "visual/scene_element_sequence.hpp"
#include "visual/scene_line.hpp"
#include "visual/scene_rectangle.hpp"
#include "visual/scene_sprite.hpp"
#include "visual/scene_writing.hpp"

#include "universe/forced_movement/forced_goto.hpp"

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include <claw/socket_stream.hpp>
#include <claw/string_algorithm.hpp>
#include <claw/tween/easing/easing_linear.hpp>

#include <sstream>

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructs a new instance that won't do anything.
 */
rp::level_ending_effect::score_request::score_request()
  : m_is_active( new bool(true) ), m_shared_mutex( new boost::mutex )
{

} // level_ending_effect::score_request::score_request()

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructs a new instance with a given callback function.
 * \param c The function to call when the score is ready.
 * \param level_name The file name of the level for which we want the score.
 */
rp::level_ending_effect::score_request::score_request
( callback_type c, std::string level_name )
  : m_callback( c ), m_level_name( level_name ),
    m_is_active( new bool(true) ), m_shared_mutex( new boost::mutex )
{

} // level_ending_effect::score_request::score_request()

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructs a new instance by copying another instance.
 * \param that The instance to copy.
 */
rp::level_ending_effect::score_request::score_request
( const score_request& that )
  : m_callback( that.m_callback ), m_level_name( that.m_level_name ),
    m_is_active( that.m_is_active ), m_shared_mutex( that.m_shared_mutex )
{

} // level_ending_effect::score_request::score_request()

/*----------------------------------------------------------------------------*/
/**
 * \brief Returns the name of the level for which we request the score.
 */
std::string rp::level_ending_effect::score_request::get_level_name() const
{
  return m_level_name;
} // level_ending_effect::score_request::get_level_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tells to not call the callback when the score is ready.
 */
void rp::level_ending_effect::score_request::disable()
{
  boost::mutex::scoped_lock lock(*m_shared_mutex);

  *m_is_active = false;
} // level_ending_effect::score_request::disable()

/*----------------------------------------------------------------------------*/
/**
 * \brief Retrieves the score from the server and call the callback.
 */
void rp::level_ending_effect::score_request::operator()()
{
  const std::string server( "www.stuff-o-matic.com" );
  const int port( 80 );

  claw::net::socket_stream server_connection( "www.stuff-o-matic.com", port );

  if ( !server_connection )
    {
#ifdef _DEBUG
      claw::logger << claw::log_verbose << "Cannot connect to " << server
                   << " on port " << port << "." << std::endl;
#endif
      return;
    }

  const std::string page( "/asgp/stats/best_score.php?level=" + m_level_name );

  std::ostringstream request;
  request << "GET " << page << " HTTP/1.1\n"
          << "Host: www.stuff-o-matic.com\n"
          << "User-Agent: " << RP_VERSION_STRING << "\n"
          << "Connection: Close\n"
          << '\n';

  server_connection << request.str() << std::flush;

  std::string protocol;
  int code;
  std::string line;

  if ( server_connection >> protocol >> code )
    if ( claw::text::getline( server_connection, line ) )
      {
#ifdef _DEBUG
        claw::logger << claw::log_verbose <<  "Stats server response: " << code
                     << ' ' << line << std::endl;
#endif
        if ( code == 200 /* OK */ )
          {
            // Find the first empty line that separate the header and the body.
            while ( !line.empty() )
              claw::text::getline( server_connection, line );

            // The body is the score alone
            unsigned int score;
            if ( server_connection >> score )
              call_callback( score );
          }
      }
} // level_ending_effect::score_request::operator()()

/*----------------------------------------------------------------------------*/
/**
 * \brief Calls the callback with a given value.
 * \param s The value to pass to the callback.
 */
void rp::level_ending_effect::score_request::call_callback( unsigned int c )
{
  boost::mutex::scoped_lock lock(*m_shared_mutex);

  if ( *m_is_active )
    m_callback( c );
} // level_ending_effect::score_request::call_callback()

/*----------------------------------------------------------------------------*/
/**
 * \brief Assigns another score request to this one.
 * \param that The instance to copy.
 */
rp::level_ending_effect::score_request&
rp::level_ending_effect::score_request::operator=( score_request that )
{
  boost::mutex::scoped_lock lock(*m_shared_mutex);

  m_callback = that.m_callback;
  m_level_name = that.m_level_name;
  m_is_active = that.m_is_active;
  m_shared_mutex = that.m_shared_mutex;

  return *this;
} // level_ending_effect::score_request::operator=()




/*----------------------------------------------------------------------------*/
const bear::visual::coordinate_type
rp::level_ending_effect::score_line::s_shadow_delta(2);
const double rp::level_ending_effect::score_line::s_scale_factor(0.5);
const bear::visual::coordinate_type
rp::level_ending_effect::score_line::s_bonus_picture_margin(10);

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param f The font to use for the texts.
 * \param text A short description of the reason we're giving the points for.
 * \param points The total score.
 * \param picture The bonus picture.
 */
rp::level_ending_effect::score_line::score_line
( const bear::visual::font& f, const std::string& text, int points,
  const bear::visual::sprite& picture )
  : m_font(f), m_label(m_font, text), 
    m_bonus_sprite( picture ),
    m_total_points( points ),
    m_current_points(0), m_y(0), m_negative( points < 0 )
{
  // force creating the writing
  update_score(0);
} // level_ending_effect::score_line::score_line()

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param f The font to use for the texts.
 * \param text A short description of the reason we're giving the points for.
 * \param coef A short description of the points coefficient.
 * \param count A short description of the points count.
 * \param points The total score.
 * \param picture The bonus picture.
 */
rp::level_ending_effect::score_line::score_line
( const bear::visual::font& f, const std::string& text, 
  const std::string& coeff, const std::string& count, int points,
  const bear::visual::sprite& picture )
  : m_font(f), m_label(m_font, text), 
    m_computation_label(m_font, count),
    m_computation_coefficient_label(m_font, coeff),
    m_total_points(points),
    m_current_points(0), m_y(0), m_negative( points < 0 )
{
  // force creating the writing
  update_score(0);

  m_bonus_sprite = picture;
} // level_ending_effect::score_line::score_line()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the scene elements of the score.
 * \param e (out) The scene element of the score.
 * \param left The minimum x-position of the elements.
 * \param right The maximum x-position of the elements.
 */
void rp::level_ending_effect::score_line::render
( scene_element_list& e, bear::visual::coordinate_type left,
  bear::visual::coordinate_type right ) const
{
  bear::visual::scene_element_sequence result;
  result.set_shadow( s_shadow_delta, -s_shadow_delta );

  if ( m_negative )
    result.get_rendering_attributes().set_intensity( 0.9, 0.2, 0.2 );
  else
    result.get_rendering_attributes().set_intensity( 1, 1, 1 );

  // text
  bear::visual::scene_writing label
    ( left + m_bonus_sprite.get_size().x + s_bonus_picture_margin, m_y,
      m_label );

  label.set_scale_factor( s_scale_factor, s_scale_factor );
  result.push_back( label );

  // computation text
  bear::visual::scene_writing computation_coeff_label
    ( left - m_computation_coefficient_label.get_width() * s_scale_factor + 800,
      m_y, m_computation_coefficient_label );
  
  computation_coeff_label.set_scale_factor( s_scale_factor, s_scale_factor );
  result.push_back( computation_coeff_label );

  bear::visual::scene_writing computation_label
    ( left + 800, m_y, m_computation_label );
  
  computation_label.set_scale_factor( s_scale_factor, s_scale_factor );
  result.push_back( computation_label );

  // points shadow
  bear::visual::scene_writing points
    ( right - m_points_text.get_width() * s_scale_factor, m_y, m_points_text);

  points.set_scale_factor( s_scale_factor, s_scale_factor );

  result.push_back( points );

  e.push_back( result );

  // The following elements are outside the scene element sequence since we do
  // not change their intensity.

  // underline
  std::vector<bear::visual::position_type> p(2);
  p[0].set( left + m_bonus_sprite.get_size().x, m_y );
  p[1].set( right, p[0].y );

  bear::visual::scene_line underline( 0, 0, RP_ORANGE_PIXEL, p, 1 );

  underline.set_shadow( s_shadow_delta, -s_shadow_delta );
  e.push_back( underline );

  // bonus picture
  bear::visual::scene_sprite sp
    ( left, m_y + ( m_bonus_sprite.get_size().y - get_height() ) / 2,
      m_bonus_sprite );

  sp.set_shadow( s_shadow_delta, -s_shadow_delta );
  e.push_back( sp );
} // level_ending_effect::score_line::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update the score.
 * \param delta The value of the update.
 * \return How much the score was updated.
 */
int rp::level_ending_effect::score_line::update_score( int delta )
{
  const int old_score( m_current_points );

  if ( m_total_points < 0 )
    m_current_points = std::max( m_total_points, m_current_points - delta );
  else
    m_current_points = std::min( m_total_points, m_current_points + delta );

  std::ostringstream oss;

  oss << m_current_points;
  m_points_text.create(m_font, oss.str());

  return m_current_points - old_score;
} // level_ending_effect::score_line::update_score()

/*----------------------------------------------------------------------------*/
/**
 * \brief Returns the total number of points represented by this line.
 */
int rp::level_ending_effect::score_line::get_total_points() const
{
  return m_total_points;
} // level_ending_effect::score_line::get_total_points()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the height of the line.
 */
bear::visual::coordinate_type
rp::level_ending_effect::score_line::get_height() const
{
  return std::max
    ( m_label.get_height() * s_scale_factor + s_shadow_delta,
      m_bonus_sprite.get_size().y );
} // level_ending_effect::score_line::get_height()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get the y-position of the line.
 */
bear::visual::coordinate_type
rp::level_ending_effect::score_line::get_y_position() const
{
  return m_y;
} // level_ending_effect::score_line::get_y_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set the y-position of the line.
 * \param y The position.
 */
void rp::level_ending_effect::score_line::set_y_position
( bear::visual::coordinate_type y )
{
  m_y = y;
} // level_ending_effect::score_line::set_y_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Force this line to display a negative score.
 */
void rp::level_ending_effect::score_line::set_negative()
{
  m_negative = true;
} // level_ending_effect::score_line::set_negative()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tells if this line displays a negative score.
 */
bool rp::level_ending_effect::score_line::is_negative() const
{
  return m_negative;
} // level_ending_effect::score_line::is_negative()





/*----------------------------------------------------------------------------*/
const unsigned int rp::level_ending_effect::s_points_per_second(10000);
const bear::visual::coordinate_type
rp::level_ending_effect::s_screen_margin(100);
const bear::visual::coordinate_type rp::level_ending_effect::s_margin(10);
const double rp::level_ending_effect::s_score_line_speed(150);

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 */
rp::level_ending_effect::level_ending_effect()
  : m_speed_factor(1), m_next_tick(0.1), m_world(NULL), m_cart(NULL), 
    m_applause_sample(NULL), m_update_function(NULL), m_flash_opacity(0),
    m_play_tick(false)
{

} // level_ending_effect::level_ending_effect()

/*----------------------------------------------------------------------------*/
/**
 * \brief Copy constructor.
 * \param that The instance to copy from.
 */
rp::level_ending_effect::level_ending_effect( const level_ending_effect& that )
  : bear::engine::transition_effect(that), m_speed_factor(that.m_speed_factor),
    m_next_tick(that.m_next_tick), m_world(that.m_world), m_cart(that.m_cart),
    m_finished(false), m_medal(0), m_rectangle_opacity(0), 
    m_decorative_medal(NULL), m_pop_level(false), m_active_component(false),
    m_in_fade_out(false), m_applause_sample(NULL), m_update_function(NULL),
    m_flash_opacity(0), m_play_tick(false),
    m_score_request( that.m_score_request )
{

} // level_ending_effect::level_ending_effect()

/*----------------------------------------------------------------------------*/
/**
 * \brief Destructor.
 */
rp::level_ending_effect::~level_ending_effect()
{
  m_score_request.disable();

  delete m_applause_sample;
} // level_ending_effect::~level_ending_effect()
 
/*----------------------------------------------------------------------------*/
/**
 * \brief Set the world in which we search the bonus points.
 * \param w The world.
 */
void rp::level_ending_effect::set_world( const bear::engine::world* w )
{
  m_world = w;
} // level_ending_effect::set_world()

/*----------------------------------------------------------------------------*/
/**
 * \brief Tell if the effect is finished.
 */
bool rp::level_ending_effect::is_finished() const
{
  return false;
} // level_ending_effect::is_finished()

/*----------------------------------------------------------------------------*/
/**
 * \brief Initialize the effect.
 */
void rp::level_ending_effect::build()
{
  m_applause_sample = 
    get_level_globals().new_sample( "sound/medal/applause.ogg" );

  std::ostringstream oss;
  oss << rp_gettext("Total score ") << game_variables::get_score();
  m_points_text.create
    (get_level_globals().get_font("font/fontopo/fontopo.fnt", 50), oss.str());

  const bear::visual::font level_name_font
    ( get_level_globals().get_font("font/fontopo/fontopo.fnt", 50) );

  m_level_name.create
    ( level_name_font, util::get_level_name(),
      bear::visual::text_align::align_center );

  fill_points();

  initialize_line_position( m_positive_points );

  if ( ! game_variables::is_boss_level() )
    update_medal();
          
  m_bronze_medal_sprite = get_level_globals().auto_sprite
    ( "gfx/status/medal/medal.png", "bronze" );
  m_silver_medal_sprite = get_level_globals().auto_sprite
    ( "gfx/status/medal/medal.png", "silver" );
  m_gold_medal_sprite = get_level_globals().auto_sprite
    ( "gfx/status/medal/medal.png", "gold" ); 
  m_new_record_sprite = get_level_globals().auto_sprite
    ( rp_gettext("gfx/status/bonus.png"), "new record" );
  m_new_record_sprite.set_opacity( 0 );

  m_tweener_fade_out = 
    claw::tween::single_tweener
    ( m_rectangle_opacity, 0.0, 1.0, 
      &claw::tween::easing_linear::ease_in_out );

  m_root_window.set_size( get_level().get_camera_focus().size() );
  add_button_component();
  m_background_on_sprite =
    get_level_globals().auto_sprite
    ( "gfx/status/buttons.png", "background on" ); 
  m_background_off_sprite =
    get_level_globals().auto_sprite
    ( "gfx/status/buttons.png", "background off" );

  game_variables::set_ending_effect(true);

  m_play_tick = true;
  m_update_function = &level_ending_effect::update_positive_lines;

  get_best_score();
} // level_ending_effect::build()

/*----------------------------------------------------------------------------*/
/**
 * \brief Adjust the components of the effect.
 * \param elapsed_time Elapsed time since the last call.
 */
bear::universe::time_type
rp::level_ending_effect::progress( bear::universe::time_type elapsed_time )
{  
  if ( get_level().is_paused() )
    return elapsed_time;

  bool do_start_level =
    game_variables::is_boss_level()
    || update_lines( elapsed_time * m_speed_factor );
 
  if ( ! do_start_level ) 
    update_tick(elapsed_time);
  else if ( game_variables::get_persistent_score()
            < game_variables::get_score() )
    m_new_record_sprite.set_opacity(1);

  if ( m_cart != NULL && game_variables::is_boss_level() )
    do_start_level = do_start_level && m_cart->can_finish();
      
  if ( do_start_level && ! m_finished )
    {
      update_unlocked_serial();
      game_variables::set_level_state
        (game_variables::get_serial_number(), 
         game_variables::get_level_number(), m_medal + 2);
       
      if ( game_variables::get_persistent_score()
           <= game_variables::get_score() )
        game_variables::set_persistent_score( game_variables::get_score() );
          
      if ( game_variables::get_persistent_balloon()
           < game_variables::get_balloons_number() )
        game_variables::set_persistent_balloon
          ( game_variables::get_balloons_number() );

      m_finished = true;
      game_variables::set_last_medal( m_medal );
      util::save_game_variables(); 

      m_button->set_icon
        ( get_level_globals().auto_sprite
          ( "gfx/status/buttons.png", "continue" ) );

      if ( game_variables::is_boss_level() )
        create_fade_out_tweener();
    }

  m_speed_factor = 1;
  
  std::ostringstream oss;
  oss << rp_gettext("Total score ") << game_variables::get_score();
  m_points_text.create
    (get_level_globals().get_font("font/fontopo/fontopo.fnt", 50), oss.str());

  if ( ! game_variables::is_boss_level() )
    update_medal();
    
  m_tweener_fade_out.update(elapsed_time);

  return 0;
} // level_ending_effect::progress()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the components of the effect.
 * \param e (out) The scene elements.
 */
void rp::level_ending_effect::render( scene_element_list& e ) const
{
  render_background(e);
  m_root_window.render( e );

  if ( ! game_variables::is_boss_level() )
    {
      render_score_background(e);
      render_score(e);
      render_flash_line(e);
    }

  if ( m_rectangle_opacity > 0 )
    render_opaque_rectangle(e);

  if ( ! game_variables::is_boss_level() )
    render_level_name(e);

  if ( ! game_variables::is_boss_level() )
    render_medal(e);
} // level_ending_effect::render()

/*----------------------------------------------------------------------------*/
/**
 * \brief Set cart.
 * \param c The pointer on the cart.
 */
void rp::level_ending_effect::set_cart(const cart* c)
{
  m_cart = c;
} // level_ending_effect::set_cart()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform the effect that a key had been pressed.
 * \param key The value of the pressed key.
 * \remark This effect pretends to treat all keys, so we can maintain the pause
 *         mode.
 */
bool rp::level_ending_effect::key_pressed( const bear::input::key_info& key )
{
  // prevent pausing the game
  return true;
} // level_ending_effect::key_pressed()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform the effect that a key is maintained.
 * \param key The value of the pressed key.
 */
bool
rp::level_ending_effect::key_maintained( const bear::input::key_info& key )
{
  m_speed_factor = 10;

  return true;
} // level_ending_effect::key_maintained()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform the effect that a joystick button had been pressed.
 * \param button The value of the pressed button.
 * \param joy_index The index of the joystick.
 * \remark This effect pretends to treat all keys, so we can maintain the pause
 *         mode.
 */
bool rp::level_ending_effect::button_pressed
( bear::input::joystick::joy_code button, unsigned int joy_index )
{
  // prevent pausing the game
  return true;
} // level_ending_effect::button_pressed()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform the effect that a joystick button is maintained.
 * \param button The value of the pressed button.
 * \param joy_index The index of the joystick.
 */
bool rp::level_ending_effect::button_maintained
( bear::input::joystick::joy_code button, unsigned int joy_index )
{
  m_speed_factor = 10;

  return true;
} // level_ending_effect::button_maintained()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform the layer that the mouse had been moved.
 * \param pos The new position of the mouse.
 */
bool rp::level_ending_effect::mouse_move
( const claw::math::coordinate_2d<unsigned int>& pos )
{
  if ( m_button->get_rectangle().includes(pos) )
    {
      if ( ! m_active_component )
        get_level_globals().play_sound( "sound/tick.ogg" );
      m_active_component = true;
    }
  else
    m_active_component = false;
  
  return false;
} // level_ending_effect::mouse_move()

/*----------------------------------------------------------------------------*/
/**
 * \brief Inform the layer that the mouse had been moved.
 * \param pos The new position of the mouse.
 */
bool rp::level_ending_effect::mouse_released
( bear::input::mouse::mouse_code button,
  const claw::math::coordinate_2d<unsigned int>& pos )
{
  if ( m_button->get_rectangle().includes(pos) && 
       ! game_variables::is_boss_level() )
    {
      if ( m_finished )
        {
          if ( ! m_in_fade_out )
            {
              create_fade_out_tweener();
              m_in_fade_out = true;
            }
        }
      else
        skip();
    }

  return false;
} // level_ending_effect::mouse_released()

/*----------------------------------------------------------------------------*/
/**
 * \brief Give all the remaining points at once.
 */
void rp::level_ending_effect::skip()
{
   if ( ! m_finished )
     {
       // We update the lines 100 seconds at once. 
       // This is an arbitrary value but
       // using too large values may produce overflows.
       while ( !update_lines( 100 ) );

       if ( ! game_variables::is_boss_level() )
         update_medal();
     }
} // level_ending_effect::skip()

/*----------------------------------------------------------------------------*/
/**
 * \brief Fill m_points with the values of the bonus_points alive in the world.
 */
void rp::level_ending_effect::fill_points()
{
  if ( m_world == NULL )
    return;

  const bear::visual::font f
    ( get_level_globals().get_font("font/fontopo/fontopo.fnt", 50) );

  give_level_points(f);

  give_balloon_points(f);
  give_cart_elements_points(f);
  give_time_points(f);
  
  give_bad_balloon_points(f);
  give_bad_plunger_points(f);  
  give_bad_cannonball_points(f);
} // level_ending_effect::fill_points()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add a line for the score of the level. The game variable of the level
 *        score will be set to zero.
 * \param f The font to use.
 */
void rp::level_ending_effect::give_level_points( const bear::visual::font& f )
{
  const unsigned int points( game_variables::get_score() );
  game_variables::set_score( 0 );

  const std::string empty_string;

  m_positive_points.push_back
    ( score_line
      ( f, rp_gettext("Level score"), empty_string, empty_string, points,
        get_level_globals().auto_sprite
        ( "gfx/status/bonus.png", "level" ) ) );
} // level_ending_effect::give_level_points()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add balloon points.
 * \param f The font to use.
 */
void rp::level_ending_effect::give_balloon_points(const bear::visual::font& f)
{
  const unsigned int points( 500 * game_variables::get_balloons_number() );

  std::ostringstream stream;
  stream << game_variables::get_balloons_number();

  m_positive_points.push_back
    ( score_line
      ( f, rp_gettext("Balloons"), "500 x ", stream.str(), points,
        get_level_globals().auto_sprite
        ( "gfx/status/bonus.png", "balloons" ) ) );
} // level_ending_effect::give_balloon_points()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add bad_balloon points.
 * \param f The font to use.
 */
void rp::level_ending_effect::give_bad_balloon_points
(const bear::visual::font& f)
{
  const int points( -500 * game_variables::get_bad_balloon_number() );

  std::ostringstream stream;
  stream << game_variables::get_bad_balloon_number();
  
  m_negative_points.push_back
    ( score_line
      ( f, rp_gettext("Burst balloons"), "-500 x " ,  stream.str(), points,
        get_level_globals().auto_sprite
        ( "gfx/status/bonus.png", "explosed balloons" ) ) );

  m_negative_points.back().set_negative();
} // level_ending_effect::give_bad_balloon_points()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add bad_plunger points.
 * \param f The font to use.
 */
void rp::level_ending_effect::give_bad_plunger_points
(const bear::visual::font& f)
{
  const int points( -250 * game_variables::get_bad_plunger_number() );
  
  std::ostringstream stream;
  stream << game_variables::get_bad_plunger_number();

  m_negative_points.push_back
    ( score_line
      ( f, rp_gettext("Missed plungers"), "-250 x ", stream.str(), points,
        get_level_globals().auto_sprite
        ( "gfx/status/bonus.png", "missed plungers" ) ) );

  m_negative_points.back().set_negative();
} // level_ending_effect::give_bad_plunger_points()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add bad_cannonball points.
 * \param f The font to use.
 */
void rp::level_ending_effect::give_bad_cannonball_points
(const bear::visual::font& f)
{
  const int points( -250 * game_variables::get_bad_cannonball_number() );
  
  std::ostringstream stream;
  stream << game_variables::get_bad_cannonball_number();

  m_negative_points.push_back
    ( score_line
      ( f, rp_gettext("Missed cannonballs"), "-250 x ", stream.str(), points,
        get_level_globals().auto_sprite
        ( "gfx/status/bonus.png", "missed cannonballs" ) ) );

  m_negative_points.back().set_negative();
} // level_ending_effect::give_bad_cannonball_points()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add bad_cannonball points.
 * \param f The font to use.
 */
void rp::level_ending_effect::give_cart_elements_points
(const bear::visual::font& f)
{
  unsigned int elems = game_variables::get_cart_elements_number(1);
  unsigned int points( 0 );
  if ( elems == 3 )
    points = 9000;
  else if ( elems == 2 )
    points = 4000;

  m_positive_points.push_back
    ( score_line
      ( f, rp_gettext("Health bonus"), points,
        get_level_globals().auto_sprite( "gfx/status/bonus.png", "health" ) ) );
} // level_ending_effect::give_cart_elements_points()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add time points.
 * \param f The font to use.
 */
void rp::level_ending_effect::give_time_points
(const bear::visual::font& f)
{
  const bear::timer* timer = m_cart->get_level_timer();
  
  if ( timer != NULL )
    {
      unsigned int t = (unsigned int)( timer->get_time() );
      
      const unsigned int points( 500 * t );
      
      std::ostringstream stream;
      stream << t;
      
      m_positive_points.push_back
        ( score_line
          ( f, rp_gettext("Time bonus"), "500 x ", stream.str(), points,
            get_level_globals().auto_sprite
            ( "gfx/status/bonus.png", "time" ) ) );
    }
} // level_ending_effect::give_cart_elements_points()

/*----------------------------------------------------------------------------*/
/**
 * \brief Returns the top coordinate of the top active line (i.e. non persistent
 *        lines).
 */
bear::visual::coordinate_type rp::level_ending_effect::get_lines_top() const
{
  bear::visual::coordinate_type result = get_score_top();

  for ( std::list<score_line>::const_iterator it=m_persistent_points.begin();
        it!=m_persistent_points.end(); ++it )
    result -= it->get_height() + s_margin;

  return result;
} // level_ending_effect::get_lines_top()

/*----------------------------------------------------------------------------*/
/**
 * \brief Returns the top coordinate of the top line of score.
 */
bear::visual::coordinate_type rp::level_ending_effect::get_score_top() const
{
  const bear::universe::rectangle_type rect
    ( bear::universe::position_type(0, 0), get_layer().get_size() );

  const bear::universe::position_type level_name_position
    ( util::get_level_name_position( rect ) );

  return level_name_position.y - m_level_name.get_height() / 2 - 50;
} // level_ending_effect::get_score_top()

/*----------------------------------------------------------------------------*/
/**
 * \brief Sets the initial y-position of a list of lines.
 * \param lines The lines to place.
 */
void rp::level_ending_effect::initialize_line_position
( std::list<score_line>& lines ) const
{
  std::list<score_line>::iterator it;
  bear::visual::coordinate_type top( get_lines_top() );

  for ( it=lines.begin(); it!=lines.end(); ++it )
    {
      it->set_y_position( top - it->get_height() );
      top -= it->get_height() + s_margin;
    }
} // level_ending_effect::initialize_line_position()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update the position and the content of the lines.
 * \param elapsed_time Elapsed time since the last call.
 * \return true if there is no points to give.
 */
bool rp::level_ending_effect::update_lines
( bear::universe::time_type elapsed_time )
{
  if ( m_update_function != NULL )
    (this->*m_update_function)( elapsed_time );

  return m_update_function == NULL;
} // level_ending_effect::update_lines()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update the score with the positive lines.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::level_ending_effect::update_positive_lines
( bear::universe::time_type elapsed_time )
{
  if ( !update_lines( elapsed_time, m_positive_points ) )
    {
      m_play_tick = false;
      m_merge_delay = 3;
      m_update_function = &level_ending_effect::delay_positive_merge;
    }
} // level_ending_effect::update_positive_lines()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update the score with the negative lines.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::level_ending_effect::update_negative_lines
( bear::universe::time_type elapsed_time )
{
  if ( !update_lines( elapsed_time, m_negative_points ) )
    {
      m_play_tick = false;
      m_merge_delay = 3;
      m_update_function = &level_ending_effect::delay_negative_merge;
    }
} // level_ending_effect::update_negative_lines()

/*----------------------------------------------------------------------------*/
/**
 * \brief Updates the value of m_delay and merge the positive lines when its
 *        value reaches zero.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::level_ending_effect::delay_positive_merge
( bear::universe::time_type elapsed_time )
{
  m_merge_delay -= elapsed_time;

  if ( m_merge_delay <= 0 )
    {
      m_merge_delay = 0;
      m_update_function = &level_ending_effect::merge_positive_lines;
    }
} // level_ending_effect::delay_positive_merge()

/*----------------------------------------------------------------------------*/
/**
 * \brief Updates the value of m_delay and merge the negative lines when its
 *        value reaches zero.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::level_ending_effect::delay_negative_merge
( bear::universe::time_type elapsed_time )
{
  m_merge_delay -= elapsed_time;

  if ( m_merge_delay <= 0 )
    {
      m_merge_delay = 0;
      m_update_function = &level_ending_effect::merge_negative_lines;
    }
} // level_ending_effect::delay_negative_merge()

/*----------------------------------------------------------------------------*/
/**
 * \brief Merge the score with the positive lines.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::level_ending_effect::merge_positive_lines
( bear::universe::time_type elapsed_time )
{
  if ( !merge_lines( elapsed_time, m_positive_points ) )
    {
      m_flash_opacity = 1;

      create_persistent_line
        ( rp_gettext("Bonus"), m_positive_points, "bonus" );

      initialize_line_position( m_negative_points );

      m_update_function = &level_ending_effect::flash_positive_persistent;
    }
} // level_ending_effect::merge_positive_lines()

/*----------------------------------------------------------------------------*/
/**
 * \brief Merge the score with the negative lines.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::level_ending_effect::merge_negative_lines
( bear::universe::time_type elapsed_time )
{
  if ( !merge_lines( elapsed_time, m_negative_points ) )
    {
      m_flash_opacity = 1;

      create_persistent_line
        ( rp_gettext("Penalties"), m_negative_points, "penalty" );
      m_update_function = &level_ending_effect::flash_negative_persistent;
    }
} // level_ending_effect::merge_negative_lines()

/*----------------------------------------------------------------------------*/
/**
 * \brief Applies the change of the opacity on the flash rectangle. When the
 *        flash is over the update function is set to update_negative_lines.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::level_ending_effect::flash_positive_persistent
( bear::universe::time_type elapsed_time )
{
  m_flash_opacity =
    std::max( 0.0, m_flash_opacity - 1 * elapsed_time );

  if ( m_flash_opacity == 0 )
    {
      m_play_tick = true;
      m_update_function = &level_ending_effect::update_negative_lines;
    }
} // level_ending_effect::flash_positive_persistent()

/*----------------------------------------------------------------------------*/
/**
 * \brief Applies the change of the opacity on the flash rectangle. When the
 *        flash is over the update function is set to NULL.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::level_ending_effect::flash_negative_persistent
( bear::universe::time_type elapsed_time )
{
  m_flash_opacity =
    std::max( 0.0, m_flash_opacity - 1 * elapsed_time );

  if ( m_flash_opacity == 0 )
    m_update_function = NULL;
} // level_ending_effect::flash_negative_persistent()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update the score with the points of the given lines.
 * \param elapsed_time Elapsed time since the last call.
 * \param lines The lines to use to update the score.
 * \return true if there is no points to give.
 */
bool rp::level_ending_effect::update_lines
( bear::universe::time_type elapsed_time, std::list<score_line>& lines ) const
{
  std::list<score_line>::iterator it;

  // The loop stops on the first line that does change the score.
  bool stop(false);

  for (it=lines.begin(); !stop && (it!=lines.end()); ++it)
    {
      const int s =
        it->update_score( s_points_per_second * elapsed_time );

      stop = (s != 0);

      game_variables::add_score(1, s, false);
    }

  return stop;
} // level_ending_effect::update_lines()

/*----------------------------------------------------------------------------*/
/**
 * \brief Merge the visible lines of score.
 * \param elapsed_time Elapsed time since the last call.
 * \param lines The lines to move.
 * \return true if the lines are merged.
 */
bool rp::level_ending_effect::merge_lines
( bear::universe::time_type elapsed_time, std::list<score_line>& lines ) const
{
  std::list<score_line>::iterator it;
  bear::visual::coordinate_type top( get_lines_top() );

  bool updated(false);

  double factor(1);

  for (it=lines.begin(); it!=lines.end(); ++it)
    {
      if ( it->get_y_position() + it->get_height() < top )
        {
          it->set_y_position
            ( std::min
              ( top - it->get_height(),
                it->get_y_position()
                + s_score_line_speed * elapsed_time * factor ) );

          updated = true;
        }
    }

  return updated;
} // level_ending_effect::merge_lines()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates a line of score that will not disappear.
 * \param label The label to display on the line.
 * \param lines The lines from which the score is computed.
 * \param icon_name The name of the icon in gfx/status/bonus.png.
 */
void rp::level_ending_effect::create_persistent_line
( std::string label, std::list<score_line>& lines, std::string icon_name )
{
  if ( lines.empty() )
    return;

  std::list<score_line>::iterator it;
  int points(0);

  for (it=lines.begin(); it!=lines.end(); ++it)
    points += it->get_total_points();

  score_line result
    ( get_level_globals().get_font("font/fontopo/fontopo.fnt", 50),
      label, points,
      get_level_globals().auto_sprite( "gfx/status/bonus.png", icon_name ) );

  result.set_y_position( lines.front().get_y_position() );
  
  if ( lines.front().is_negative() )
    result.set_negative();

  result.update_score( std::abs( result.get_total_points() ) );

  m_persistent_points.push_back( result );

  lines.clear();
} // level_ending_effect::create_persistent_line()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update the date of the tick and play the sound if needed.
 * \param elapsed_time Elapsed time since the last call.
 */
void rp::level_ending_effect::update_tick
( bear::universe::time_type elapsed_time )
{
  if ( !m_play_tick )
    return;

  if ( m_next_tick > elapsed_time )
    m_next_tick -= elapsed_time;
  else
    {
      get_level_globals().play_sound("sound/tick.ogg");

      if ( m_speed_factor > 1 )
        m_next_tick = 0.05;
      else
        m_next_tick = 0.1;
    }
} // level_ending_effect::update_tick()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update medal state.
 */
void rp::level_ending_effect::update_medal()
{
  unsigned int medal = get_medal();
  if ( medal != m_medal )
    {
      bool sad = m_medal > medal;

      m_medal = medal;
      std::string medal_name;

      if ( medal == 1 )
        medal_name = "bronze";
      else if ( medal == 2 )
        medal_name = "silver";
      else if ( medal == 3 )
        {
          medal_name = "gold";
          m_applause_sample->play();
        }

      if ( medal_name.empty() )
        m_medal_sprite = bear::visual::sprite();
      else
        m_medal_sprite = get_level_globals().auto_sprite
          ( "gfx/status/medal/medal.png", medal_name );
          
      create_decorative_medal();

      if ( sad )
        {
          m_applause_sample->stop();
          get_level_globals().play_sound( "sound/cart/andy/sad.ogg" );
        }
      else
        get_level_globals().play_sound( "sound/medal/" + medal_name + ".ogg" );
    }
} // level_ending_effect::update_medal()

/*----------------------------------------------------------------------------*/
/**
 * \brief Update the unlocked serial game variable.
 */
void rp::level_ending_effect::update_unlocked_serial() const
{
  if ( game_variables::is_boss_level() )
    game_variables::set_unlocked_serial
      ( std::max
        ( game_variables::get_unlocked_serial(),
          game_variables::get_serial_number() + 1 ) );
} // level_ending_effect::update_unlocked_serial()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get medal state.
 */
void rp::level_ending_effect::create_decorative_medal()
{ 
  if ( m_decorative_medal != NULL )
    m_decorative_medal->clear_forced_movement();

  m_decorative_medal = new bear::decorative_item;

  m_decorative_medal->set_sprite( m_medal_sprite );
  m_decorative_medal->set_size( m_medal_sprite.get_size() );
  m_decorative_medal->set_global(true);
  m_decorative_medal->set_z_position( 1000000 );

  bear::universe::position_type pos =
    util::get_medal_position( get_level().get_camera_focus() );
  bear::universe::coordinate_type dist = 
    get_level().get_camera_focus().size().y;
  m_decorative_medal->set_center_of_mass
    ( pos + bear::universe::position_type( 0, dist ) );

  m_cart->new_item( *m_decorative_medal );
  
  bear::universe::forced_goto mvt; 
  mvt.set_length(bear::universe::vector_type(0,-dist));
  mvt.set_total_time(0.2);
  mvt.set_acceleration_time(0);
  m_decorative_medal->set_forced_movement( mvt );
} // level_ending_effect::create_decorative_medal()

/*----------------------------------------------------------------------------*/
/**
 * \brief Get medal state.
 */
unsigned int rp::level_ending_effect::get_medal() const
{
  unsigned int score = game_variables::get_score();
      
  if ( score >= game_variables::get_gold_threshold() )
    return 3;
  else if ( score >= game_variables::get_silver_threshold() )
    return 2;
  else if ( score >= game_variables::get_bronze_threshold() )
    return 1;
  else
    return 0;
} // level_ending_effect::get_medal()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the level name.
 * \param e (out) The scene elements.
 */
void rp::level_ending_effect::render_score( scene_element_list& e) const
{
  render_score_lines( e, m_persistent_points );

  if ( !m_positive_points.empty() )
    render_score_lines( e, m_positive_points );
  else if ( !m_negative_points.empty() && (m_flash_opacity == 0) )
    render_score_lines( e, m_negative_points );
  
  const bear::universe::rectangle_type rect
    ( bear::universe::position_type(0, 0), get_layer().get_size() );
  const bear::universe::position_type pos = util::get_medal_position( rect );

  double factor = 1;
  if ( m_points_text.get_width() > get_layer().get_size().x / 2 )
    factor = get_layer().get_size().x / 2.0 / m_points_text.get_width();

  bear::visual::scene_writing points
    ( ( get_layer().get_size().x - factor * m_points_text.get_width() ) / 2, 
      pos.y - factor * m_points_text.get_height() / 2, m_points_text );
  
  points.set_shadow( 5, -5 );
  points.set_scale_factor(factor, factor);

  const bear::visual::scene_sprite sp
    ( points.get_position().x + points.get_bounding_box().width()
      - m_new_record_sprite.width() * 0.5,
      points.get_position().y
      + points.get_bounding_box().height()
      - m_new_record_sprite.height() * 0.25,
      m_new_record_sprite );

  e.push_back( sp );
  e.push_back( points );

  bear::visual::scene_writing world_record
    ( points.get_position().x + m_points_text.get_width() * factor
      - m_world_record.get_width(),
      points.get_position().y - m_world_record.get_height() - 5,
      m_world_record );
  world_record.set_shadow( 3, -3 );

  e.push_back( world_record );
} // level_ending_effect::render_score()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render some lines of score.
 * \param e (out) The scene elements.
 * \param lines The lines to render.
 */
void rp::level_ending_effect::render_score_lines
( scene_element_list& e, const std::list<score_line>& lines ) const
{
  bear::visual::coordinate_type left( s_screen_margin );
  bear::visual::coordinate_type right( get_layer().get_size().x -  left );

  std::list<score_line>::const_iterator it;

  for ( it=lines.begin(); it!=lines.end(); ++it )
    it->render( e, left, right );
} // level_ending_effect::render_score_lines()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the background of the scores.
 * \param e (out) The scene elements.
 */
void
rp::level_ending_effect::render_score_background( scene_element_list& e) const
{
  const bear::universe::rectangle_type rect
    ( bear::universe::position_type(0, 0), get_layer().get_size() );
  const bear::universe::position_type pos( util::get_medal_position( rect ) );

  const bear::universe::coordinate_type margin( 30 );

  const bear::universe::coordinate_type bottom
    ( pos.y + m_gold_medal_sprite.height() / 2 + margin );

  const bear::universe::coordinate_type top( get_score_top() + margin );

  const bear::visual::rectangle_type background_fill
    ( 0, bottom, get_layer().get_size().x, top );

  e.push_back
    ( bear::visual::scene_rectangle
      ( 0, 0, bear::visual::color_type("#3B000000"), background_fill, true,
        0 ) );
} // level_ending_effect::render_score_background()

/*----------------------------------------------------------------------------*/
/**
 * \brief Renders the flash rectangle over the last persistent score.
 * \param e (out) The scene elements.
 */
void rp::level_ending_effect::render_flash_line( scene_element_list& e ) const
{
  if ( m_persistent_points.empty() || (m_flash_opacity == 0) )
    return;

  const bear::visual::size_type margin( 5 );
  const bear::visual::coordinate_type bottom
    ( m_persistent_points.back().get_y_position() - margin );
  const bear::visual::coordinate_type height
    ( m_persistent_points.back().get_height() + 2 * margin );
  
  const bear::visual::rectangle_type flash_rectangle
    ( 0, bottom, get_layer().get_size().x, bottom + height );

  bear::visual::color_type c("#ffffff");
  c.set_opacity( m_flash_opacity );

  e.push_back
    ( bear::visual::scene_rectangle( 0, 0, c, flash_rectangle, true, 0 ) );
} // level_ending_effect::render_flash_line()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the level name.
 * \param e (out) The scene elements.
 */
void rp::level_ending_effect::render_level_name( scene_element_list& e) const
{
  bear::universe::rectangle_type rect
    ( bear::universe::position_type(0,0), get_layer().get_size() );

  bear::universe::position_type pos( util::get_level_name_position( rect ) );
  
  bear::visual::scene_writing result
    ( pos.x - m_level_name.get_width() / 2, 
      pos.y - m_level_name.get_height() / 2, 
      m_level_name );

  result.set_shadow( RP_LEVEL_NAME_SHADOW, -RP_LEVEL_NAME_SHADOW );
  result.set_shadow_opacity( 1 );

  e.push_back( result );
} // level_ending_effect::render_level_name()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the opaque rectangle.
 * \param e (out) The scene elements.
 */
void rp::level_ending_effect::render_opaque_rectangle
( scene_element_list& e) const
{
  bear::universe::rectangle_type rect
    ( bear::universe::position_type(0,0),
      bear::universe::position_type
      (get_layer().get_size().x, get_layer().get_size().y) );

  bear::visual::scene_element scene
    ( bear::visual::scene_rectangle
      ( 0, 0,
        claw::graphic::black_pixel, rect, true) );
  scene.get_rendering_attributes().set_opacity(m_rectangle_opacity);

  e.push_back( scene );
} // level_ending_effect::render_opaque_rectange()

/*----------------------------------------------------------------------------*/
/**
 * \brief Render the medal.
 * \param e (out) The scene elements.
 */
void rp::level_ending_effect::render_medal( scene_element_list& e) const
{
  if ( m_finished && m_medal > 0 )
    {
      bear::universe::rectangle_type rect
        ( bear::universe::position_type(0,0), get_layer().get_size() );
      bear::universe::position_type pos = util::get_medal_position( rect );
      
      bear::visual::scene_sprite sp
        ( pos.x - m_medal_sprite.width() / 2, 
          pos.y - m_medal_sprite.height() / 2, 
          m_medal_sprite );
      e.push_back( sp );
    }  
} // level_ending_effect::render_medal()

/*----------------------------------------------------------------------------*/
/**
 * \brief Add background sprites.
 * \param visuals (out) The sprites of the item, and their positions.
 */
void rp::level_ending_effect::render_background( scene_element_list& e ) const
{
  if ( m_button->get_visible() )
    {
      if ( m_active_component )
        {
          bear::visual::scene_sprite s
            ( m_root_window.right() - 100 - m_background_on_sprite.width(), 
              80, m_background_on_sprite);
          e.push_back( s );
        }
      else
        {
          bear::visual::scene_sprite s
            ( m_root_window.right() - 100 - m_background_on_sprite.width(),
              80, m_background_off_sprite);
          e.push_back( s );
        }
    }
} // level_ending_effect::render_background()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create tweener for fade out opacity.
 */
void rp::level_ending_effect::create_fade_out_tweener()
{
  m_tweener_fade_out = 
    claw::tween::single_tweener
    ( m_rectangle_opacity, 1.0, 2.0, 
      &claw::tween::easing_linear::ease_in_out );

  m_tweener_fade_out.on_finished
    (boost::bind( &rp::level_ending_effect::pop_level, this ));
} // level_ending_effect::create_fade_out_tweener()

/*----------------------------------------------------------------------------*/
/**
 * \brief Pop the level.
 */
void rp::level_ending_effect::pop_level()
{
  if ( ! m_pop_level )
    {
      m_pop_level = true;
      game_variables::set_ending_effect(false);

      util::send_complete_level( get_level().get_filename() );

      if ( ! game_variables::is_boss_level() )
        bear::engine::game::get_instance().pop_level();
      else
        {
          std::ostringstream stream;
          stream << "level/" << game_variables::get_serial_number() 
                 << "/level-8-transition.cl";
          bear::engine::game::get_instance().set_waiting_level( stream.str() );
        }
    }
} // level_ending_effect::pop_level()

/*----------------------------------------------------------------------------*/
/**
 * \brief Creates the button component.
 */
void rp::level_ending_effect::add_button_component()
{
  m_button =
    new bear::gui::button
    ( get_level_globals().auto_sprite( "gfx/status/buttons.png", "skip" ) );

  m_button->set_right( m_root_window.right() - 100 );
  m_button->set_bottom( 80 );

  m_root_window.insert( m_button );

  if ( game_variables::is_boss_level() )
    m_button->set_visible(false);
} // level_ending_effect::add_button_component()

/*----------------------------------------------------------------------------*/
/**
 * \brief Retrieve the best score of the level from our stats server.
 */
void rp::level_ending_effect::get_best_score()
{
  m_score_request =
    score_request
    ( boost::bind( &level_ending_effect::set_best_score, this, _1 ),
      get_level().get_filename() );

  boost::thread t( m_score_request );
} // level_ending_effect::get_best_score()

/*----------------------------------------------------------------------------*/
/**
 * \brief Sets the best score of the current level.
 * \param score The best score.
 */
void rp::level_ending_effect::set_best_score( unsigned int score )
{
  std::ostringstream oss;
  oss << rp_gettext("World record: ") << score;

  m_world_record.create
    ( get_level_globals().get_font("font/fontopo/fontopo-small.fnt", 20),
      oss.str() );
} // level_ending_effect::get_best_score()
