/*
  Copyright (C) 2012 Stuffomatic Ltd. <contact@stuff-o-matic.com>

  All rights reserved.

  See the accompanying license file for details about usage, modification and
  distribution of this file.
*/
package com.stuffomatic.asgp;

import com.stuffomatic.ad.InterstitialService;

import org.libsdl.app.SDLActivity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.SystemClock;
import android.view.View;
import android.view.WindowManager;

import com.chartboost.sdk.Chartboost;
import com.facebook.FacebookSdk;
import com.facebook.appevents.AppEventsLogger;

import net.hockeyapp.android.Constants;
import net.hockeyapp.android.CrashManager;
import net.hockeyapp.android.NativeCrashManager;

import java.util.Map;

import com.stuffomatic.coasters.R;
import com.stuffomatic.event.EventManager;

public class ASGP extends SDLActivity
{
    static {
        System.loadLibrary( "gnustl_shared" );
        System.loadLibrary( "straining-coasters" );
    }

    private InterstitialService mInterstitialService;
    private Notifications mNotifications;
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        initializeHockeyApp();
        
        FacebookSdk.sdkInitialize( this );
        AppEventsLogger.activateApp( this );
        
        mInterstitialService = new InterstitialService( this );

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        hideActionBars();
        addVisibilityChangeListener();

        mNotifications = new Notifications( this );
    }

    public void openUrl( String url ) {

        final Intent browserIntent =
            new Intent( Intent.ACTION_VIEW, Uri.parse(url) );
        startActivity(browserIntent);
    }

    public void tagEvent( String tag, Map< String, String > properties ) {
        EventManager.tagEvent( tag, properties );
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

    public boolean hasExtraPlungers() {

        final long now = SystemClock.elapsedRealtime();
        final SharedPreferences preferences =
            getSharedPreferences( "power-up", Context.MODE_PRIVATE );

        return
            ( preferences.getLong( "extra-plungers-begin", 0 ) <= now )
            && ( now <= preferences.getLong( "extra-plungers-end", 0 ) );
    }
    
    @Override
    public void onStart() {
        super.onStart();
        Chartboost.onStart( this );

        mNotifications.scheduleNotifications();
    }

    @Override
    public void onResume() {
        super.onResume();
        CrashManager.register(this);
        Chartboost.onResume( this );
        hideActionBars();

        mNotifications.scheduleNotifications();
    }

    @Override
    public void onPause() {
        super.onPause();
        Chartboost.onPause( this );
    }

    @Override
    public void onStop() {
        super.onStop();
        Chartboost.onStop( this );
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Chartboost.onDestroy( this );
    }

    @Override
    public void onWindowFocusChanged( boolean hasFocus ) {
        super.onWindowFocusChanged( hasFocus );
        if ( hasFocus ) {
            hideActionBars();
        }
    }

    private void initializeHockeyApp() {

        CrashManager.register( this );

        Constants.loadFromContext( this );
        setUpBreakpad( Constants.FILES_PATH );

        checkForCrashes();
    }

    private native void setUpBreakpad( String filepath );
    
    private void checkForCrashes() {
        NativeCrashManager.handleDumpFiles
            ( this, getApplicationContext().getString
              ( R.string.hockeyapp_app_id ) );
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
