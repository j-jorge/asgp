package com.stuffomatic.asgp;

import org.libsdl.app.SDLActivity;
import android.os.Bundle;

public class ASGP extends SDLActivity
{
    static {
        System.loadLibrary( "gnustl_shared" );
        System.loadLibrary( "andy-super-great-park" );
    }
    
    public native void init();


    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        init();
    }
}
