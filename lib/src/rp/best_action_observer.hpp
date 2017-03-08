#pragma once

#include "engine/base_item.hpp"
#include "engine/export.hpp"
#include "visual/capture.hpp"

#include <claw/image.hpp>

#include <boost/signals2/connection.hpp>
#include <boost/thread/mutex.hpp>

namespace rp
{
  class best_action_observer:
    public bear::engine::base_item
  {
    DECLARE_BASE_ITEM( best_action_observer );

  private:
    typedef bear::engine::base_item super;

  public:
    best_action_observer();
    best_action_observer( const best_action_observer& that );
    ~best_action_observer();
    
    void build() override;
    void progress( bear::universe::time_type elapsed_time ) override;

  private:
    void scan();
    void render_capture();
    void store_capture( const claw::graphic::image& image );
    void saved
    ( const std::string& save_path, const claw::graphic::image& image );
    void add_image_resource
    ( const std::string& save_path, const bear::visual::sprite& sprite );
    void dispatch_progress( double p );
    
  private:
    double m_best_score;
    bear::visual::capture m_best_scene;
    boost::function< void() > m_progress;
    boost::mutex m_mutex;

    boost::signals2::connection m_screenshot_connection;
    boost::signals2::connection m_snapshot_connection;
    boost::signals2::connection m_finalize_connection;
    boost::signals2::connection m_saved_connection;
  };
}
