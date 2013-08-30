package com.stuffomatic.asgp;

import org.libsdl.app.SDLActivity;

public class ASGP extends SDLActivity
{
    static {
        System.loadLibrary( "gnustl_shared" );
        System.loadLibrary( "andy-super-great-park" );
    }
}
