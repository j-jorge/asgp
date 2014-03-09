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
import android.os.Build;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.AdListener;

public class ASGP extends SDLActivity
{
    /**
     * The view displaying the ads when the game tells us to.
     */
    private AdView mAdView;

    static {
        System.loadLibrary( "gnustl_shared" );
        System.loadLibrary( "andy-super-great-park" );
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        createAdView();
 
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().getDecorView().setSystemUiVisibility
            ( View.SYSTEM_UI_FLAG_LOW_PROFILE );
    } // onCreate()

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

    /**
     * Returns the name of the device on which the game is running.
     */
    public String getDeviceModelName() {
        
        return Build.MANUFACTURER + ", " + Build.PRODUCT + ", " + Build.MODEL;
    } // getDeviceModelName()

    @Override
    public void onPause() {
        mAdView.pause();
        super.onPause();
    } // onPause()

    @Override
    public void onResume() {
        super.onResume();
        mAdView.resume();
    } // onResume()

    @Override
    public void onDestroy() {
        mAdView.destroy();
        super.onDestroy();
    } // onDestroy()

    private void createAdView() {

        // Create the adView.
        mAdView = new AdView( this );
        
        mAdView.setAdUnitId( "ca-app-pub-8517534578027121/5449175693" );
        mAdView.setAdSize( AdSize.BANNER );

        RelativeLayout layout = new RelativeLayout( getContext() );
        layout.setGravity( Gravity.RIGHT | Gravity.BOTTOM );

        layout.setLayoutParams
            ( new RelativeLayout.LayoutParams
              ( RelativeLayout.LayoutParams.FILL_PARENT,
                RelativeLayout.LayoutParams.FILL_PARENT ) );

        layout.addView(mAdView);

        mLayout.addView( layout );

        requestAd();
    }

    private void requestAd() {

        // Initiate a generic request.
        AdRequest adRequest =
            new AdRequest.Builder()
            .addTestDevice( AdRequest.DEVICE_ID_EMULATOR )
            .build();

        // Load the adView with the ad request.
        mAdView.loadAd(adRequest);
    } // createAdView()
}
