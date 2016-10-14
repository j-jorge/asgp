/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
/**
 * \file
 * \brief Implementation of the rp::launcher class.
 * \author Julien Jorge
 */
#include "launcher.hpp"

#include "bear_gettext.hpp"

#include <claw/logger.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <libintl.h>

#define STRINGIZE_HELPER(a) # a

#define STRINGIZE(a) STRINGIZE_HELPER(a)

/*----------------------------------------------------------------------------*/
/**
 * \brief Constructor.
 * \param argc Number of program arguments.
 * \param argv Program arguments.
 */
rp::launcher::launcher( int& argc, char** &argv )
  : claw::application(argc, argv), m_game(NULL)
{
#ifdef BEAR_TEXT_DOMAIN_PATH
  bindtextdomain( "bear-engine", BOOST_PP_STRINGIZE(BEAR_TEXT_DOMAIN_PATH) );
#endif

  bind_textdomain_codeset( "bear-engine", "ISO-8859-15" );
  textdomain( "bear-engine" );

  m_arguments.add
    ("-h", "--help", bear_gettext("Print this help screen and exit."), true);
  m_arguments.parse(argc, argv);

  if ( m_arguments.get_bool("--help") )
    help();
  else
    create_game( argc, argv );
} // running_rp::launcher()

/*----------------------------------------------------------------------------*/
/**
 * \brief Destructor.
 */
rp::launcher::~launcher()
{
  delete m_game;
} // running_bear::~running_bear()

/*----------------------------------------------------------------------------*/
/**
 * \brief Run the application.
 */
int rp::launcher::run()
{
  std::string text_domain_dir;

#ifdef _WIN32

  text_domain_dir = get_application_path() + "..\\share\\locale";

#elif defined RP_STATIC

  text_domain_dir = get_application_path() + "share/locale";

#elif defined RP_TEXT_DOMAIN_PATH
  
  text_domain_dir = BOOST_PP_STRINGIZE(RP_TEXT_DOMAIN_PATH);
#endif

  if ( text_domain_dir.empty() )
    claw::logger << claw::log_verbose << "Using default text domain directory."
                 << std::endl;
  else if ( bindtextdomain
            ( "straining-coasters", text_domain_dir.c_str() ) != NULL )
    claw::logger << claw::log_verbose << "Text domain directory is '"
                 << text_domain_dir << "'." << std::endl;
  else
    claw::logger << claw::log_error
                 << "Could not set the text domain directory to '"
                 << text_domain_dir << "'." << std::endl;

  bind_textdomain_codeset( "straining-coasters", "ISO-8859-15" );

  try
    {
      if ( m_game != NULL )
        {
          m_game->run();
          delete m_game;
          m_game = NULL;
        }

      return EXIT_SUCCESS;
    }
  catch( std::exception& e )
    {
      claw::logger << claw::log_error << "(exception) " << e.what()
                   << std::endl;
      delete m_game;
      m_game = NULL;
      return EXIT_FAILURE;
    }
} // running_bear::run()

/*----------------------------------------------------------------------------*/
/**
 * \brief Create the game.
 * \param argc Number of program arguments.
 * \param argv Program arguments.
 */
void rp::launcher::create_game( int& argc, char** &argv )
{
  const std::string data_path_argument( get_data_path_argument() );
  const std::string generic_item_argument( get_generic_items_argument() );
  const std::string rp_argument( get_rp_argument() );

  const char* default_args[] =
    {
      "--game-name=Straining Coasters",
      "--screen-width=1280",
      "--screen-height=720",
      "--active-area=250",

#if defined RP_TAG
      "--tag=" STRINGIZE( RP_TAG ),
#endif

#if !defined(NDEBUG)
      "--data-path=data",
#endif

      data_path_argument.c_str(),

#ifdef RP_STATIC
      "--auto-load-symbols",
#else
      generic_item_argument.c_str(),
      rp_argument.c_str(),
#endif

      "--start-level=level/start.cl",
      NULL
    };

  // Compute the number of elements in default_args
  int default_argc(0);
  while ( default_args[ default_argc ] != NULL )
    ++default_argc;

  int final_argc( argc + default_argc );
  const char** final_args = new const char*[ final_argc ];

  for ( int i=0; i!=default_argc; ++i )
    final_args[i] = default_args[i];

  for ( int i=0; i!=argc; ++i )
    final_args[ default_argc + i ] = argv[i];

  try
    {
      char** engine_args = const_cast<char**>(final_args);
      m_game = new bear::engine::game( final_argc, engine_args );
    }
  catch( std::exception& e )
    {
      std::cerr << "Exception: " << e.what() << std::endl;
      help();
    }

  delete[] final_args;
} // running_bear::create_game()

/*----------------------------------------------------------------------------*/
/**
 * \brief Print some help about the usage of the program.
 */
void rp::launcher::help() const
{
  m_arguments.help( bear_gettext("engine_options") );
  std::cout << '\n';
  bear::engine::game::print_help();
} // running_bear::help()

/*----------------------------------------------------------------------------*/
/**
 * \brief Returns the path of the program.
 */
std::string rp::launcher::get_application_path() const
{
  const std::string p( m_arguments.get_program_name() );
  std::string::size_type pos;

#ifdef _WIN32
  pos = p.find_last_of("\\'");
#else
  pos = p.find_last_of('/');
#endif

  if ( pos == std::string::npos )
    return "";
  else
    return p.substr( 0, pos + 1 );
} // launcher::get_application_path()

/*----------------------------------------------------------------------------*/
/**
 * \brief Returns the argument that gives the path of the data directory.
 */
std::string rp::launcher::get_data_path_argument() const
{
  std::string a( "--data-path=" );

#ifdef _WIN32
  return a + get_application_path() + STRINGIZE( RP_DATA_DIRECTORY );
#elif defined RP_STATIC
  return a + get_application_path() + "share/straining-coasters";
#elif !defined(NDEBUG)
  return a + STRINGIZE( RP_DATA_DEBUG_DIRECTORY );
#else
  return a + STRINGIZE( RP_DATA_DIRECTORY );
#endif
} // launcher::get_data_path_argument()

/*----------------------------------------------------------------------------*/
/**
 * \brief Returns the argument that gives the path of the generic items library.
 */
std::string rp::launcher::get_generic_items_argument() const
{
  std::string a( "--item-library=" );

#ifdef _WIN32
  return a + get_application_path() + STRINGIZE(GENERIC_ITEMS_LIBRARY_PATH);
#elif !defined(NDEBUG)
  return a + STRINGIZE(GENERIC_ITEMS_LIBRARY_DEBUG_PATH);
#else
  return a + STRINGIZE(GENERIC_ITEMS_LIBRARY_PATH);
#endif
} // launcher::get_generic_items_argument()

/*----------------------------------------------------------------------------*/
/**
 * \brief Returns the argument that gives the path of the game's library.
 */
std::string rp::launcher::get_rp_argument() const
{
  std::string a( "--item-library=" );

#ifdef _WIN32
  return a + get_application_path() + STRINGIZE(RP_LIBRARY_PATH);
#elif !defined(NDEBUG)
  return a + STRINGIZE(RP_LIBRARY_DEBUG_PATH);
#else
  return a + STRINGIZE(RP_LIBRARY_PATH);
#endif
} // launcher::get_rp_argument()
