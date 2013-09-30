package com.stuffomatic.asgp;

import org.libsdl.app.SDLActivity;
import android.view.WindowManager;
import android.os.Bundle;

public class ASGP extends SDLActivity
{
    static {
        System.loadLibrary( "gnustl_shared" );
        System.loadLibrary( "andy-super-great-park" );
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }
}
