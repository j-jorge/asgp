#include "rp/best_action_observer.hpp"

#include "rp/balloon.hpp"
#include "rp/bird.hpp"
#include "rp/bomb.hpp"
#include "rp/cable.hpp"
#include "rp/cannonball.hpp"
#include "rp/cart.hpp"
#include "rp/crate.hpp"
#include "rp/explosion.hpp"
#include "rp/game_variables.hpp"
#include "rp/plunger.hpp"
#include "rp/switching.hpp"
#include "rp/tar.hpp"
#include "rp/tnt.hpp"
#include "rp/wall.hpp"
#include "rp/zeppelin.hpp"
#include "rp/message/level_capture_ready_message.hpp"
#include "rp/transition_effect/level_ending_effect_default_name.hpp"

#include "engine/export.hpp"
#include "engine/game.hpp"
#include "engine/system/game_filesystem.hpp"
#include "universe/world.hpp"

#include <claw/png.hpp>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread.hpp>

#include <memory>
#include <unordered_map>

BASE_ITEM_EXPORT( best_action_observer, rp )

namespace rp
{
  namespace detail
  {
    struct writer
    {
      std::string file_name;
      claw::graphic::image image;
      boost::signals2::signal
      <
        void( const std::string& save_path, const claw::graphic::image& image )
        > signal;
    };

    class bucket_base
    {
    public:
      virtual void insert( bear::universe::physical_item* item ) = 0;
    };
      
    template< typename T >
    struct bucket:
      public bucket_base
    {
      void insert( bear::universe::physical_item* item ) override
      {
        T* casted( dynamic_cast< T* >( item ) );
        assert( casted != nullptr );
        items.push_back( casted );
      }
      
      std::vector< T* > items;
    };
    
    struct scan_result
    {
      bucket< balloon > balloons;
      bucket< bird > birds;
      bucket< bomb > bombs;
      bucket< cable > cables;
      bucket< cannonball > cannonballs;
      bucket< cart > carts;
      bucket< crate > crates;
      bucket< explosion > explosions;
      bucket< plunger > plungers;
      bucket< switching > switchings;
      bucket< tar > tars;
      bucket< tnt > tnts;
      bucket< wall > walls;
      bucket< zeppelin > zeppelins;
    };

    static void fill_buckets
    ( scan_result& result, const bear::universe::world::item_list& items );

    static unsigned int get_score( const scan_result& items );
  }
}
  
rp::best_action_observer::best_action_observer()
: m_best_score( 0 )
{
  set_global( true );
  set_phantom( true );
  set_weak_collisions( true );
  set_can_move_items( false );
}

rp::best_action_observer::best_action_observer( const best_action_observer& )
{
  assert( false );
}

rp::best_action_observer::~best_action_observer()
{
  m_snapshot_connection.disconnect();
  m_finalize_connection.disconnect();
  m_saved_connection.disconnect();
  m_screenshot_connection.disconnect();
}

void rp::best_action_observer::build()
{
  const auto prepare_scan
    ( [ this ]() -> void
      {
        m_progress = boost::bind( &best_action_observer::scan, this );
      } );
  
  m_snapshot_connection =
    bear::engine::game::get_instance().listen_bool_variable_change
    ( game_variables::get_action_snapshot_variable_name(),
      boost::bind< void >( prepare_scan ) );

  const auto prepare_save
    ( [ this ]( bool save ) -> void
      {
        m_snapshot_connection.disconnect();
        m_progress = boost::bind( &best_action_observer::render_capture, this );
      } );
  
  m_finalize_connection =
    bear::engine::game::get_instance().listen_bool_variable_change
    ( game_variables::get_ending_effect_variable_name(),
      boost::bind< void >( prepare_save, _1 ) );
}

void
rp::best_action_observer::progress( bear::universe::time_type elapsed_time )
{
  super::progress( elapsed_time );

  boost::function< void() > f;

  {
    const boost::unique_lock< boost::mutex > lock( m_mutex );
    f = std::move( m_progress );
    assert( !m_progress );
  }
  
  if ( f )
    f();
}

void rp::best_action_observer::scan()
{
  bear::universe::world::item_list items;
  get_owner().pick_items_in_rectangle
    ( items, get_level().get_camera_focus() );

  detail::scan_result result;
  detail::fill_buckets( result, items );

  const double score( detail::get_score( result ) );

  if ( score <= m_best_score )
    return;

  m_best_score = score;
  m_best_scene = bear::engine::game::get_instance().screen_capture();
}

void rp::best_action_observer::render_capture()
{
  if ( m_best_score == 0 )
    return;
  
  assert( !m_screenshot_connection.connected() );
  m_screenshot_connection =
    m_best_scene.render
    ( boost::bind( &best_action_observer::store_capture, this, _1 ) );
}

void
rp::best_action_observer::store_capture( const claw::graphic::image& image )
{
  assert( !m_snapshot_connection.connected() );
  m_finalize_connection.disconnect();

  const boost::filesystem::path path
    ( bear::engine::game::get_instance().get_game_filesystem()
      .get_custom_data_file_name( "/files/capture.png" ) );
  
  boost::system::error_code error;
  boost::filesystem::create_directories( path.parent_path(), error );
    
  std::shared_ptr< detail::writer > w( new detail::writer() );

  w->file_name = path.string();
  w->image = image;
  
  m_saved_connection =
    w->signal.connect
    ( boost::bind( &best_action_observer::saved, this, _1, _2 ) );

  auto save
    ( [ this, w ]() -> void
      {
        std::ofstream f( w->file_name );
        claw::graphic::png::writer( w->image, f );
        w->signal( w->file_name, w->image );
      } );

  boost::thread( boost::bind< void >( save ) );
}

void rp::best_action_observer::saved
( const std::string& save_path, const claw::graphic::image& image )
{
  bear::visual::sprite sprite
    ( bear::visual::image( image ),
      bear::visual::sprite::clip_rectangle_type
      ( 0, 0, image.width(), image.height() ) );
  const boost::unique_lock< boost::mutex > lock( m_mutex );

  m_progress =
    boost::bind
    ( &best_action_observer::add_image_resource, this, save_path, sprite );
}

void rp::best_action_observer::add_image_resource
( const std::string& save_path, const bear::visual::sprite& sprite )
{
  get_level_globals().add_image( save_path, sprite.get_image() );

  level_capture_ready_message msg( save_path, sprite );
  get_level_globals().send_message
    ( get_level_ending_effect_default_name(), msg );
}

void rp::detail::fill_buckets
( scan_result& result, const bear::universe::world::item_list& items )
{
  std::unordered_map< std::string, detail::bucket_base* > buckets;
  
  buckets[ balloon::static_class_name() ] = &result.balloons;
  buckets[ bird::static_class_name() ] = &result.birds;
  buckets[ bomb::static_class_name() ] = &result.bombs;
  buckets[ cable::static_class_name() ] = &result.cables;
  buckets[ cannonball::static_class_name() ] = &result.cannonballs;
  buckets[ cart::static_class_name() ] = &result.carts;
  buckets[ crate::static_class_name() ] = &result.crates;
  buckets[ explosion::static_class_name() ] = &result.explosions;
  buckets[ plunger::static_class_name() ] = &result.plungers;
  buckets[ switching::static_class_name() ] = &result.walls;
  buckets[ tar::static_class_name() ] = &result.tars;
  buckets[ tnt::static_class_name() ] = &result.tnts;
  buckets[ wall::static_class_name() ] = &result.walls;
  buckets[ zeppelin::static_class_name() ] = &result.zeppelins;

  const auto end( buckets.end() );
  
  for ( bear::universe::physical_item* item : items )
    {
      const std::string class_name
        ( dynamic_cast< bear::engine::base_item* >( item )->get_class_name() );

      const auto it( buckets.find( class_name ) );

      if ( it != end )
        it->second->insert( item );
    }
}

unsigned int rp::detail::get_score( const scan_result& items )
{
  static constexpr unsigned int combo_bonus( 10 );
  
  unsigned int result( 0 );

  for ( const auto item : items.balloons.items )
    result += 10 + 10 * item->has_forced_movement();

  for ( const auto item : items.birds.items )
    {
      const bool afraid( item->is_afraid() );
      const bool dying( item->is_dying() );

      result +=
        10
        + 20 * afraid
        + 10 * item->is_flying()
        + 10 * dying
        + combo_bonus * item->get_combo_value() * ( afraid || dying );
    }

  for ( const auto item : items.bombs.items )
    result += 10 + 10 * item->has_bottom_contact();

  for ( const auto item : items.cables.items )
    result += 10 + 10 * item->is_ejected();
  
  result += 20 * items.cannonballs.items.size();
  
  for ( const auto item : items.carts.items )
    result +=
      20 * item->is_jumping()
      + 30 * item->is_dying()
      + 10 * item->is_speeding()
      + 30 * item->is_covered_with_tar()
      + item->attached_balloon_count();

  result += 10 * items.crates.items.size();

  if ( !items.explosions.items.empty() )
    result += 30;
  
  result += 20 * items.plungers.items.size();
  result += 10 * items.switchings.items.size();
  result += 10 * items.tars.items.size();
  
  for ( const auto item : items.walls.items )
    result += 10 + 10 * item->impact_count() / 3;
  
  for ( const auto item : items.zeppelins.items )
    result += 10 + 10 * ( item->get_drop_item() != nullptr );

  result += 10 * items.tnts.items.size();
    
  return result;
}

