if( RP_NO_MANPAGES )
  message("Ignoring manpages, on user's demand.")
else( RP_NO_MANPAGES )
  if(DOCBOOK_TO_MAN_FOUND)
    docbook_create_man( asgp-man 6 andy-super-great-park.sgml )
  else(DOCBOOK_TO_MAN_FOUND)
    message("docbook-to-man was not found on your system. The manpages of "
      "Andy's Super Great Park are not generated.")
  endif(DOCBOOK_TO_MAN_FOUND)
endif( RP_NO_MANPAGES )

