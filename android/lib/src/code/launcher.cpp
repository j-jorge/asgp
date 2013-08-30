#include "launcher.hpp"

#include "engine/game.hpp"

/** \brief The game we are running. */
bear::engine::game* g_game(NULL);

JNIEXPORT void JNICALL
Java_com_stuffomatic_asgp_ASGP_init( JNIEnv* env )
{
  const char* default_args[] =
    {
      "--game-name=Super Great Park",
      "--screen-width=1280",
      "--screen-height=720",
      "--active-area=250",
      "--tag=android",
      "--data-path=assets/",
      "--auto-load-symbols",
      "--stats-destination=http://www.stuff-o-matic.com/asgp/stats/save.php",
      "--start-level=level/start.cl",
      NULL
    };

  // Compute the number of elements in default_args
  int default_argc(0);
  while ( default_args[ default_argc ] != NULL )
    ++default_argc;

  try
    {
      char** engine_args = const_cast<char**>(default_args);
      g_game = new bear::engine::game( default_argc, engine_args );
    }
  catch( std::exception& e )
    {
      std::cerr << "Exception: " << e.what() << std::endl;
    }
} // Java_com_stuffomatic_asgp_ASGP_init()
