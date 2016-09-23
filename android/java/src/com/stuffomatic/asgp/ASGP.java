/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
package com.stuffomatic.asgp;

import com.stuffomatic.ad.InterstitialService;

import org.libsdl.app.SDLActivity;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;

import com.amplitude.api.Amplitude;

public class ASGP extends SDLActivity
{
    static {
        System.loadLibrary( "gnustl_shared" );
        System.loadLibrary( "straining-coasters" );
    }

    private InterstitialService mInterstitialService;
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Amplitude.getInstance().initialize
            ( this, "03de6dc65358e076d5ae691dc8748d74")
            .enableForegroundTracking(getApplication() );

        
        mInterstitialService = new InterstitialService( this );

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        hideActionBars();
        addVisibilityChangeListener();
    }

    public void openUrl( String url ) {

        final Intent browserIntent =
            new Intent( Intent.ACTION_VIEW, Uri.parse(url) );
        startActivity(browserIntent);
    }

    public void tagEvent( String tag ) {
        Amplitude.getInstance().logEvent( tag );
    }
        
    public void showHome() {

        final Intent homeIntent = new Intent( Intent.ACTION_MAIN );
        homeIntent.addCategory( Intent.CATEGORY_HOME );
        startActivity(homeIntent);
    }

    public String getDeviceModelName() {
        
        return Build.MANUFACTURER + ", " + Build.PRODUCT + ", " + Build.MODEL;
    }

    public void showInterstitial( long callback ) {
        
        mInterstitialService.show( callback );
    }

    @Override
    public void onResume() {
        super.onResume();
        hideActionBars();
    }

    @Override
    public void onWindowFocusChanged( boolean hasFocus ) {
        super.onWindowFocusChanged( hasFocus );
        if ( hasFocus ) {
            hideActionBars();
        }
    }

    private void hideActionBars() {

        getWindow().getDecorView().setSystemUiVisibility
            ( View.SYSTEM_UI_FLAG_LOW_PROFILE
              | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
              | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
              | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
              | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
              | View.SYSTEM_UI_FLAG_FULLSCREEN
              | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY );
    }

    private void addVisibilityChangeListener() {

        final View decorView = getWindow().getDecorView();

        decorView.setOnSystemUiVisibilityChangeListener
            ( new View.OnSystemUiVisibilityChangeListener() {
                @Override
                public void onSystemUiVisibilityChange( int visibility ) {
        
                    if ( (visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0 ) {
                        hideActionBars();
                    }
                }
            } );
    }
}
