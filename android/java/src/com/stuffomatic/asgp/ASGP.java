/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
package com.stuffomatic.asgp;

import org.libsdl.app.SDLActivity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;

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
        getWindow().getDecorView().setSystemUiVisibility
            ( View.SYSTEM_UI_FLAG_LOW_PROFILE );
    }

    /**
     * Tells the system to open a given URL.
     * @param url The URL to open.
     */
    public void openUrl( String url ) {

        final Intent browserIntent =
            new Intent( Intent.ACTION_VIEW, Uri.parse(url) );
        startActivity(browserIntent);
    } // openUrl()

    /**
     * Tells the system to show the home screen.
     */
    public void showHome() {

        final Intent homeIntent = new Intent( Intent.ACTION_MAIN );
        homeIntent.addCategory( Intent.CATEGORY_HOME );
        startActivity(homeIntent);
    } // showHome()
}
